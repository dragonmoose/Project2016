#pragma once
#include "Console/Logger.h"
#include "Threading/ThreadInfoManager.h"
#include <intrin.h>

#ifdef HAWK_DEBUG
	#define ASSERT(p, msg)							\
		if ((p))									\
		{											\
		}											\
		else										\
		{	FATAL(msg, "assert");					\
			__debugbreak();							\
		}
	#define ASSERT_THREAD(p)		ASSERT(Hawk::ThreadInfoManager::IsThread(p), "Not on thread: " << p);
	#define ASSERT_LN(msg)			ASSERT(false, msg)
#else
	#define ASSERT(p, msg)
	#define ASSERT_THREAD(p)
	#define ASSERT_LN(msg)
#endif