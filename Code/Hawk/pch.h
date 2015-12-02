#pragma once

#include "Console/Logger.h"
#include "System/Config.h"
#include "System/Exception.h"
#include <string>
#include <sstream>

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
	#define NOMINMAX
#endif
#include <windows.h>