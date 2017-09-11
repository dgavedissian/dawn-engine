/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2017 (git@dga.me.uk)
 */
#pragma once

#include "core/Delegate.h"

namespace dw {
typedef u32 EventType;

class DW_API EventData {
public:
    virtual ~EventData() {
    }
    virtual const EventType& getType() const = 0;
    virtual String getName() const = 0;
};

using EventDataPtr = SharedPtr<EventData>;
using EventListenerDelegate = Delegate<void(EventDataPtr)>;
}