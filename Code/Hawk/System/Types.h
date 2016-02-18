#pragma once
#include <cstdint>

namespace Hawk {

using INT32			= int32_t;
using UINT32		= uint32_t;
using INT64			= int64_t;
using UINT64		= uint64_t;

using ModuleID		= UINT32;
using ThreadID		= UINT32;
using EntityID_t	= UINT32;

__declspec(selectany) ThreadID ThreadID_Invalid = ThreadID();
__declspec(selectany) ThreadID ModuleID_Invalid = ModuleID();
}