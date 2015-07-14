/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Input/InputManager.h"
#include "Deferred/DeferredShadingManager.h"
#include "Scene/Camera.h"
#include "RenderSystem.h"

#if DW_PLATFORM == DW_MAC_OSX
#include "OSXGetNSView.h"
#endif

#include <SDL_syswm.h>

NAMESPACE_BEGIN

RenderSystem::RenderSystem(const string& basePath, const string& prefPath,
                           InputManager* inputMgr, const string& windowTitle)
    : mInputMgr(inputMgr),
      mWindow(nullptr),
      mSceneManager(nullptr),
      mRenderWindow(nullptr),
      mViewport(nullptr),
      mDefaultCamera(nullptr),
      mRaySceneQuery(nullptr),
      mRTRoot(nullptr),
      mRTRootPosition(Position::origin)
{
    // Read config file
    Vec2i displayMode = Config::Get<Vec2i>("displaymode", Vec2i(1280, 800));
    bool fullscreen = Config::Get<bool>("fullscreen", false);
    bool vsync = Config::Get<bool>("vsync", true);

    // Set up Ogre's log manager
    mLogManager = make_shared<Ogre::LogManager>();
    mLog = mLogManager->createLog(prefPath + "ogre.log", true, false, false);

    // Create the Ogre root
    mRoot = make_shared<Ogre::Root>("", "");
    LOG << "Created Ogre Root";
    LOG << "\tVersion: " << OGRE_VERSION_MAJOR << "." << OGRE_VERSION_MINOR << "."
                << OGRE_VERSION_PATCH << " " << OGRE_VERSION_NAME;

    // PLUGIN STAGE
    LoadPlugins();

    // Set the render system to the first available renderer
    if (mRoot->getAvailableRenderers().empty())
    {
        // TODO: ERROR
        assert(0);
    }
    mRoot->setRenderSystem(mRoot->getAvailableRenderers().front());

    // WINDOW CREATION STAGE

    // Window options
    Ogre::NameValuePairList options;
    options["vsync"] = vsync ? "true" : "false";
    options["gamma"] = "true";
    CreateSDLWindow(windowTitle, displayMode, fullscreen, options);

    // Initialise Ogre and use the SDL window
    mRoot->initialise(false, "", "");
    mRenderWindow = mRoot->createRenderWindow("", displayMode.x, displayMode.y, fullscreen, &options);
    mRenderWindow->setVisible(true);

    // OGRE IS NOW READY, SET UP THE SCENE
    InitResources(basePath);
    InitScene();

    // Set up the raycast query object
    mRaySceneQuery =
        mSceneManager->createRayQuery(Ogre::Ray(), Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK);
    mRaySceneQuery->setSortByDistance(true);
    mRaySceneQuery->setQueryMask(UNIVERSE_OBJECT);

    // Set up the deferred shading system
    mDeferredShadingMgr = make_shared<DeferredShadingManager>(mViewport, mSceneManager);

    // Set up the sprite manager
    mSpriteManager = make_shared<SpriteManager>(mViewport, mSceneManager);

    // Set up the root scene nodes
    mRootNode = mSceneManager->getRootSceneNode();
    mRTRoot = mRootNode->createChildSceneNode();

    // Set up post processing
    Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "DOF");

    // Add event delegates
    ADD_LISTENER(RenderSystem, EvtData_KeyDown);
}

RenderSystem::~RenderSystem()
{
    REMOVE_LISTENER(RenderSystem, EvtData_KeyDown);

    // Delete the sprite manager and deferred shading manager
    mSpriteManager.reset();
    mDeferredShadingMgr.reset();

    // Clean up Ogre
    mRoot->destroySceneManager(mSceneManager);
#if DW_PLATFORM != DW_MAC_OSX    // TODO: Fix OgreOSXCocoaWindow
    mRenderWindow->destroy();
#endif
    mRoot.reset();
    mLogManager.reset();
    LOG << "Ogre cleaned up";

    // Destroy the SDL window
    SDL_DestroyWindow(mWindow);
}

void RenderSystem::RenderFrame(Camera* camera)
{
    // Pump Events
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDL_QUIT:
            EventSystem::inst().QueueEvent(make_shared<EvtData_Exit>());
            break;

        default:
            mInputMgr->HandleSDLEvent(e);
            break;
        }
    }

    // Update Ribbon trail root node
    mRTRoot->setPosition(mRTRootPosition.ToCameraSpace(camera));

    // Draw a frame
    if (mViewport->getCamera() != camera->GetOgreCamera())
        mViewport->setCamera(camera->GetOgreCamera());
    mRoot->renderOneFrame();
}

void RenderSystem::HandleEvent(EventDataPtr eventData)
{
    if (EventIs<EvtData_KeyDown>(eventData))
    {
        auto castedEventData = static_pointer_cast<EvtData_KeyDown>(eventData);
        switch (castedEventData->keycode)
        {
        case SDLK_F4:
            // Toggle deferred shading debug mode
            mDeferredShadingMgr->SetDebugMode(!mDeferredShadingMgr->IsDebugMode());
            break;

        case SDLK_p:
            // Take a screenshot
            mRenderWindow->writeContentsToTimestampedFile("Transcendent", ".png");
            break;

        default:
            break;
        }
    }
}

void MergeBounds(Ogre::SceneNode* node, Ogre::SceneNode* rootNode,
                  const Ogre::Matrix4& parentTransform, Ogre::AxisAlignedBox& aabb)
{
    // Get this nodes current transform
    Ogre::Matrix4 currentTransform = parentTransform;
    if (node != rootNode)
    {
        Ogre::Matrix4 localTransform(node->getOrientation());
        localTransform.setTrans(node->getPosition());
        currentTransform = currentTransform * localTransform;
    }

    // Merge this nodes objects
    Ogre::SceneNode::ObjectIterator object = node->getAttachedObjectIterator();
    while (object.hasMoreElements())
    {
        Ogre::AxisAlignedBox localAABB = object.getNext()->getBoundingBox();
        localAABB.transform(currentTransform);
        aabb.merge(localAABB);
    }

    // Iterate through all children and call this function on them
    Ogre::SceneNode::ChildNodeIterator child = node->getChildIterator();
    while (child.hasMoreElements())
    {
        MergeBounds(static_cast<Ogre::SceneNode*>(child.getNext()), rootNode, currentTransform,
                     aabb);
    }
}

Ogre::AxisAlignedBox RenderSystem::CalculateBounds(Ogre::SceneNode* node)
{
    // As not all scene managers guarantee a hierarchal AABB, calculate this
    // ourselves using a recursive function
    Ogre::AxisAlignedBox aabb = Ogre::AxisAlignedBox::BOX_NULL;
    MergeBounds(node, node, Ogre::Matrix4::IDENTITY, aabb);
    return aabb;
}

Ogre::MaterialPtr RenderSystem::GetMaterial(const string& name)
{
    return Ogre::MaterialManager::getSingleton().getByName(name);
}

Ogre::MaterialPtr RenderSystem::GetMaterialCopy(const string& originalName, const string& newName)
{
    Ogre::MaterialPtr material = GetMaterial(originalName);
    if (material.isNull())
        return material;
    return material->clone(newName);
}

Vec2 RenderSystem::ProjectPoint(const Position& point, Camera* camera)
{
    // Convert to screen coords and discard depth
    Vec3 screenPosition = ProjectPointDepth(point, camera);
    return Vec2(screenPosition.x, screenPosition.y);
}

Vec3 RenderSystem::ProjectPointDepth(const Position& point, Camera* camera)
{
    // Transform point from world space to screen space
    Vec3 cameraSpace = camera->GetViewMatrix().TransformPos(point.ToCameraSpace(camera));
    Vec3 screenPosition = camera->GetProjMatrix().TransformPos(cameraSpace);

    // Convert point from device coordinates to normalised coordinates
    return Vec3((screenPosition.x + 1.0f) * 0.5f, (1.0f - screenPosition.y) * 0.5f, cameraSpace.z);
}

Position RenderSystem::UnprojectPoint(const Vec2& screenPosition, float depth, Camera* camera)
{
    auto ray = camera->GetOgreCamera()->getCameraToViewportRay(screenPosition.x, screenPosition.y);
    return Position::FromCameraSpace(camera, ray.getPoint(depth));
}

bool RenderSystem::InViewport(const Vec2& screenPosition)
{
    return screenPosition.x >= 0.0f && screenPosition.x <= 1.0f && screenPosition.y >= 0.0f &&
           screenPosition.y <= 1.0f;
}

bool RenderSystem::InViewport(const Position& point, Camera* camera)
{
    Vec3 screenPosition = ProjectPointDepth(point, camera);
    return InViewport(Vec2(screenPosition.x, screenPosition.y)) && screenPosition.z < 0.0f;
}

bool RenderSystem::RaycastPickScreen(const Vec2& screenPosition, float depth, Camera* camera,
                                     RendererRaycastResult& result)
{
    // Set up the ray query object
    Ogre::Camera* ogreCamera = camera->GetOgreCamera();
    Ogre::Ray ray = ogreCamera->getCameraToViewportRay(screenPosition.x, screenPosition.y);
    mRaySceneQuery->setRay(ray);
    mRaySceneQuery->setQueryMask(~UNIVERSE_OBJECT);

    // Execute the ray query
    Ogre::RaySceneQueryResult& rayQuery = mRaySceneQuery->execute();
    for (auto i = rayQuery.begin(); i != rayQuery.end(); ++i)
    {
        Ogre::Entity* entity = dynamic_cast<Ogre::Entity*>((*i).movable);

        if (entity && (*i).distance > 0.0f)
        {
            result.hit = true;
            result.entity = entity;
            result.position = Position::FromCameraSpace(camera, ray.getPoint((*i).distance));
            result.normal = Vec3::zero;
            return true;
        }
    }

    // At this point - the ray didn't hit anything of interest
    result.hit = false;
    result.entity = nullptr;
    result.position = Position::FromCameraSpace(camera, ray.getPoint(depth));
    result.normal = Vec3::zero;
    return false;
}

bool RenderSystem::RaycastQueryAABB(const Vec3& start, const Vec3& end, Camera* camera,
                                    RendererRaycastResult& result)
{
    // Create the ray
    Vec3 direction = end - start;
    Ogre::Ray ray(start, direction);

    // Set up the ray query object
    mRaySceneQuery->setRay(ray);
    mRaySceneQuery->setQueryMask(~UNIVERSE_OBJECT);

    // Execute the ray query
    Ogre::RaySceneQueryResult& rayQuery = mRaySceneQuery->execute();
    for (auto i = rayQuery.begin(); i != rayQuery.end(); ++i)
    {
        Ogre::Entity* entity = dynamic_cast<Ogre::Entity*>((*i).movable);

        if (entity)
        {
            result.hit = true;
            result.entity = entity;
            result.position = Position::FromCameraSpace(camera, ray.getPoint((*i).distance));
            result.normal = Vec3::zero;
            return true;
        }
    }

    // At this point - the ray didn't hit anything of interest
    result.hit = false;
    result.entity = nullptr;
    result.position = Vec3::zero;
    result.normal = Vec3::zero;
    return false;
}

bool RenderSystem::RaycastQueryGeometry(const Vec3& start, const Vec3& end, Camera* camera,
                                        RendererRaycastResult& result)
{
    // Create the ray
    Vec3 direction = end - start;
    Ogre::Ray ray(start, direction);

    // Set up the ray query object
    mRaySceneQuery->setRay(ray);
    mRaySceneQuery->setQueryMask(~UNIVERSE_OBJECT);

    // Execute the ray query
    Ogre::RaySceneQueryResult& rayQuery = mRaySceneQuery->execute();
    for (auto i = rayQuery.begin(); i != rayQuery.end(); ++i)
    {
        Ogre::Entity* entity = dynamic_cast<Ogre::Entity*>((*i).movable);

        if (entity)
        {
            result.hit = true;
            result.entity = entity;
            result.position = Position::FromCameraSpace(camera, ray.getPoint((*i).distance));
            result.normal = Vec3::zero;
            return true;
        }
    }

    // At this point - the ray didn't hit anything of interest
    result.hit = false;
    result.entity = nullptr;
    result.position = camera->GetPosition();
    result.normal = Vec3::zero;
    return false;
}

void RenderSystem::TogglePostEffect(const string& name, bool enabled)
{
    Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, name, enabled);
}

void RenderSystem::SetRibbonTrailRootPosition(const Position& position)
{
    mRTRootPosition = position;
}

Ogre::SceneNode* RenderSystem::GetRibbonTrailRoot()
{
    return mRTRoot;
}

vector<SDL_DisplayMode> RenderSystem::EnumerateDisplayModes() const
{
    // Enumerate available video modes
    vector<SDL_DisplayMode> displayModes;
    int count = SDL_GetNumDisplayModes(0);
    for (int i = 0; i < count; ++i)
    {
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, i, &mode);
        displayModes.push_back(mode);
    }
    return displayModes;
}

void RenderSystem::LoadPlugins()
{
    std::vector<string> plugins;
    plugins.push_back("RenderSystem_GL");
#if DW_PLATFORM == DW_WIN32
    plugins.push_back("Plugin_ParticleUniverse");
#endif
    LOG << "Plugins:";
    for (auto& plugin : plugins)
    {
#if DW_PLATFORM == DW_WIN32 && defined(DW_DEBUG)
        plugin.append("_d");
#endif
        mRoot->loadPlugin(plugin);
        LOG << "\t" << plugin;
    }

    // Work around the missing msSingleton assert for ParticleUniverse
#if DW_PLATFORM != DW_WIN32
    mParticleUniversePlugin = make_shared<ParticleUniverse::ParticleUniversePlugin>();
    mRoot->installPlugin(mParticleUniversePlugin.get());    
#endif
}

void RenderSystem::CreateSDLWindow(const string& windowTitle, const Vec2i& displayMode,
                                   bool fullscreen, Ogre::NameValuePairList& options)
{
    // Create the window
    int windowFlags = 0;
    if (fullscreen)
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    mWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               displayMode.x, displayMode.y, windowFlags);

    // Check that the window was successfully created
    if (mWindow == nullptr)
    {
        // TODO: Error
        assert(0);
    }

    // Pass SDL information to Ogre
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(mWindow, &wmInfo);
#if DW_PLATFORM == DW_WIN32
    unsigned long winHandle = reinterpret_cast<unsigned long>(wmInfo.info.win.window);
    options["externalWindowHandle"] = Ogre::StringConverter::toString(winHandle);
#elif DW_PLATFORM == DW_MAC_OSX
    unsigned long winHandle = getWindowContentViewHandle(wmInfo);
    options["macAPI"] = "cocoa";
    options["macAPICocoaUseNSView"] = "true";
    options["externalWindowHandle"] = Ogre::StringConverter::toString(winHandle);
#elif DW_PLATFORM == DW_LINUX
    unsigned long winHandle = reinterpret_cast<unsigned long>(wmInfo.info.x11.window);
    options["parentWindowHandle"] = Ogre::StringConverter::toString(winHandle);
#else
#   error Unhandled SDL2 platform
#endif
}

void RenderSystem::InitResources(const string& basePath)
{
    // Add all resource locations to the resource group manager
    // TODO move resource manager to global class
    std::vector<string> rl;
    rl.push_back("Media/fonts/");
    rl.push_back("Media/materials/deferred/");
    rl.push_back("Media/materials/explosions/pu/");
    rl.push_back("Media/materials/explosions/");
    rl.push_back("Media/materials/scene/");
    rl.push_back("Media/materials/ship/");
    rl.push_back("Media/materials/");
    rl.push_back("Media/models/");
    rl.push_back("Media/scripts/");
    rl.push_back("Media/scripts/gamemodes");
    rl.push_back("Media/scripts/gui");
    rl.push_back("Media/textures/");
    rl.push_back("Media/textures/scene");
    rl.push_back("Media/textures/ui");
    LOG << "Resource Locations:";
    for (auto& resourceLocation : rl)
    {
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                basePath + resourceLocation, "FileSystem", "General");
        LOG << "\t" << resourceLocation;
    }

    // Initialise resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    LOG << "Initialised resources";
}

void RenderSystem::InitScene()
{
    // Set up the scene manager
    mSceneManager = mRoot->createSceneManager(Ogre::ST_GENERIC);
    mSceneManager->setAmbientLight(Ogre::ColourValue(0.02f, 0.02f, 0.02f));

    // Set up the viewport and default camera
    mDefaultCamera = mSceneManager->createCamera("DefaultCamera");
    mViewport = mRenderWindow->addViewport(mDefaultCamera);
    mViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));
}

void RenderSystem::FindClosestPolygon(Ogre::Entity* entity, float& closestDistance,
                                       Ogre::Vector3& position, Ogre::Vector3& normal)
{
    closestDistance = std::numeric_limits<float>::max();    // default value (means
                                                            // nothing detected)

    // Get transformation
    Ogre::SceneNode* parentNode = entity->getParentSceneNode();
    Ogre::Vector3 parentPos;
    Ogre::Quaternion parentOrientation;
    Ogre::Vector3 parentScale;
    if (parentNode)
    {
        parentPos = parentNode->_getDerivedPosition();
        parentOrientation = parentNode->_getDerivedOrientation();
        parentScale = parentNode->_getDerivedScale();
    }
    else
    {
        parentPos = Ogre::Vector3::ZERO;
        parentOrientation = Ogre::Quaternion::IDENTITY;
        parentScale = Ogre::Vector3::UNIT_SCALE;
    }

    // Handle animated entities
    bool isAnimated = entity->hasSkeleton();
    if (isAnimated)
    {
        entity->addSoftwareAnimationRequest(false);
        entity->_updateAnimation();
    }

    // Loop through each submesh
    Ogre::MeshPtr mesh = entity->getMesh();
    for (uint i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* subMesh = mesh->getSubMesh(i);

        // Ignore anything that isn't a triangle list
        if (subMesh->operationType != Ogre::RenderOperation::OT_TRIANGLE_LIST)
            continue;

        // Get the vertex data
        Ogre::VertexData* vertexData;
        if (subMesh->useSharedVertices)
        {
            if (isAnimated)
                vertexData = entity->_getSkelAnimVertexData();
            else
                vertexData = mesh->sharedVertexData;
        }
        else
        {
            if (isAnimated)
                vertexData = entity->getSubEntity(i)->_getSkelAnimVertexData();
            else
                vertexData = subMesh->vertexData;
        }

        // Get the size of one vertex
        const Ogre::VertexElement* posEl =
            vertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
        Ogre::HardwareVertexBufferSharedPtr vBuff =
            vertexData->vertexBufferBinding->getBuffer(posEl->getSource());
        uint vertexSize = vBuff->getVertexSize();

        // Save pointer to first vertex
        short* pVertex = (short*)vBuff->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
        short* pStartVertex = pVertex;

        // Get the index buffer
        // If it is null then skip as it must be a point cloud
        Ogre::HardwareIndexBufferSharedPtr iBuff = subMesh->indexData->indexBuffer;

        if (iBuff.isNull())
            continue;

        uint* pLong = (uint*)iBuff->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);
        uint16_t* pShort = (uint16_t*)pLong;

        // Look through each vertex and check each triangle with the ray
        Ogre::Vector3 vertexPos;
        Ogre::Vector3 vertex1;
        Ogre::Vector3 vertex2;
        float* pReal;
        uint index;

        for (uint k = 0; k < subMesh->indexData->indexCount; k++)
        {
            // Read index value
            if (iBuff->getType() == Ogre::HardwareIndexBuffer::IT_32BIT)    // if 32bit indexes
            {
                index = (uint)pLong[k];
            }
            else
            {
                index = (uint)pShort[k];
            }

            // Read referenced vertex
            pVertex = pStartVertex + (vertexSize * index);              // calculate pointer
            posEl->baseVertexPointerToElement(pVertex, &pReal);         // read vertex
            vertexPos = Ogre::Vector3(pReal[0], pReal[1], pReal[2]);    // read position values

            // Apply world transformations
            if (parentNode)
                vertexPos = (parentOrientation * (vertexPos * parentScale)) + parentPos;

            // Figure out triangle and calculate the distance if it's the closest
            switch (k % 3)
            {
            case 0:
                vertex1 = vertexPos;
                break;

            case 1:
                vertex2 = vertexPos;
                break;

            case 2:
                RayToTriangleCheck(vertex1, vertex2, vertexPos, closestDistance, position, normal);
                break;

            default:
                break;
            }
        }

        iBuff->unlock();
        vBuff->unlock();
    }

    if (isAnimated)
    {
        entity->removeSoftwareAnimationRequest(false);
    }
}

void RenderSystem::RayToTriangleCheck(Ogre::Vector3& corner1, Ogre::Vector3& corner2,
                                       Ogre::Vector3& corner3, float& closestDistance,
                                       Ogre::Vector3& position, Ogre::Vector3& normal)
{
    // Check for a hit against this triangle
    std::pair<bool, float> hit =
        Ogre::Math::intersects(mRaySceneQuery->getRay(), corner1, corner2, corner3, true, false);

    // Check if closest distance
    if (hit.first == true && hit.second < closestDistance)
    {
        // Save distance
        closestDistance = hit.second;

        // Calculate normal and position
        position = mRaySceneQuery->getRay().getPoint(closestDistance);
        Ogre::Vector3 v1 = corner1 - corner2;
        Ogre::Vector3 v2 = corner3 - corner2;
        normal = v2.crossProduct(v1).normalisedCopy();
    }
}

NAMESPACE_END
