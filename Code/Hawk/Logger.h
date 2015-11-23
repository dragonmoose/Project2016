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
		Fatal,
	};

	bool Initialize();
	void RegisterThreadName(const std::string& p_Name, std::thread::id p_ID);

	HAWK_DLL_EXPORT void Write(const std::string& p_Msg, const std::string& p_System, Level p_Level);
}
}

#define LOG_WITH_SOURCE_INFO(msg, system, level)															\
{																											\
	std::ostringstream l_Stream;																			\
	l_Stream << msg << " [" << __FILE__ << ":" << std::to_string(__LINE__) << "]\r\n";						\
	Hawk::Logger::Write(l_Stream.str(), system, Hawk::Logger::Level::level);								\
}

#define LOG_EXCEPTION_WITH_SOURCE_INFO(e, system, level)													\
{																											\
	std::ostringstream l_Stream;																			\
	l_Stream << "EXCEPTION: " << e.what() << " [" << e.GetSourceInfo() << " -> " << __FILE__ << ":";		\
	l_Stream << std::to_string(__LINE__) << "]\r\n";														\
	Hawk::Logger::Write(l_Stream.str(), system, Hawk::Logger::Level::level);								\
}

#define LOG(msg, level)						LOG_WITH_SOURCE_INFO(msg, "system", level)
#define LOG_IF(p, msg, level)				if ((p)) LOG(msg, level)

#define LOG_SYS(msg, level)					LOG_WITH_SOURCE_INFO(msg, this->GetName(), level)
#define LOG_SYS_IF(p, msg, level)			if ((p)) LOG_SYS(msg, level)

#define LOG_EXCEPTION(e, level)				LOG_EXCEPTION_WITH_SOURCE_INFO(e, "system", level);
#define LOG_SYS_EXCEPTION(e, level)			LOG_EXCEPTION_WITH_SOURCE_INFO(e, this->GetName(), level);

#define LOG_STD_EXCEPTION(e, level)			LOG_WITH_SOURCE_INFO(e.what(), "system", level);
#define LOG_SYS_STD_EXCEPTION(e, level)		LOG_WITH_SOURCE_INFO(e.what(), this->GetName(), level);

#else
#define LOG(msg, level)		
#define LOG_IF(p, msg, level)

#define LOG_SYS(msg, level)	
#define LOG_SYS_IF(p, msg, level)

#define LOG_EXCEPTION(e, level)		
#define LOG_SYS_EXCEPTION(e, level)		

#define LOG_STD_EXCEPTION(e, level)
#define LOG_SYS_STD_EXCEPTION(e, level)
#endif