#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX

#include "Console/ConsoleAPI.h"
#include "Console/Logger.h"
#include "Debug/Profiler.h"
#include "Debug/Assert.h"
#include "System/Config.h"
#include "System/Exception.h"
#include "System/Types.h"
#include "System/CoreInfo.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>