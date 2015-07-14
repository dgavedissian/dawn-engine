/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
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

class DefaultCamera : public Camera
{
public:
    DefaultCamera(Renderer* renderSystem, Input* inputMgr, SceneManager* sceneMgr);
    virtual ~DefaultCamera();

    /// Control State

    /// Sets the control state to use
    /// @param state State identifier
    void SetControlState(uint state);

    /// Gets the current control state
    /// @returns The current control state
    uint GetControlState() const;

    /// Sets the drag event state, to control when dragging affects the
    /// orientation of the camera
    /// @param state State identifier
    void SetDragEventState(uint state);

    /// Internal: Updates the drag state flag
    void SetDraggingFlag(bool flag);

    /// Sets the movement speed of the camera in free mode
    /// @param speed Speed in metres per second
    void SetMovementSpeed(float speed);

    /// Gets the movement speed of the camera in free mode
    /// @returns The speed of the camera in metres per second
    float GetMovementSpeed() const;

    /// Sets the tracking target to a fixed point in track mode
    /// @param point Point to track
    /// @param ffset Offset of the camera from the point
    void SetTrackingTarget(const Position& point, const Vec3& offset);

    /// Sets the tracking target to a fixed point in track mode, with angle
    /// limits
    /// about the X axis. This also switches the rotation to FPS mode
    /// @param point Point to track
    /// @param offset Offset of the camera from the point
    /// @param limits The limits of the X angle (min, max)
    void SetTrackingTarget(const Position& point, const Vec3& offset, const Vec2& limits);

    /// Gets the tracked point
    /// @returns The tracked point
    Position GetTrackedPoint() const;

    /// Sets the tracking offset
    /// @param offset Offset of the camera from the target
    void SetOffset(const Vec3& offset);

    /// Gets the tracking offset
    /// @returns The tracking offset
    Vec3 GetOffset() const;

    /// Sets the tracking orientation offset
    /// @param offset Offset of the cameras orientation
    void SetOrientationOffset(const Quat& offset);

    /// Gets the tracking orientation offset
    /// @returns The tracking orientation offset
    Quat GetOrientationOffset() const;

    // Motion Particles
    // TODO: Move this elsewhere

    /// Toggles motion particles
    /// @param active True if motion particles should be displayed, false
    /// otherwise
    void ToggleParticles(bool active);

    /// Update motion particles. Called internally by update()
    /// @param dt Delta-time
    void UpdateParticles(float dt);

    /// Get the state of motion particles
    /// @returns true if motion particles are enabled, false otherwise
    bool IsParticlesEnabled() const;

    /// Event Delegate
    void HandleEvent(EventDataPtr eventData);

    /// Custom control state handler
    virtual void CustomControlStateHandler(float dt) {}
    virtual bool CustomControlStateShouldInterpolate(int controlState) const { return false; }

    // Inherited from Camera
    virtual void SetPosition(const Position& position) override;
    virtual void SetOrientation(const Quat& orientation) override;
    virtual bool Update(float dt) override;

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
    std::vector<Ogre::Billboard*> mParticles;
    Position mLastParticlePosition;

    // Internal direct control methods used by update
    void FreeControl(float dt);
    void TrackFixed(const Position& position, const Quat& orientation, const Vec3& offset);
    void TrackFree(const Position& position, const Quat& orientation, const Vec3& offset,
                   const Quat& orientationOffset);
    bool ShouldInterpolate() const;
};

NAMESPACE_END

