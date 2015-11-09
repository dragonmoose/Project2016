#pragma once

#include <sstream>

#ifndef STATIC_BUILD
	#ifdef HAWK_ENGINE
		#define HAWK_DLL_EXPORT __declspec(dllexport)
	#else
		#define HAWK_DLL_EXPORT __declspec(dllimport)
	#endif
#else
	#define HAWK_DLL_EXPORT
#endif

#include "LogSystem.h"
#ifdef HAWK_DEV
	#define LOG_WITH_SOURCE_INFO(msg, level)															\
	{																									\
		std::stringstream l_Stream;																		\
		l_Stream << msg << " [" << __FILE__ << ":" << std::to_string(__LINE__) << "]\r\n";				\
		Hawk::LogSystem::Write(l_Stream.str(), Hawk::LogSystem::Level::level);							\
	}
	#define LOG_EXCEPTION_WITH_SOURCE_INFO(e)															\
	{																									\
		std::stringstream l_Stream;																		\
		l_Stream << e.what() << " [" << e.GetSourceInfo() << " -> " << __FILE__ << ":";					\
		l_Stream << std::to_string(__LINE__) << "]\r\n";												\
		Hawk::LogSystem::Write(l_Stream.str(), Hawk::LogSystem::Level::Exception);						\
	}

	#define LOG_INFO(msg)						LOG_WITH_SOURCE_INFO(msg, Info);
	#define LOG_WARNING(msg)					LOG_WITH_SOURCE_INFO(msg, Warning);
	#define LOG_ERROR(msg)						LOG_WITH_SOURCE_INFO(msg, Error);
	#define LOG_EXCEPTION(e)					LOG_EXCEPTION_WITH_SOURCE_INFO(e);

	#define LOG_INFO_IF(p, msg)					if ((p)) LOG_INFO(msg)
	#define LOG_WARNING_IF(p, msg)				if ((p)) LOG_WARNING(msg)
	#define LOG_ERROR_IF(p, msg)				if ((p)) LOG_ERROR(msg)
#else
	#define LOG_INFO(msg)
	#define LOG_WARNING(msg)
	#define LOG_ERROR(msg)
	#define LOG_EXCEPTION(e)

	#define LOG_INFO_IF(p, msg)		
	#define LOG_WARNING_IF(p, msg)	
	#define LOG_ERROR_IF(p, msg)	
#endif

#include "Exception.h"
#define THROW(msg)								throw Hawk::Exception(msg, __FILE__, __LINE__);
#define THROW_IF(p, msg)						if ((p)) throw Hawk::Exception(msg, __FILE__, __LINE__);
#define THROW_IF_NOT(p, msg)					if (!(p)) throw Hawk::Exception(msg, __FILE__, __LINE__);