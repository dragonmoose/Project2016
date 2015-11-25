#pragma once

#include "DllExport.h"

namespace Hawk {
namespace Logger
{
	HAWK_DLL_EXPORT void SetFatalFlag();
	bool FatalFlagSet();
}
}

#ifdef HAWK_DEBUG
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
	HAWK_DLL_EXPORT void Write(const std::string& p_Msg, const std::string& p_System, const std::string& p_FileInfo, Level p_Level);
}
}

#define LOG_WITH_SOURCE_INFO(msg, system, level)															\
{																											\
	std::ostringstream l_StreamMsg;																			\
	l_StreamMsg << msg;																						\
	std::ostringstream l_StreamFileInfo;																	\
	l_StreamFileInfo << "[" << __FILE__ << ":" << std::to_string(__LINE__) << "]\r\n";						\
	Hawk::Logger::Write(l_StreamMsg.str(), system, l_StreamFileInfo.str(), Hawk::Logger::Level::level);		\
	if (Hawk::Logger::Level::level == Hawk::Logger::Level::Fatal) Hawk::Logger::SetFatalFlag();				\
}

#define LOG_EXCEPTION_WITH_SOURCE_INFO(e, system, level)													\
{																											\
	std::ostringstream l_StreamMsg;																			\
	l_StreamMsg << "EXCEPTION: " << e.what();																\
	std::ostringstream l_StreamFileInfo;																	\
	l_StreamFileInfo << "[" << e.GetSourceInfo() << " -> " << __FILE__ << ":";								\
	l_StreamFileInfo << std::to_string(__LINE__) << "]\r\n";												\
	Hawk::Logger::Write(l_StreamMsg.str(), system, l_StreamFileInfo.str(), Hawk::Logger::Level::level);		\
	if (Hawk::Logger::Level::level == Hawk::Logger::Level::Fatal) Hawk::Logger::SetFatalFlag();				\
}

#define LOG(msg, system, level)						LOG_WITH_SOURCE_INFO(msg, system, level)
#define LOG_IF(p, msg, system, level)				if ((p)) LOG(msg, system, level)

#define LOG_SYS(msg, level)							LOG_WITH_SOURCE_INFO(msg, this->GetName(), level)
#define LOG_SYS_IF(p, msg, level)					if ((p)) LOG_SYS(msg, level)

#define LOG_EXCEPTION(e, system, level)				LOG_EXCEPTION_WITH_SOURCE_INFO(e, system, level);
#define LOG_SYS_EXCEPTION(e, level)					LOG_EXCEPTION_WITH_SOURCE_INFO(e, this->GetName(), level);

#define LOG_STD_EXCEPTION(e, system, level)			LOG_WITH_SOURCE_INFO(e.what(), system, level);
#define LOG_SYS_STD_EXCEPTION(e, level)				LOG_WITH_SOURCE_INFO(e.what(), this->GetName(), level);

#define FATAL(msg, system)							LOG_WITH_SOURCE_INFO(msg, system, Fatal);
#define FATAL_IF(p, msg, system)					if ((p)) FATAL(msg, system)

#else
#define LOG(msg, system, level)
#define LOG_IF(p, msg, system, level)

#define LOG_SYS(msg, level)	
#define LOG_SYS_IF(p, msg, level)

#define LOG_EXCEPTION(e, system, level)		
#define LOG_SYS_EXCEPTION(e, system, level)		

#define LOG_STD_EXCEPTION(e, system, level)
#define LOG_SYS_STD_EXCEPTION(e, system, level)

#define FATAL(msg, system)							Hawk::Logger::SetFatalFlag();
#define FATAL_IF(p, msg, system)					if ((p)) FATAL(msg, system);
#endif