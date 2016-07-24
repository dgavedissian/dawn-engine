/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "SpriteManager.h"
#include "GeometryUtils.h"

NAMESPACE_BEGIN

enum QueryMasks
{
    UNIVERSE_OBJECT = 1 << 0
};

class Camera;
class DeferredShadingManager;
class Input;

struct DW_API RendererRaycastResult
{
    bool hit;
    Ogre::Entity* entity;
    Position position;
    Vec3 normal;
};

// Manages the Ogre rendering library and provides some helper functions
class DW_API Renderer
{
public:
    Renderer(const String& basePath, const String& prefPath, Input* inputMgr,
                 const String& windowTitle);
    ~Renderer();

    // Render one frame
    void renderFrame(Camera* camera);

    // Handle an event
    void handleEvent(EventDataPtr eventData);

    // Calculate a scene nodes AABB
    DEPRECATED Ogre::AxisAlignedBox calculateBounds(Ogre::SceneNode* node);

    // Get the MaterialPtr to a material
    DEPRECATED Ogre::MaterialPtr getMaterial(const String& name);

    // Get a new MaterialPtr which is a copy of another material
    DEPRECATED Ogre::MaterialPtr getMaterialCopy(const String& originalName, const String& newName);

    // Convert from scene coordinates to screen coordinates
    Vec2 projectPoint(const Position& point, Camera* camera);

    // Convert from scene coordinates to screen coordinates with depth
    Vec3 projectPointDepth(const Position& point, Camera* camera);

    // Convert from screen coordinates to scene coordinates
    // Screen position is in the range [0..1]
    Position unprojectPoint(const Vec2& screenPosition, float depth, Camera* camera);

    // Determine if a screen position is visible in the current viewport
    // Screen position is in the range [0..1]
    bool inViewport(const Vec2& screenPosition);

    // Determine if a position is visible in the current viewport
    bool inViewport(const Position& point, Camera* camera);

    // Perform a raycast query against the scene bounding boxes by projecting from the camera to the
    // depth specified
    // Screen position in the range [0..1]
    bool rayPickScreen(const Vec2& screenPosition, float depth, Camera* camera, RendererRaycastResult& result);

    // Perform a raycast query against the scene bounding boxes between two points
    bool rayQueryAabb(const Vec3& start, const Vec3& end, Camera* camera, RendererRaycastResult& result);

    // Perform a raycast query against the scene geometry between two points
    // Be warned that this is quite an expensive operation
    bool rayQueryGeometry(const Vec3& start, const Vec3& end, Camera* camera, RendererRaycastResult& result);

    /// Post process effects
    DEPRECATED void togglePostEffect(const String& name, bool enabled);

    /// Internal: Sets the position of the ribbon trail root
    /// @param position The new root position
    DEPRECATED void _setRibbonTrainPosition(const Position& position);

    /// Creates a new ribbon trail object
    Ogre::SceneNode* _getRibbonTrailRoot();

    /// Query the graphics card for a List of resolutions supported
    Vector<SDL_DisplayMode> getDeviceDisplayModes() const;

    // Accessors
    uint getWidth() const { return mViewport->getActualWidth(); }
    uint getHeight() const { return mViewport->getActualHeight(); }
    Vec2i getViewportSize() const { return Vec2i(getWidth(), getHeight()); }
    float getAspectRatio() const { return (float)getWidth() / (float)getHeight(); }
    SpriteManager* getSpriteMgr() { return mSpriteManager; }
    DEPRECATED Ogre::SceneManager* getSceneMgr() { return mSceneManager; }
    DEPRECATED Ogre::SceneNode* getRootSceneNode() { return mRootNode; }
    DEPRECATED Ogre::RenderWindow* getWindow() { return mRenderWindow; }
    DEPRECATED Ogre::Viewport* getViewport() { return mViewport; }
    DEPRECATED DeferredShadingManager* getDeferredShadingMgr() { return mDeferredShadingMgr; }
    DEPRECATED Ogre::RenderSystem* getOgreRenderSystem() const { return mRoot->getRenderSystem(); }

	/// Access the Resource Manager
    // TODO: Move this into a seperate class
	void addResourceLocation(const String& location) {
        LOG << "Adding location: " << location;
        mResourceMgr->addResourceLocation(location, "FileSystem");
    }
    void addResourcePrefix(const String& prefix) {
        Vector<String> rl;
        rl.push_back("fonts");
        rl.push_back("materials/deferred");
        rl.push_back("materials/explosions");
        rl.push_back("materials/scene");
        rl.push_back("materials");
        rl.push_back("models");
        rl.push_back("scripts");
        rl.push_back("textures");
        rl.push_back("ui");

        // Add locations
        for (String& location : rl)
            addResourceLocation(prefix + "/" + location);
	    mResourceMgr->initialiseAllResourceGroups();
    }

private:
    Input* mInputMgr;

    // Window
    SDL_Window* mWindow;

    // Ogre Log
    Ogre::LogManager* mLogManager;
    Ogre::Log* mLog;

    // Ogre
    Ogre::Root* mRoot;
    Ogre::SceneManager* mSceneManager;
    Ogre::SceneNode* mRootNode;
    Ogre::RenderWindow* mRenderWindow;
	Ogre::ResourceGroupManager* mResourceMgr;

    Ogre::Viewport* mViewport;
    Ogre::Camera* mDefaultCamera;

    Ogre::RaySceneQuery* mRaySceneQuery;

	// Ogre Plugins
	Ogre::GL3PlusPlugin* mRenderSystemPlugin;

    // Deferred Shading Pipeline
    DeferredShadingManager* mDeferredShadingMgr;

    // Post Processing
    Ogre::CompositorInstance* mHDRComp;
    Ogre::CompositorInstance* mBlurComp;

    // Sprite Manager
    SpriteManager* mSpriteManager;

    // Ribbon-trail root node
    // This is automatically set to the scene origin
    Ogre::SceneNode* mRTRoot;
    Position mRTRootPosition;

    // Set up
    void loadPlugins();
    void createSDLWindow(const String& windowTitle, const Vec2i& displayMode, bool fullscreen,
                         Ogre::NameValuePairList& options);
    void initScene();

    // Needed for raycasting
    void findClosestPolygon(Ogre::Entity* entity, float& closestDistance, Ogre::Vector3& position,
                            Ogre::Vector3& normal);
    void rayToTriangleCheck(Ogre::Vector3& corner1, Ogre::Vector3& corner2, Ogre::Vector3& corner3,
                            float& closestDistance, Ogre::Vector3& position,
                            Ogre::Vector3& normal);
};

NAMESPACE_END
