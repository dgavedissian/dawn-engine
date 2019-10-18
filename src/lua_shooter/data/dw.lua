--- Dawn Engine runtime

dw = {}

-- __dw_world contains the object that contains ECS functions. Set by the scripting system.
dw.world = __dw_world

-- Vector functions.
function dw.vec2(x, y)
    return {x = x, y = y}
end

function dw.vec3(x, y, z)
    return {x = x, y = y, z = z}
end

-- Returns an instance to a dw::Transform component.
function dw.Transform(v)
    return {v = v}
end