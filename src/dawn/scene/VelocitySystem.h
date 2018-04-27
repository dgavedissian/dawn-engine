/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "Velocity.h"
#include "System.h"

namespace dw {
class VelocitySystem : public System {
  DW_OBJECT(VelocitySystem);

  explicit VelocitySystem(Context* ctx);
  void processEntity(Entity& e, float dt) override;
};
}  // namespace dw
