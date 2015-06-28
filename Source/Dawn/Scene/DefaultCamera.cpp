/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2015 (avedissian.david@gmail.com)
 */
#include "Common.h"
#include "Graphics/RenderSystem.h"
#include "Input/InputManager.h"
#include "DefaultCamera.h"

#define PARTICLE_COUNT 500
#define PARTICLE_REGION_SIZE 600.0f

NAMESPACE_BEGIN

DefaultCamera::DefaultCamera(RenderSystem* renderSystem, InputManager* inputMgr, SceneManager* sceneMgr)
    : Camera(sceneMgr),
      mInputMgr(inputMgr),
      mTargetPosition(Vec3::zero),
      mTargetOrientation(Quat::identity),
      mControlState(CCS_MANUAL),
      mDragEventState(CDES_ALWAYS),
      mIsDragging(true),
      mSpeed(100.0f),
      mFreeControlVelocity(Vec3::zero),
      mTrackedPosition(),
      mOffset(Vec3::zero),
      mOrientationOffset(Quat::identity),
      mTrackAngleLimit(Vec2::zero),
      mAngleLimitOrientation(Vec2::zero),
      mFPSStyle(false),
      mShakeFactor(0.0f),
      mShakeTimer(0.0f),
      mNoiseFunction(3, 16.0f, 1.0f, 0),
      mParticlesEnabled(false),
      mParticlesSize(PARTICLE_REGION_SIZE),
      mParticlesRoot(nullptr),
      mParticleSet(nullptr),
      mLastParticlePosition()
{
    mParticlesRoot = renderSystem->GetRootSceneNode()->createChildSceneNode();
    mParticleSet = renderSystem->GetSceneMgr()->createBillboardSet(PARTICLE_COUNT);
    mParticleSet->setDefaultDimensions(0.5f, 80.0f);
    mParticleSet->setBounds(Ogre::AxisAlignedBox::BOX_INFINITE, 1e20f);
    mParticleSet->setUseAccurateFacing(true);
    mParticleSet->setBillboardType(Ogre::BBT_ORIENTED_COMMON);
    mParticleSet->setMaterialName("Effect/MotionParticle");
    mParticlesRoot->attachObject(mParticleSet);

    ADD_LISTENER(DefaultCamera, EvtData_MouseDown);
    ADD_LISTENER(DefaultCamera, EvtData_MouseUp);
    ADD_LISTENER(DefaultCamera, EvtData_MouseMove);
}

DefaultCamera::~DefaultCamera()
{
    ToggleParticles(false);

    REMOVE_LISTENER(DefaultCamera, EvtData_MouseDown);
    REMOVE_LISTENER(DefaultCamera, EvtData_MouseUp);
    REMOVE_LISTENER(DefaultCamera, EvtData_MouseMove);
}

void DefaultCamera::SetControlState(uint state)
{
    mControlState = state;
}

uint DefaultCamera::GetControlState() const
{
    return mControlState;
}

void DefaultCamera::SetDragEventState(uint state)
{
    mDragEventState = state;

    // Update mIsDragging
    switch (mDragEventState)
    {
        case CDES_ALWAYS:
            SetDraggingFlag(true);
            break;

        case CDES_LMB_DOWN:
            SetDraggingFlag(mInputMgr->IsMouseButtonDown(SDL_BUTTON_LEFT));
            break;

        case CDES_LMB_UP:
            SetDraggingFlag(!mInputMgr->IsMouseButtonDown(SDL_BUTTON_LEFT));
            break;

        case CDES_RMB_DOWN:
            SetDraggingFlag(mInputMgr->IsMouseButtonDown(SDL_BUTTON_RIGHT));
            break;

        case CDES_RMB_UP:
            SetDraggingFlag(!mInputMgr->IsMouseButtonDown(SDL_BUTTON_RIGHT));
            break;

        default:
            break;
    }

    // Update cursor visibility
    mInputMgr->LockCursor(mIsDragging);
}

void DefaultCamera::SetDraggingFlag(bool flag)
{
    mIsDragging = flag;
    mInputMgr->LockCursor(flag);
}

void DefaultCamera::SetMovementSpeed(float speed)
{
    mSpeed = speed;
}

float DefaultCamera::GetMovementSpeed() const
{
    return mSpeed;
}

void DefaultCamera::SetTrackingTarget(const Position& point, const Vec3& offset)
{
    ToggleParticles(false);

    mTrackedPosition = point;
    mOffset = offset;
    mOrientationOffset = Quat::identity;

    mFPSStyle = false;
}

void DefaultCamera::SetTrackingTarget(const Position& point, const Vec3& offset,
                                             const Vec2& limits)
{
    ToggleParticles(false);

    mTrackedPosition = point;
    mOffset = offset;
    mOrientationOffset = Quat::identity;

    mFPSStyle = true;
    mTrackAngleLimit = limits;
    mAngleLimitOrientation = Vec2::zero;
}

Position DefaultCamera::GetTrackedPoint() const
{
    return mTrackedPosition;
}

void DefaultCamera::SetOffset(const Vec3& offset)
{
    mOffset = offset;
}

Vec3 DefaultCamera::GetOffset() const
{
    return mOffset;
}

void DefaultCamera::SetOrientationOffset(const Quat& orientation)
{
    mOrientationOffset = orientation;
}

Quat DefaultCamera::GetOrientationOffset() const
{
    return mOrientationOffset;
}

void DefaultCamera::ToggleParticles(bool active)
{
    if (active && !mParticlesEnabled)
    {
        static const float size = mParticlesSize.x;
        std::default_random_engine generator;
        std::uniform_real_distribution<float> pos(-size, size);

        // Create a number of motion particles
        for (uint i = 0; i < PARTICLE_COUNT; ++i)
        {
            Ogre::Vector3 position(pos(generator), pos(generator), pos(generator));
            mParticles.push_back(mParticleSet->createBillboard(position));
        }
    }

    if (!active && mParticlesEnabled)
    {
        // Clear particles
        for (auto i = mParticles.begin(); i != mParticles.end(); ++i)
            mParticleSet->removeBillboard(*i);
        mParticles.clear();
    }

    mParticlesEnabled = active;
}

void DefaultCamera::UpdateParticles(float dt)
{
    // Calculate velocity
    static const float maxVelocity = 2000.0f;
    Vec3 velocity = mPosition.GetRelativeToPoint(mLastParticlePosition);
    float length = velocity.Length();
    if (velocity.Dot(velocity) > (maxVelocity * maxVelocity))
    {
        velocity *= maxVelocity / length;
        length = maxVelocity;
    }

    // Update particles root
    mParticlesRoot->setPosition(mPosition.ToCameraSpace(this));

    // Update and wrap particles
    for (auto i = mParticles.begin(); i != mParticles.end(); ++i)
    {
        Ogre::Vector3& position = (*i)->mPosition;
        position -= velocity;
        position.x = Wrap(position.x, -mParticlesSize.x, mParticlesSize.x);
        position.y = Wrap(position.y, -mParticlesSize.y, mParticlesSize.y);
        position.z = Wrap(position.z, -mParticlesSize.z, mParticlesSize.z);

        // Set length
        (*i)->setDimensions(math::Min(length * 0.5f, mParticleSet->getDefaultWidth()),
                            math::Min(length * 2.0f, mParticleSet->getDefaultHeight()));

        // Fade out based on distance
        float alpha = 1.0f - math::Min(position.length() / PARTICLE_REGION_SIZE, 1.0f);
        (*i)->setColour(Ogre::ColourValue(alpha, alpha, alpha, alpha));
    }

    // Update direction
    mParticleSet->setCommonDirection(velocity / length);

    // Update last particle track position
    mLastParticlePosition = mPosition;
}

bool DefaultCamera::IsParticlesEnabled() const
{
    return mParticlesEnabled;
}

void DefaultCamera::SetPosition(const Position& position)
{
    mTargetPosition = position;
}

void DefaultCamera::SetOrientation(const Quat& orientation)
{
    mTargetOrientation = orientation;
}

bool DefaultCamera::Update(float dt)
{
    // Update the camera position based on the control state
    switch (mControlState)
    {
        case CCS_MANUAL:
            break;

        case CCS_FREE:
            FreeControl(dt);
            break;

        case CCS_TRACK_FIXED:
            TrackFixed(mTrackedPosition, Quat::identity, mOffset);
            break;

        case CCS_TRACK_FREE:
            TrackFree(mTrackedPosition, Quat::identity, mOffset, mOrientationOffset);
            break;

        default:
            CustomControlStateHandler(dt);
            break;
    }

    // Calculate shake
    Quat shake = Quat::identity;
    if (mShakeFactor > 0.0f)
    {
        float scale = mShakeFactor * 0.008f;
        Vec2 shakeFactor(mNoiseFunction.noise(mShakeTimer, 0.0f) * scale,
                         mNoiseFunction.noise(0.0f, mShakeTimer) * scale);
        shake = Quat::RotateX(shakeFactor.x) * Quat::RotateY(shakeFactor.y);
        mShakeTimer += 1.0f * dt;
    }

    // Should we interpolate position?
    if (CustomControlStateShouldInterpolate(mControlState) &&
        GetPosition().GetRelativeToPoint(mTargetPosition).Length() < 1000.0f)
    {
        float percentage = 0.95f;
        float scaledDt = dt * 4.0f;
        Camera::SetPosition(Lerp(GetPosition(), mTargetPosition, percentage, scaledDt * 4.0f));
        Camera::SetOrientation(Lerp(GetOrientation(), mTargetOrientation, percentage, scaledDt));
    }
    else
    {
        Camera::SetPosition(mTargetPosition);
        Camera::SetOrientation(mTargetOrientation * shake);
    }

    // Update particles
    if (mParticlesEnabled)
        UpdateParticles(dt);

    return true;
}

void DefaultCamera::HandleEvent(EventDataPtr eventData)
{
    static const float unitsToRadians = -0.002f;

    if (EventIs<EvtData_MouseDown>(eventData))
    {
        auto castedEventData = static_pointer_cast<EvtData_MouseDown>(eventData);
        if (castedEventData->button == SDL_BUTTON_LEFT)
        {
            if (mDragEventState == CDES_LMB_DOWN)
            {
                SetDraggingFlag(true);
            }

            if (mDragEventState == CDES_LMB_UP)
            {
                SetDraggingFlag(false);
            }
        }

        if (castedEventData->button == SDL_BUTTON_RIGHT)
        {
            if (mDragEventState == CDES_RMB_DOWN)
            {
                SetDraggingFlag(true);
            }

            if (mDragEventState == CDES_RMB_UP)
            {
                SetDraggingFlag(false);
            }
        }
    }

    if (EventIs<EvtData_MouseUp>(eventData))
    {
        auto castedEventData = static_pointer_cast<EvtData_MouseUp>(eventData);
        if (castedEventData->button == SDL_BUTTON_LEFT)
        {
            if (mDragEventState == CDES_LMB_DOWN)
            {
                SetDraggingFlag(false);
            }
            if (mDragEventState == CDES_LMB_UP)
            {
                SetDraggingFlag(true);
            }
        }

        if (castedEventData->button == SDL_BUTTON_RIGHT)
        {
            if (mDragEventState == CDES_RMB_DOWN)
            {
                SetDraggingFlag(false);
            }
            if (mDragEventState == CDES_RMB_UP)
            {
                SetDraggingFlag(true);
            }
        }
    }

    if (EventIs<EvtData_MouseMove>(eventData))
    {
        auto castedEventData = static_pointer_cast<EvtData_MouseMove>(eventData);

        // Dragging
        if (mIsDragging)
        {
            // Here we can actually control mOrientationOffset if the camera is
            // either tracking a point or orbiting around a player
            if (mControlState == CCS_TRACK_FREE)
            {
                // Orbit orientation
                if (mFPSStyle)
                {
                    mAngleLimitOrientation.x += castedEventData->motion.y * unitsToRadians;
                    mAngleLimitOrientation.y += castedEventData->motion.x * unitsToRadians;
                    mAngleLimitOrientation.x = -math::Clamp(-mAngleLimitOrientation.x,
                                                            mTrackAngleLimit.x, mTrackAngleLimit.y);
                    mOrientationOffset = Quat::RotateY(mAngleLimitOrientation.y) *
                                         Quat::RotateX(mAngleLimitOrientation.x);
                }
                else
                {
                    mOrientationOffset = mOrientationOffset *
                                         Quat::RotateX(castedEventData->motion.y * unitsToRadians) *
                                         Quat::RotateY(castedEventData->motion.x * unitsToRadians);
                }
            }
            else if (mControlState == CCS_FREE)
            {
                // Free control orientation
                SetOrientation(GetOrientation() *
                               Quat::RotateX(castedEventData->motion.y * unitsToRadians) *
                               Quat::RotateY(castedEventData->motion.x * unitsToRadians));
            }
        }
    }
}

void DefaultCamera::FreeControl(float dt)
{
    // Control based on the input state
    float speed = mSpeed;
    if (mInputMgr->IsKeyDown(SDLK_LSHIFT))
        speed *= 10.0f;

    // Calculate acceleration
    float forwardAcceleration = speed * ((float)mInputMgr->IsKeyDown(SDLK_s) -
                                         (float)mInputMgr->IsKeyDown(SDLK_w));
    float rightAcceleration = speed * ((float)mInputMgr->IsKeyDown(SDLK_d) -
                                       (float)mInputMgr->IsKeyDown(SDLK_a));

    // Damp velocity and update position
    float damping = 1.0f - math::Clamp01(dt);
    mFreeControlVelocity *= damping * damping;
    mFreeControlVelocity +=
            GetOrientation() * Vec3(rightAcceleration, 0.0f, forwardAcceleration) * dt;
    SetPosition(GetPosition() + mFreeControlVelocity * dt);
}

void DefaultCamera::TrackFixed(const Position& position, const Quat& orientation,
                               const Vec3& offset)
{
    SetOrientation(orientation);
    SetPosition(position + GetOrientation() * offset);
}

void DefaultCamera::TrackFree(const Position& position, const Quat& orientation,
                                     const Vec3& offset, const Quat& orientationOffset)
{
    SetOrientation(orientationOffset);
    SetPosition(position + GetOrientation() * offset);
}

NAMESPACE_END
