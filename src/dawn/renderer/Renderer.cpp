/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "input/Input.h"
#include "deferred/DeferredShadingManager.h"
#include "scene/Camera.h"
#include "Renderer.h"

#if DW_PLATFORM == DW_MAC_OSX
#include "OSXGetNSView.h"
#endif

#include <SDL_syswm.h>

NAMESPACE_BEGIN

Renderer::Renderer(const String& basePath, const String& prefPath,
                   Input* inputMgr, const String& windowTitle)
    : mInputMgr(inputMgr),
      mWindow(nullptr),
	  mLogManager(nullptr),
	  mLog(nullptr),
	  mRoot(nullptr),
      mSceneManager(nullptr),
	  mRootNode(nullptr),
      mRenderWindow(nullptr),
      mViewport(nullptr),
      mDefaultCamera(nullptr),
      mRaySceneQuery(nullptr),
	  mRenderSystemPlugin(nullptr),
      mRTRoot(nullptr),
      mRTRootPosition(Position::origin)
{
    // Read config file
    Vec2i displayMode = Config::get<Vec2i>("displaymode", Vec2i(1280, 800));
    bool fullscreen = Config::get<bool>("fullscreen", false);
    bool vsync = Config::get<bool>("vsync", true);

    // Set up Ogre's log manager
    mLogManager = new Ogre::LogManager();
	mLog = mLogManager->createLog(prefPath + "ogre.log", true, false, false);

    // Create the Ogre root
    mRoot = new Ogre::Root("", "");
    LOG << "Created Ogre Root";
    LOG << "\tVersion: " << OGRE_VERSION_MAJOR << "." << OGRE_VERSION_MINOR << "."
                << OGRE_VERSION_PATCH << " " << OGRE_VERSION_NAME;
	mResourceMgr = Ogre::ResourceGroupManager::getSingletonPtr();

    // PLUGIN STAGE
    loadPlugins();

    // Set the render system to the first available renderer
    if (mRoot->getAvailableRenderers().empty())
    {
        // TODO: THIS SHOULD NEVER HAPPEN
        assert(0);
    }
    mRoot->setRenderSystem(mRoot->getAvailableRenderers().front());

    // WINDOW CREATION STAGE

    // Window options
    Ogre::NameValuePairList options;
    options["vsync"] = vsync ? "true" : "false";
    options["gamma"] = "true";
    options["title"] = windowTitle;
    createSDLWindow(windowTitle, displayMode, fullscreen, options);

    // Initialise Ogre and use the SDL window
    mRoot->initialise(false, "", "");
    mRenderWindow = mRoot->createRenderWindow(windowTitle, displayMode.x, displayMode.y, fullscreen, &options);
    mRenderWindow->setVisible(true);

    // OGRE IS NOW READY, SET UP THE SCENE
    addResourcePrefix(basePath + "engine/media/base", "DawnEngine");
    initScene();

    // Set up the raycast query object
    mRaySceneQuery = mSceneManager->createRayQuery(Ogre::Ray(), Ogre::SceneManager::WORLD_GEOMETRY_TYPE_MASK);
    mRaySceneQuery->setSortByDistance(true);
    mRaySceneQuery->setQueryMask(UNIVERSE_OBJECT);

    // Set up the deferred shading system
    mDeferredShadingMgr = new DeferredShadingManager(mViewport, mSceneManager);

    // Set up post processing
    mHDRComp = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "HDR");
    mBlurComp = Ogre::CompositorManager::getSingleton().addCompositor(mViewport, "GaussianBlur");
    mHDRComp->setEnabled(true);
    //mBlurComp->setEnabled(true);

    // Set up the sprite manager
    mSpriteManager = new SpriteManager(mViewport, mSceneManager);

    // Set up the root scene nodes
    mRootNode = mSceneManager->getRootSceneNode();
    mRTRoot = mRootNode->createChildSceneNode();

    // Add event delegates
    ADD_LISTENER(Renderer, EvtData_KeyDown);
}

Renderer::~Renderer()
{
    REMOVE_LISTENER(Renderer, EvtData_KeyDown);

    // Delete the sprite manager and deferred shading manager
    SAFE_DELETE(mSpriteManager);
    SAFE_DELETE(mDeferredShadingMgr);

    // Clean up Ogre
    mRoot->destroySceneManager(mSceneManager);
	SAFE_DELETE(mRoot);
	SAFE_DELETE(mLogManager);
	SAFE_DELETE(mRenderSystemPlugin);
    LOG << "Ogre cleaned up";

    // Destroy the SDL window
    SDL_SetWindowFullscreen(mWindow, 0);
    SDL_DestroyWindow(mWindow);
}

void Renderer::renderFrame(Camera* camera)
{
    // Pump Events
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDL_QUIT:
            EventSystem::inst().queueEvent(makeShared<EvtData_Exit>());
            break;

        case SDL_WINDOWEVENT:
            switch (e.window.event)
            {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                int w, h;
                SDL_GetWindowSize(mWindow, &w, &h);
#if DW_PLATFORM == DW_LINUX
                mRenderWindow->resize(w, h);
#else
                mRenderWindow->windowMovedOrResized();
#endif
                break;

            case SDL_WINDOWEVENT_RESIZED:
#if DW_PLATFORM == DW_LINUX
                mRenderWindow->resize(e.window.data1, e.window.data2);
#else
                mRenderWindow->windowMovedOrResized();
#endif
                break;

            case SDL_WINDOWEVENT_CLOSE:
                break;

            case SDL_WINDOWEVENT_SHOWN:
                mRenderWindow->setVisible(true);
                break;

            case SDL_WINDOWEVENT_HIDDEN:
                mRenderWindow->setVisible(false);
                break;

            default:
                break;
            }
            break;

        default:
            mInputMgr->handleSDLEvent(e);
            break;
        }
    }

    // Update ribbon trail root node
	if (camera)
	{
		mRTRoot->setPosition(mRTRootPosition.toCameraSpace(camera));
		if (mViewport->getCamera() != camera->_getOgreCamera())
			mViewport->setCamera(camera->_getOgreCamera());
	}

    // Draw a frame
    mRoot->renderOneFrame();
}

void Renderer::handleEvent(EventDataPtr eventData)
{
    if (eventIs<EvtData_KeyDown>(eventData))
    {
        auto castedEventData = castEvent<EvtData_KeyDown>(eventData);
        switch (castedEventData->keycode)
        {
        case SDLK_F4:
            // Toggle deferred shading debug mode
            mDeferredShadingMgr->SetDebugMode(!mDeferredShadingMgr->IsDebugMode());
            //mHDRComp->setEnabled(!mDeferredShadingMgr->IsDebugMode());
            break;

        case SDLK_p:
            // Take a screenshot
            mRenderWindow->writeContentsToTimestampedFile("Transcendent", ".png");
            break;

        case SDLK_h:
            // Toggle HDR
            mHDRComp->setEnabled(!mHDRComp->getEnabled());
            break;

        default:
            break;
        }
    }
}

void mergeBounds(Ogre::SceneNode* node, Ogre::SceneNode* rootNode,
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
        mergeBounds(static_cast<Ogre::SceneNode*>(child.getNext()), rootNode, currentTransform, aabb);
    }
}

Ogre::AxisAlignedBox Renderer::calculateBounds(Ogre::SceneNode* node)
{
    // As not all scene managers guarantee a hierarchal AABB, calculate this
    // ourselves using a recursive function
    Ogre::AxisAlignedBox aabb = Ogre::AxisAlignedBox::BOX_NULL;
    mergeBounds(node, node, Ogre::Matrix4::IDENTITY, aabb);
    return aabb;
}

Ogre::MaterialPtr Renderer::getMaterial(const String& name)
{
    return Ogre::MaterialManager::getSingleton().getByName(name);
}

Ogre::MaterialPtr Renderer::getMaterialCopy(const String& originalName, const String& newName)
{
    Ogre::MaterialPtr material = getMaterial(originalName);
    if (material.isNull())
        return material;
    return material->clone(newName);
}

Vec2 Renderer::projectPoint(const Position& point, Camera* camera)
{
    // Convert to screen coords and discard depth
    Vec3 screenPosition = projectPointDepth(point, camera);
    return Vec2(screenPosition.x, screenPosition.y);
}

Vec3 Renderer::projectPointDepth(const Position& point, Camera* camera)
{
    // Transform point from world space to screen space
    Vec3 cameraSpace = camera->getViewMatrix().TransformPos(point.toCameraSpace(camera));
    Vec3 screenPosition = camera->getProjMatrix().TransformPos(cameraSpace);

    // Convert point from device coordinates to normalised coordinates
    return Vec3((screenPosition.x + 1.0f) * 0.5f, (1.0f - screenPosition.y) * 0.5f, cameraSpace.z);
}

Position Renderer::unprojectPoint(const Vec2& screenPosition, float depth, Camera* camera)
{
    auto ray = camera->_getOgreCamera()->getCameraToViewportRay(screenPosition.x, screenPosition.y);
    return Position::fromCameraSpace(camera, ray.getPoint(depth));
}

bool Renderer::inViewport(const Vec2& screenPosition)
{
    return screenPosition.x >= 0.0f && screenPosition.x <= 1.0f && screenPosition.y >= 0.0f &&
           screenPosition.y <= 1.0f;
}

bool Renderer::inViewport(const Position& point, Camera* camera)
{
    Vec3 screenPosition = projectPointDepth(point, camera);
    return inViewport(Vec2(screenPosition.x, screenPosition.y)) && screenPosition.z < 0.0f;
}

bool Renderer::rayPickScreen(const Vec2& screenPosition, float depth, Camera* camera,
                                     RendererRaycastResult& result)
{
    // Set up the ray query object
    Ogre::Camera* ogreCamera = camera->_getOgreCamera();
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
            result.position = Position::fromCameraSpace(camera, ray.getPoint((*i).distance));
            result.normal = Vec3::zero;
            return true;
        }
    }

    // At this point - the ray didn't hit anything of interest
    result.hit = false;
    result.entity = nullptr;
    result.position = Position::fromCameraSpace(camera, ray.getPoint(depth));
    result.normal = Vec3::zero;
    return false;
}

bool Renderer::rayQueryAabb(const Vec3& start, const Vec3& end, Camera* camera,
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
            result.position = Position::fromCameraSpace(camera, ray.getPoint((*i).distance));
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

bool Renderer::rayQueryGeometry(const Vec3& start, const Vec3& end, Camera* camera,
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
            result.position = Position::fromCameraSpace(camera, ray.getPoint((*i).distance));
            result.normal = Vec3::zero;
            return true;
        }
    }

    // At this point - the ray didn't hit anything of interest
    result.hit = false;
    result.entity = nullptr;
    result.position = camera->getPosition();
    result.normal = Vec3::zero;
    return false;
}

void Renderer::togglePostEffect(const String& name, bool enabled)
{
    //Ogre::CompositorManager::getSingleton().setCompositorEnabled(mViewport, name, enabled);
}

void Renderer::_setRibbonTrainPosition(const Position& position)
{
    mRTRootPosition = position;
}

Ogre::SceneNode* Renderer::_getRibbonTrailRoot()
{
    return mRTRoot;
}

Vector<SDL_DisplayMode> Renderer::getDeviceDisplayModes() const
{
    // Enumerate available video modes
    Vector<SDL_DisplayMode> displayModes;
    int count = SDL_GetNumDisplayModes(0);
    for (int i = 0; i < count; ++i)
    {
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, i, &mode);
        displayModes.push_back(mode);
    }
    return displayModes;
}

void Renderer::loadPlugins()
{
	mRenderSystemPlugin = new Ogre::GL3PlusPlugin();
	mRoot->installPlugin(mRenderSystemPlugin);
}

void Renderer::createSDLWindow(const String& windowTitle, const Vec2i& displayMode,
                                   bool fullscreen, Ogre::NameValuePairList& options)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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

    // Get the native window handle
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(mWindow, &wmInfo);

    String winHandle;
    switch (wmInfo.subsystem)
    {
#if DW_PLATFORM == DW_WIN32
    case SDL_SYSWM_WINDOWS:
        winHandle = Ogre::StringConverter::toString((unsigned long)wmInfo.info.win.window);
        break;
#elif DW_PLATFORM == DW_MAC_OSX
    case SDL_SYSWM_COCOA:
        options["macAPI"] = "cocoa";
        options["macAPICocoaUseNSView"] = "true";
        winHandle = Ogre::StringConverter::toString(getWindowContentViewHandle(wmInfo));
        break;
#elif DW_PLATFORM == DW_LINUX
    case SDL_SYSWM_X11:
        winHandle = Ogre::StringConverter::toString((unsigned long)wmInfo.info.x11.window);
        break;
#else
#   error Unhandled SDL2 platform
#endif
    default:
        // TODO: Error!
        break;
    }

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    options["externalWindowHandle"] = winHandle;
#else
    options["parentWindowHandle"] = winHandle;
#endif
}

void Renderer::initScene()
{
    // Set up the scene manager
    mSceneManager = mRoot->createSceneManager(Ogre::ST_GENERIC);
    mSceneManager->setAmbientLight(Ogre::ColourValue(0.02f, 0.02f, 0.02f));

    // Set up the viewport and default camera
    mDefaultCamera = mSceneManager->createCamera("DefaultCamera");
    mViewport = mRenderWindow->addViewport(mDefaultCamera);
    mViewport->setBackgroundColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f));
}

void Renderer::findClosestPolygon(Ogre::Entity* entity, float& closestDistance,
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

        // Ignore anything that isn't a triangle List
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
                rayToTriangleCheck(vertex1, vertex2, vertexPos, closestDistance, position, normal);
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

void Renderer::rayToTriangleCheck(Ogre::Vector3& corner1, Ogre::Vector3& corner2,
                                       Ogre::Vector3& corner3, float& closestDistance,
                                       Ogre::Vector3& position, Ogre::Vector3& normal)
{
    // Check for a hit against this triangle
    Pair<bool, float> hit =
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
