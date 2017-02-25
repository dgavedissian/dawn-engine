/*
* Dawn Engine
* Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
*/
#pragma once

#include "renderer/Material.h"

namespace dw {
class DW_API Renderable : public Object {
public:
    DW_OBJECT(Renderable);

	Renderable(Context* context);
    virtual ~Renderable();

	/// @brief Draws this Renderable to the viewport.
	virtual void Draw() = 0;

	/// @brief Changes the material used to render this Renderable object.
    /// @param material The material to assign to this Renderable.
    void SetMaterial(SharedPtr<Material> material);

	/// @brief Returns the material of this Renderable.
    /// @return The material currently assigned to this Renderable.
    Material* GetMaterial() const;

private:
    SharedPtr<Material> material_;
};
}
