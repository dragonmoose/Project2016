#pragma once
#include "System/DllExport.h"
#include "System/CoreInfo.h"
#include "Util/FileUtil.h"

namespace Hawk {
namespace Logger
{
	HAWK_DLL_EXPORT void SetFatalFlag();
	HAWK_DLL_EXPORT bool FatalFlagSet();
}
}

#ifdef _WIN32
#define ON_FATAL(msg, tag)																							\
{																													\
	if (!Hawk::Logger::FatalFlagSet())																				\
	{																												\
		std::ostringstream l_StreamMsg;																				\
		l_StreamMsg << "Unrecoverable error, application will now quit.\nReason: " << msg << "\nModule: " << tag;	\
		::MessageBox(nullptr, l_StreamMsg.str().c_str(), Hawk::CoreInfo::GetAppName(), MB_OK | MB_ICONERROR);		\
		Hawk::Logger::SetFatalFlag();																				\
	}																												\
}
#else
#define ON_FATAL(p, msg)
#endif

#ifdef HAWK_DEBUG
#include "System/Types.h"
#include <sstream>
#include <thread>

namespace Hawk {
namespace Logger
{
	using WORD = unsigned short;
	enum class Level
	{
		Trace,
		Debug,
		Watch,
		Info,
		Warning,
		Error,
		_Fatal,
	};

	HAWK_DLL_EXPORT void Log(const std::string& p_Msg, const std::string& p_Tag, const std::string& p_FileInfo, Level p_Level);
	bool IsValidLogLevelString(const std::string& p_Level);
}
}

#define LOG_WITH_SOURCE_INFO(msg, tag, level)																	\
{																												\
	std::ostringstream l_StreamMsg;																				\
	l_StreamMsg << msg;																							\
	std::ostringstream l_StreamFileInfo;																		\
	l_StreamFileInfo << Hawk::FileUtil::GetFilenameWithoutPath(__FILE__) << ":" << std::to_string(__LINE__);	\
	Hawk::Logger::Log(l_StreamMsg.str(), tag, l_StreamFileInfo.str(), Hawk::Logger::Level::level);				\
}

#define LOG_EXCEPTION_WITH_SOURCE_INFO(e, tag, level)															\
{																												\
	std::ostringstream l_StreamMsg;																				\
	l_StreamMsg << "EXCEPTION: " << e.what();																	\
	std::ostringstream l_StreamFileInfo;																		\
	l_StreamFileInfo << e.GetSourceInfo() << " -> " << Hawk::FileUtil::GetFilenameWithoutPath(__FILE__);		\
	l_StreamFileInfo << ":" << std::to_string(__LINE__);														\
	Hawk::Logger::Log(l_StreamMsg.str(), tag, l_StreamFileInfo.str(), Hawk::Logger::Level::level);				\
}

#define LOG(msg, tag, level)						LOG_WITH_SOURCE_INFO(msg, tag, level)
#define LOGM(msg, level)							LOG(msg, GetLogDesc(), level)

#define FATAL(msg, tag)								LOG_WITH_SOURCE_INFO(msg, tag, _Fatal) ON_FATAL(msg, tag)
#define FATALM(msg)									FATAL(msg, GetLogDesc())

#define LOG_IF(p, msg, tag, level)					if ((p)) LOG(msg, tag, level)
#define LOGM_IF(p, msg, level)						if ((p)) LOG(msg, GetLogDesc(), level)

#define FATAL_IF(msg, tag)							if ((p)) FATAL(msg, tag)
#define FATALM_IF(msg)								FATALM(msg)

#define LOG_EXCEPTION(e, tag, level)				LOG_EXCEPTION_WITH_SOURCE_INFO(e, tag, level)
#define LOGM_EXCEPTION(e, level)					LOG_EXCEPTION_WITH_SOURCE_INFO(e, GetLogDesc(), level)

#define FATAL_EXCEPTION(e, tag)						LOG_EXCEPTION_WITH_SOURCE_INFO(e, tag, _Fatal) ON_FATAL(e.what(), tag)
#define FATALM_EXCEPTION(e)							FATAL_EXCEPTION(e, GetLogDesc())

#define LOG_EXCEPTION_CONSOLE(e)					std::cout << "Exception: " << e.what() << "\n\n";

#define LOG_STD_EXCEPTION(e, tag, level)			LOG_WITH_SOURCE_INFO(e.what(), tag, level)
#define LOGM_STD_EXCEPTION(e, level)				LOG_WITH_SOURCE_INFO(e.what(), GetLogDesc(), level)

#define FATAL_STD_EXCEPTION(e, tag)					LOG_WITH_SOURCE_INFO(e.what(), tag, _Fatal) ON_FATAL(e.what(), tag)
#define FATALM_STD_EXCEPTION(e)						FATAL_STD_EXCEPTION(e, GetLogDesc())

#define NOT_IMPLEMENTED()							LOG_WITH_SOURCE_INFO(__FUNCTION__ <<  " not implemented yet!", "system", Warning);

#else
#define LOG(msg, tag, level)
#define LOGM(msg, level)

#define FATAL(msg, tag) ON_FATAL(msg, tag)
#define FATALM(msg) FATAL(msg, GetLogDesc())

#define LOG_IF(p, msg, tag, level)
#define LOGM_IF(p, msg, level)

#define FATAL_IF(p, msg, tag) if ((p)) FATAL(msg, tag)
#define FATALM_IF(p, msg) if ((p)) FATAL(msg, GetLogDesc())

#define LOG_EXCEPTION(e, tag, level)			
#define LOGM_EXCEPTION(e, level)

#define FATAL_EXCEPTION(e, tag) FATAL(e.what(), tag)
#define FATALM_EXCEPTION(e) FATAL(e.what(), GetLogDesc())

#define LOG_EXCEPTION_CONSOLE(e)

#define LOG_STD_EXCEPTION(e, tag, level)
#define LOGM_STD_EXCEPTION(e, level)

#define FATAL_STD_EXCEPTION(e, tag) FATAL_EXCEPTION(e, tag)
#define FATALM_STD_EXCEPTION(e) FATALM_EXCEPTION(e)

#define NOT_IMPLEMENTED()

#endif