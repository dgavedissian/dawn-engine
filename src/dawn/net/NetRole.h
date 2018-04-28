/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

namespace dw {
enum class NetRole {
    Proxy = 1,               // An object which receives replicated properties from the server.
    AuthoritativeProxy = 2,  // A proxy which can send client RPCs.
    Authority = 3,           // Authoritative copy (usually on the server).
    None = 4                 // No net role.
};
}  // namespace dw