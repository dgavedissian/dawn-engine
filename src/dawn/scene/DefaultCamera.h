/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#pragma once

#include "Camera.h"

NAMESPACE_BEGIN

class Renderer;
class InputSystem;
class SceneManager;

// TODO: Removing these would be great
enum CameraControlState
{
    CCS_MANUAL,         // Manual Camera Control
    CCS_FREE,           // Free Camera
    CCS_TRACK_FIXED,    // Track a point with a fixed rotation
    CCS_TRACK_FREE,     // Track a point with free rotation
    CCS_CUSTOM_ID       // Custom camera control state
};

// When using CCS_FREE or CCS_TRACK_FREE, this flag controls when the orientation changes
enum CameraDragEventState
{
    CDES_ALWAYS,
    CDES_LMB_UP,
    CDES_LMB_DOWN,
    CDES_RMB_UP,
    CDES_RMB_DOWN
};

class DW_API DefaultCamera : public Camera
{
public:
    DefaultCamera(Renderer* renderSystem, Input* inputMgr, SceneManager* sceneMgr);
    virtual ~DefaultCamera();

    /// Control State

    /// Sets the control state to use
    /// @param state State identifier
    void setControlState(uint state);

    /// Gets the current control state
    /// @returns The current control state
    uint getControlState() const;

    /// Sets the drag event state, to control when dragging affects the
    /// orientation of the camera
    /// @param state State identifier
    void setDragEventState(uint state);

    /// Internal: Updates the drag state flag
    void setDraggingFlag(bool flag);

    /// Sets the movement speed of the camera in free mode
    /// @param speed Speed in metres per second
    void setMovementSpeed(float speed);

    /// Gets the movement speed of the camera in free mode
    /// @returns The speed of the camera in metres per second
    float getMovementSpeed() const;

    /// Sets the tracking target to a fixed point in track mode
    /// @param point Point to track
    /// @param ffset Offset of the camera from the point
    void setTrackingTarget(const Position& point, const Vec3& offset);

    /// Sets the tracking target to a fixed point in track mode, with angle
    /// limits
    /// about the X axis. This also switches the rotation to FPS mode
    /// @param point Point to track
    /// @param offset Offset of the camera from the point
    /// @param limits The limits of the X angle (min, max)
    void setTrackingTarget(const Position& point, const Vec3& offset, const Vec2& limits);

    /// Gets the tracked point
    /// @returns The tracked point
    Position getTrackedPoint() const;

    /// Sets the tracking offset
    /// @param offset Offset of the camera from the target
    void setOffset(const Vec3& offset);

    /// Gets the tracking offset
    /// @returns The tracking offset
    Vec3 getOffset() const;

    /// Sets the tracking orientation offset
    /// @param offset Offset of the cameras orientation
    void setOrientationOffset(const Quat& offset);

    /// Gets the tracking orientation offset
    /// @returns The tracking orientation offset
    Quat getOrientationOffset() const;

    // Motion Particles
    // TODO: Move this elsewhere

    /// Toggles motion particles
    /// @param active True if motion particles should be displayed, false
    /// otherwise
    void toggleParticles(bool active);

    /// Update motion particles. Called internally by update()
    /// @param dt Delta-time
    void updateParticles(float dt);

    /// Get the state of motion particles
    /// @returns true if motion particles are enabled, false otherwise
    bool isParticlesEnabled() const;

    /// Event Delegate
    void handleEvent(EventDataPtr eventData);

    /// Custom control state handler
    virtual void customControlStateHandler(float dt) {}
    virtual bool customControlStateShouldInterpolate(int controlState) const { return false; }

    // Inherited from Camera
    virtual void setPosition(const Position& position) override;
    virtual void setOrientation(const Quat& orientation) override;
    virtual bool update(float dt) override;

protected:
    Input* mInputMgr;

    Position mTargetPosition;
    Quat mTargetOrientation;

    // State
    uint mControlState;
    uint mDragEventState;
    bool mIsDragging;

    // Free Control
    float mSpeed;
    Vec3 mFreeControlVelocity;

    // Tracking
    Position mTrackedPosition;
    // TODO: consider mTrackedOrientation?
    Vec3 mOffset;
    Quat mOrientationOffset;
    Vec2 mTrackAngleLimit;
    Vec2 mAngleLimitOrientation;

    // Track parameters
    bool mFPSStyle;    // true if tracking with angle limits (fps style)

    // Shake
    // TODO
    float mShakeFactor;
    float mShakeTimer;
    PerlinNoise mNoiseFunction;

    // Motion Particles
    bool mParticlesEnabled;
    Vec3 mParticlesSize;
    Ogre::SceneNode* mParticlesRoot;
    Ogre::BillboardSet* mParticleSet;
    Vector<Ogre::Billboard*> mParticles;
    Position mLastParticlePosition;

    // Internal direct control methods used by update
    void freeControl(float dt);
    void trackFixed(const Position& position, const Quat& orientation, const Vec3& offset);
    void trackFree(const Position& position, const Quat& orientation, const Vec3& offset,
                   const Quat& orientationOffset);
    bool shouldInterpolate() const;
};

NAMESPACE_END

