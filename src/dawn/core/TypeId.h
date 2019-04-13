/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#pragma once

namespace dw {
using TypeId = void(*)();

template <typename T>
TypeId typeId() noexcept
{
    return TypeId(typeId<T>);
}
}
