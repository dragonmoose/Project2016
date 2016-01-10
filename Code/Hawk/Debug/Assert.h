#pragma once
#include "Console/Logger.h"
#include <intrin.h>

#ifdef HAWK_DEBUG
	#define ASSERT(p, msg)							\
		if ((p))									\
		{											\
		}											\
		else										\
		{	LOG(msg, "assert", Fatal);				\
			__debugbreak();							\
		}
#else
	#define ASSERT(p, msg)
#endif

#define ASSERT_LN(msg) ASSERT(false, msg)