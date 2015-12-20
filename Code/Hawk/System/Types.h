#pragma once

namespace Hawk {

using ModuleID = unsigned int;
using ThreadID = unsigned int;

__declspec(selectany) ThreadID ThreadID_Invalid = ThreadID();
__declspec(selectany) ThreadID ModuleID_Invalid = ModuleID();

}