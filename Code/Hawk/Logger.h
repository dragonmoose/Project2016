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
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal,
	};

	bool Initialize();
	void RegisterThreadName(const std::string& p_Name, std::thread::id p_ID);
	HAWK_DLL_EXPORT void Write(const std::string& p_Msg, const std::string& p_Module, const std::string& p_FileInfo, Level p_Level);
	void WriteCmd(const std::string& p_Cmd);
}
}

#define LOG_WITH_SOURCE_INFO(msg, module, level)															\
{																											\
	std::ostringstream l_StreamMsg;																			\
	l_StreamMsg << msg;																						\
	std::ostringstream l_StreamFileInfo;																	\
	l_StreamFileInfo << "[" << __FILE__ << ":" << std::to_string(__LINE__) << "]\r\n";						\
	Hawk::Logger::Write(l_StreamMsg.str(), module, l_StreamFileInfo.str(), Hawk::Logger::Level::level);		\
	if (Hawk::Logger::Level::level == Hawk::Logger::Level::Fatal) Hawk::Logger::SetFatalFlag();				\
}

#define LOG_EXCEPTION_WITH_SOURCE_INFO(e, module, level)													\
{																											\
	std::ostringstream l_StreamMsg;																			\
	l_StreamMsg << "EXCEPTION: " << e.what();																\
	std::ostringstream l_StreamFileInfo;																	\
	l_StreamFileInfo << "[" << e.GetSourceInfo() << " -> " << __FILE__ << ":";								\
	l_StreamFileInfo << std::to_string(__LINE__) << "]\r\n";												\
	Hawk::Logger::Write(l_StreamMsg.str(), module, l_StreamFileInfo.str(), Hawk::Logger::Level::level);		\
	if (Hawk::Logger::Level::level == Hawk::Logger::Level::Fatal) Hawk::Logger::SetFatalFlag();				\
}

#define LOG(msg, module, level)						LOG_WITH_SOURCE_INFO(msg, module, level)
#define LOG_IF(p, msg, module, level)				if ((p)) LOG(msg, module, level)

#define LOGM(msg, level)							LOG_WITH_SOURCE_INFO(msg, this->GetName(), level)
#define LOGM_IF(p, msg, level)						if ((p)) LOGM(msg, level)

#define LOG_EXCEPTION(e, module, level)				LOG_EXCEPTION_WITH_SOURCE_INFO(e, module, level);
#define LOGM_EXCEPTION(e, level)					LOG_EXCEPTION_WITH_SOURCE_INFO(e, this->GetName(), level);

#define LOG_STD_EXCEPTION(e, module, level)			LOG_WITH_SOURCE_INFO(e.what(), module, level);
#define LOGM_STD_EXCEPTION(e, level)				LOG_WITH_SOURCE_INFO(e.what(), this->GetName(), level);

#define FATAL(msg, module)							LOG_WITH_SOURCE_INFO(msg, module, Fatal);
#define FATAL_IF(p, msg, module)					if ((p)) FATAL(msg, module)

#else
#define LOG(msg, module, level)
#define LOG_IF(p, msg, module, level)

#define LOGM(msg, level)	
#define LOGM_IF(p, msg, level)

#define LOG_EXCEPTION(e, module, level)		
#define LOGM_EXCEPTION(e, module, level)		

#define LOG_STD_EXCEPTION(e, module, level)
#define LOGM_STD_EXCEPTION(e, module, level)

#define FATAL(msg, module)							Hawk::Logger::SetFatalFlag();
#define FATAL_IF(p, msg, module)					if ((p)) FATAL(msg, module);
#endif