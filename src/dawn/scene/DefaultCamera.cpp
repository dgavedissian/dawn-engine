/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "renderer/Renderer.h"
#include "input/Input.h"
#include "DefaultCamera.h"

#define PARTICLE_COUNT 500
#define PARTICLE_REGION_SIZE 600.0f

NAMESPACE_BEGIN

DefaultCamera::DefaultCamera(Renderer* renderSystem, Input* inputMgr, SceneManager* sceneMgr)
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
    mParticlesRoot = renderSystem->getRootSceneNode()->createChildSceneNode();
    mParticleSet = renderSystem->getSceneMgr()->createBillboardSet(PARTICLE_COUNT);
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
    toggleParticles(false);

    REMOVE_LISTENER(DefaultCamera, EvtData_MouseDown);
    REMOVE_LISTENER(DefaultCamera, EvtData_MouseUp);
    REMOVE_LISTENER(DefaultCamera, EvtData_MouseMove);
}

void DefaultCamera::setControlState(uint state)
{
    mControlState = state;
}

uint DefaultCamera::getControlState() const
{
    return mControlState;
}

void DefaultCamera::setDragEventState(uint state)
{
    mDragEventState = state;

    // Update mIsDragging
    switch (mDragEventState)
    {
        case CDES_ALWAYS:
            setDraggingFlag(true);
            break;

        case CDES_LMB_DOWN:
            setDraggingFlag(mInputMgr->isMouseButtonDown(SDL_BUTTON_LEFT));
            break;

        case CDES_LMB_UP:
            setDraggingFlag(!mInputMgr->isMouseButtonDown(SDL_BUTTON_LEFT));
            break;

        case CDES_RMB_DOWN:
            setDraggingFlag(mInputMgr->isMouseButtonDown(SDL_BUTTON_RIGHT));
            break;

        case CDES_RMB_UP:
            setDraggingFlag(!mInputMgr->isMouseButtonDown(SDL_BUTTON_RIGHT));
            break;

        default:
            break;
    }

    // Update cursor visibility
    mInputMgr->lockCursor(mIsDragging);
}

void DefaultCamera::setDraggingFlag(bool flag)
{
    mIsDragging = flag;
    mInputMgr->lockCursor(flag);
}

void DefaultCamera::setMovementSpeed(float speed)
{
    mSpeed = speed;
}

float DefaultCamera::getMovementSpeed() const
{
    return mSpeed;
}

void DefaultCamera::setTrackingTarget(const Position& point, const Vec3& offset)
{
    toggleParticles(false);

    mTrackedPosition = point;
    mOffset = offset;
    mOrientationOffset = Quat::identity;

    mFPSStyle = false;
}

void DefaultCamera::setTrackingTarget(const Position& point, const Vec3& offset,
                                             const Vec2& limits)
{
    toggleParticles(false);

    mTrackedPosition = point;
    mOffset = offset;
    mOrientationOffset = Quat::identity;

    mFPSStyle = true;
    mTrackAngleLimit = limits;
    mAngleLimitOrientation = Vec2::zero;
}

Position DefaultCamera::getTrackedPoint() const
{
    return mTrackedPosition;
}

void DefaultCamera::setOffset(const Vec3& offset)
{
    mOffset = offset;
}

Vec3 DefaultCamera::getOffset() const
{
    return mOffset;
}

void DefaultCamera::setOrientationOffset(const Quat& orientation)
{
    mOrientationOffset = orientation;
}

Quat DefaultCamera::getOrientationOffset() const
{
    return mOrientationOffset;
}

void DefaultCamera::toggleParticles(bool active)
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

void DefaultCamera::updateParticles(float dt)
{
    // Calculate velocity
    static const float maxVelocity = 2000.0f;
    Vec3 velocity = mPosition.getRelativeTo(mLastParticlePosition);
    float length = velocity.Length();
    if (velocity.Dot(velocity) > (maxVelocity * maxVelocity))
    {
        velocity *= maxVelocity / length;
        length = maxVelocity;
    }

    // Update particles root
    mParticlesRoot->setPosition(mPosition.toCameraSpace(this));

    // Update and wrap particles
    for (auto i = mParticles.begin(); i != mParticles.end(); ++i)
    {
        Ogre::Vector3& position = (*i)->mPosition;
        position -= velocity;
        position.x = wrap<float>(position.x, -mParticlesSize.x, mParticlesSize.x);
		position.y = wrap<float>(position.y, -mParticlesSize.y, mParticlesSize.y);
		position.z = wrap<float>(position.z, -mParticlesSize.z, mParticlesSize.z);

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

bool DefaultCamera::isParticlesEnabled() const
{
    return mParticlesEnabled;
}

void DefaultCamera::setPosition(const Position& position)
{
    mTargetPosition = position;
    if (!shouldInterpolate())
        Camera::setPosition(mTargetPosition);
}

void DefaultCamera::setOrientation(const Quat& orientation)
{
    mTargetOrientation = orientation;
    if (!shouldInterpolate())
        Camera::setOrientation(mTargetOrientation);
}

bool DefaultCamera::update(float dt)
{
    // Update the camera position based on the control state
    switch (mControlState)
    {
        case CCS_MANUAL:
            break;

        case CCS_FREE:
            freeControl(dt);
            break;

        case CCS_TRACK_FIXED:
            trackFixed(mTrackedPosition, Quat::identity, mOffset);
            break;

        case CCS_TRACK_FREE:
            trackFree(mTrackedPosition, Quat::identity, mOffset, mOrientationOffset);
            break;

        default:
            customControlStateHandler(dt);
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

    // Interpolate position
    float percentage = 0.95f;
    float scaledDt = dt * 4.0f;
    Camera::setPosition(lerp(getPosition(), mTargetPosition, percentage, scaledDt * 4.0f));
    Camera::setOrientation(lerp(getOrientation(), mTargetOrientation, percentage, scaledDt));

    // Update particles
    if (mParticlesEnabled)
        updateParticles(dt);

    return true;
}

void DefaultCamera::handleEvent(EventDataPtr eventData)
{
    static const float unitsToRadians = -0.002f;

    if (eventIs<EvtData_MouseDown>(eventData))
    {
        auto castedEventData = castEvent<EvtData_MouseDown>(eventData);
        if (castedEventData->button == SDL_BUTTON_LEFT)
        {
            if (mDragEventState == CDES_LMB_DOWN)
            {
                setDraggingFlag(true);
            }

            if (mDragEventState == CDES_LMB_UP)
            {
                setDraggingFlag(false);
            }
        }

        if (castedEventData->button == SDL_BUTTON_RIGHT)
        {
            if (mDragEventState == CDES_RMB_DOWN)
            {
                setDraggingFlag(true);
            }

            if (mDragEventState == CDES_RMB_UP)
            {
                setDraggingFlag(false);
            }
        }
    }

    if (eventIs<EvtData_MouseUp>(eventData))
    {
        auto castedEventData = castEvent<EvtData_MouseUp>(eventData);
        if (castedEventData->button == SDL_BUTTON_LEFT)
        {
            if (mDragEventState == CDES_LMB_DOWN)
            {
                setDraggingFlag(false);
            }
            if (mDragEventState == CDES_LMB_UP)
            {
                setDraggingFlag(true);
            }
        }

        if (castedEventData->button == SDL_BUTTON_RIGHT)
        {
            if (mDragEventState == CDES_RMB_DOWN)
            {
                setDraggingFlag(false);
            }
            if (mDragEventState == CDES_RMB_UP)
            {
                setDraggingFlag(true);
            }
        }
    }

    if (eventIs<EvtData_MouseMove>(eventData))
    {
        auto castedEventData = castEvent<EvtData_MouseMove>(eventData);

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
                setOrientation(getOrientation() *
                               Quat::RotateX(castedEventData->motion.y * unitsToRadians) *
                               Quat::RotateY(castedEventData->motion.x * unitsToRadians));
            }
        }
    }
}

void DefaultCamera::freeControl(float dt)
{
    // Control based on the input state
    float speed = mSpeed;
    if (mInputMgr->isKeyDown(SDLK_LSHIFT))
        speed *= 10.0f;

    // Calculate acceleration
    float forwardAcceleration = speed * ((float)mInputMgr->isKeyDown(SDLK_s) -
                                         (float)mInputMgr->isKeyDown(SDLK_w));
    float rightAcceleration = speed * ((float)mInputMgr->isKeyDown(SDLK_d) -
                                       (float)mInputMgr->isKeyDown(SDLK_a));

    // Damp velocity and update position
    float damping = 1.0f - math::Clamp01(dt);
    mFreeControlVelocity *= damping * damping;
    mFreeControlVelocity +=
            getOrientation() * Vec3(rightAcceleration, 0.0f, forwardAcceleration) * dt;
    setPosition(getPosition() + mFreeControlVelocity * dt);
}

void DefaultCamera::trackFixed(const Position& position, const Quat& orientation,
                               const Vec3& offset)
{
    setOrientation(orientation);
    setPosition(position + getOrientation() * offset);
}

void DefaultCamera::trackFree(const Position& position, const Quat& orientation,
                                     const Vec3& offset, const Quat& orientationOffset)
{
    setOrientation(orientationOffset);
    setPosition(position + getOrientation() * offset);
}

bool DefaultCamera::shouldInterpolate() const
{
    return customControlStateShouldInterpolate(mControlState) &&
        getPosition().getRelativeTo(mTargetPosition).Length() < 1000.0f;
}

NAMESPACE_END
