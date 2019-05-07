-- Lua API idea
-- --------------------------------------

-- Some "game initialisation" script.

print "HELLO WORLD!"

dw = require('dw')

local luaVelocitySystem = dw.world:createEntitySystem(
    "VelocitySystem",
    {dw.Transform, "velocity"}
)
function luaVelocitySystem:process(view, dt)
    for e in view:entities() do
        local transform = e:component(dw.Transform)
        transform.position = transform.position + e.velocity * dt
    end
end

-- Some level initialisation script.

local entity = dw.world:createEntity({
    dw.Transform(dw.vec3(1.0, 2.0, 3.0)),
    velocity = dw.vec3(0.0, 0.0, 1.0)
})
