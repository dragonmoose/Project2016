#pragma once
#include <cstdint>

namespace Hawk {

using ModuleID_t		= uint32_t;
using ThreadID_t		= uint32_t;
using EntityID_t		= uint32_t;

__declspec(selectany) ThreadID_t ThreadID_Invalid = ThreadID_t();
__declspec(selectany) ThreadID_t ModuleID_Invalid = ModuleID_t();
}