/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.dev)
 */
#pragma once

#include "Base.h"
#include "core/io/File.h"
#include "core/io/FileSystem.h"
#include "core/io/InputStream.h"
#include "core/io/OutputStream.h"
#include "core/io/Path.h"
#include "core/io/StringInputStream.h"
#include "core/math/Colour.h"
#include "core/math/Defs.h"
#include "core/math/MathGeoLib.h"
#include "core/math/Noise.h"
#include "core/math/Rect.h"
#include "core/math/StringHash.h"
#include "core/math/Vec2i.h"
#include "core/math/Vec3i.h"
#include "core/math/Vec4i.h"
#include "core/platform/Defines.h"
#include "core/App.h"
#include "core/BaseTypes.h"
#include "core/Collections.h"
#include "core/CommandLine.h"
#include "core/Concurrency.h"
#include "core/Context.h"
#include "core/Delegate.h"
#include "core/Engine.h"
#include "core/EventData.h"
#include "core/EventSystem.h"
#include "core/FixedMemoryPool.h"
#include "core/GameMode.h"
#include "core/GameSession.h"
#include "core/Handle.h"
#include "core/Log.h"
#include "core/Memory.h"
#include "core/Module.h"
#include "core/Object.h"
#include "core/Preprocessor.h"
#include "core/StringUtils.h"
#include "core/Timer.h"
#include "core/Type.h"
