/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2016 (git@davedissian.com)
 */
#include "Common.h"
#include "Entity.h"

NAMESPACE_BEGIN

Entity::Entity(SceneManager* sceneMgr)
    : mSceneMgr(sceneMgr),
      mPosition(Position::origin),
      mOrientation(Quat::identity)
{
}

Entity::Entity(SceneManager* sceneMgr, const Position& position)
    : mSceneMgr(sceneMgr),
      mPosition(position),
      mOrientation(Quat::identity)
{
}

Entity::Entity(SceneManager* sceneMgr, const Position& position, const Quat& orientation)
    : mSceneMgr(sceneMgr),
      mPosition(position),
      mOrientation(orientation)
{
}

Entity::~Entity()
{
}

void Entity::SetPosition(const Position& position)
{
    mPosition = position;
}

void Entity::SetOrientation(const Quat& orient)
{
    mOrientation = orient;
}

const Position& Entity::GetPosition() const
{
    return mPosition;
}

const Quat& Entity::GetOrientation() const
{
    return mOrientation;
}

NAMESPACE_END
