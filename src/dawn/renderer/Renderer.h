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
    void RenderFrame(Camera* camera);

    // Handle an event
    void HandleEvent(EventDataPtr eventData);

    // Calculate a scene nodes AABB
    Ogre::AxisAlignedBox CalculateBounds(Ogre::SceneNode* node);

    // Get the MaterialPtr to a material
    Ogre::MaterialPtr GetMaterial(const String& name);

    // Get a new MaterialPtr which is a copy of another material
    Ogre::MaterialPtr GetMaterialCopy(const String& originalName, const String& newName);

    // Convert from scene coordinates to screen coordinates
    Vec2 ProjectPoint(const Position& point, Camera* camera);

    // Convert from scene coordinates to screen coordinates with depth
    Vec3 ProjectPointDepth(const Position& point, Camera* camera);

    // Convert from screen coordinates to scene coordinates
    // Screen position is in the range [0..1]
    Position UnprojectPoint(const Vec2& screenPosition, float depth, Camera* camera);

    // Determine if a screen position is visible in the current viewport
    // Screen position is in the range [0..1]
    bool InViewport(const Vec2& screenPosition);

    // Determine if a position is visible in the current viewport
    bool InViewport(const Position& point, Camera* camera);

    // Perform a raycast query against the scene bounding boxes by projecting from the camera to the
    // depth specified
    // Screen position in the range [0..1]
    bool RaycastPickScreen(const Vec2& screenPosition, float depth, Camera* camera, RendererRaycastResult& result);

    // Perform a raycast query against the scene bounding boxes between two points
    bool RaycastQueryAABB(const Vec3& start, const Vec3& end, Camera* camera, RendererRaycastResult& result);

    // Perform a raycast query against the scene geometry between two points
    // Be warned that this is quite an expensive operation
    bool RaycastQueryGeometry(const Vec3& start, const Vec3& end, Camera* camera, RendererRaycastResult& result);

    /// Post process effects
    void TogglePostEffect(const String& name, bool enabled);

    /// Internal: Sets the position of the ribbon trail root
    /// @param position The new root position
    DEPRECATED void SetRibbonTrailRootPosition(const Position& position);

    /// Creates a new ribbon trail object
    Ogre::SceneNode* GetRibbonTrailRoot();

    /// Query the graphics card for a List of resolutions supported
    Vector<SDL_DisplayMode> EnumerateDisplayModes() const;

    // Accessors
    uint GetWidth() const { return mViewport->getActualWidth(); }
    uint GetHeight() const { return mViewport->getActualHeight(); }
    Vec2i GetViewportSize() const { return Vec2i(GetWidth(), GetHeight()); }
    float GetAspect() const { return (float)GetWidth() / (float)GetHeight(); }
    Ogre::SceneManager* GetSceneMgr() { return mSceneManager; }
    Ogre::SceneNode* GetRootSceneNode() { return mRootNode; }
    Ogre::RenderWindow* GetWindow() { return mRenderWindow; }
 	Ogre::Viewport* GetViewport() { return mViewport; }
    SpriteManager* GetSpriteMgr() { return mSpriteManager; }
    DEPRECATED DeferredShadingManager* GetDeferredShadingMgr() { return mDeferredShadingMgr; }
    DEPRECATED Ogre::RenderSystem* GetOgreRenderSystem() const { return mRoot->getRenderSystem(); }

	/// Access the Resource Manager
	void AddResourceLocation(const String& location) { mResourceMgr->addResourceLocation(location, "FileSystem"); }

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
    void LoadPlugins();
    void CreateSDLWindow(const String& windowTitle, const Vec2i& displayMode, bool fullscreen,
                         Ogre::NameValuePairList& options);
    void InitResources(const String& basePath);
    void InitScene();

    // Needed for raycasting
    void FindClosestPolygon(Ogre::Entity* entity, float& closestDistance, Ogre::Vector3& position,
                             Ogre::Vector3& normal);
    void RayToTriangleCheck(Ogre::Vector3& corner1, Ogre::Vector3& corner2, Ogre::Vector3& corner3,
                             float& closestDistance, Ogre::Vector3& position,
                             Ogre::Vector3& normal);
};

NAMESPACE_END
