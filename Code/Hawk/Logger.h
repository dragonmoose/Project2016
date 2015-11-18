#pragma once

#ifdef HAWK_DEBUG
#include "DllExport.h"
#include <sstream>
#include <thread>

namespace Hawk {
namespace Logger
{
	enum class Level
	{
		Debug,
		Info,
		Warning,
		Error,
		Exception
	};

	bool Initialize();
	void RegisterThreadName(const std::string& p_Name, std::thread::id p_ID);

	HAWK_DLL_EXPORT void Write(const std::string& p_Msg, Level p_Level);
}
}

#define LOG_WITH_SOURCE_INFO(msg, level)															\
{																									\
	std::ostringstream l_Stream;																		\
	l_Stream << msg << " [" << __FILE__ << ":" << std::to_string(__LINE__) << "]\r\n";				\
	Hawk::Logger::Write(l_Stream.str(), Hawk::Logger::Level::level);							\
}
#define LOG_EXCEPTION_WITH_SOURCE_INFO(e)															\
{																									\
	std::ostringstream l_Stream;																		\
	l_Stream << e.what() << " [" << e.GetSourceInfo() << " -> " << __FILE__ << ":";					\
	l_Stream << std::to_string(__LINE__) << "]\r\n";												\
	Hawk::Logger::Write(l_Stream.str(), Hawk::Logger::Level::Exception);						\
}

#define LOG_DEBUG(msg)						LOG_WITH_SOURCE_INFO(msg, Debug);
#define LOG_INFO(msg)						LOG_WITH_SOURCE_INFO(msg, Info);
#define LOG_WARNING(msg)					LOG_WITH_SOURCE_INFO(msg, Warning);
#define LOG_ERROR(msg)						LOG_WITH_SOURCE_INFO(msg, Error);
#define LOG_EXCEPTION(e)					LOG_EXCEPTION_WITH_SOURCE_INFO(e);
#define LOG_STD_EXCEPTION(e)				LOG_WITH_SOURCE_INFO(e.what(), Exception);

#define LOG_INFO_IF(p, msg)					if ((p)) LOG_INFO(msg)
#define LOG_WARNING_IF(p, msg)				if ((p)) LOG_WARNING(msg)
#define LOG_ERROR_IF(p, msg)				if ((p)) LOG_ERROR(msg)

#else
#define LOG_DEBUG(msg)
#define LOG_INFO(msg)
#define LOG_WARNING(msg)
#define LOG_ERROR(msg)
#define LOG_EXCEPTION(e)

#define LOG_INFO_IF(p, msg)		
#define LOG_WARNING_IF(p, msg)	
#define LOG_ERROR_IF(p, msg)	
#endif