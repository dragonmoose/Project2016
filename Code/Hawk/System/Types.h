#pragma once
#include <cstdint>

using uint8 = uint8_t;
using int8 = int8_t;
using uint16 = uint16_t;
using int16 = int16_t;
using uint32 = uint32_t;
using int32 = int32_t;
using uint64 = uint64_t;
using int64 = int64_t;
using uintmax = uintmax_t;

namespace Hawk {

using ModuleID		= uint32;
using ThreadID		= uint32;
using SceneNodeID	= uint32;
using EntityID		= uint32;

__declspec(selectany) ThreadID ThreadID_Invalid = ThreadID();
__declspec(selectany) ThreadID ModuleID_Invalid = ModuleID();
}