#pragma once
#include "System/DllExport.h"
#include "Util/FileUtil.h"

namespace Hawk {
namespace Logger
{
	HAWK_DLL_EXPORT void SetFatalFlag();
	bool FatalFlagSet();
}
}

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
		Info,
		Warning,
		Error,
		Fatal,
	};

	void RegisterThread(const std::string& p_Name, std::thread::id p_SysThreadID, ThreadID p_ID = ThreadID_Invalid);
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
	if (Hawk::Logger::Level::level == Hawk::Logger::Level::Fatal) Hawk::Logger::SetFatalFlag();					\
}

#define LOG_EXCEPTION_WITH_SOURCE_INFO(e, tag, level)															\
{																												\
	std::ostringstream l_StreamMsg;																				\
	l_StreamMsg << "EXCEPTION: " << e.what();																	\
	std::ostringstream l_StreamFileInfo;																		\
	l_StreamFileInfo << e.GetSourceInfo() << " -> " << Hawk::FileUtil::GetFilenameWithoutPath(__FILE__);		\
	l_StreamFileInfo << ":" << std::to_string(__LINE__);														\
	Hawk::Logger::Log(l_StreamMsg.str(), tag, l_StreamFileInfo.str(), Hawk::Logger::Level::level);				\
	if (Hawk::Logger::Level::level == Hawk::Logger::Level::Fatal) Hawk::Logger::SetFatalFlag();					\
}

#define LOG(msg, tag, level)						LOG_WITH_SOURCE_INFO(msg, tag, level)
#define LOG_IF(p, msg, tag, level)					if ((p)) LOG(msg, tag, level)
#define LOG_EXCEPTION(e, tag, level)				LOG_EXCEPTION_WITH_SOURCE_INFO(e, tag, level);
#define LOG_STD_EXCEPTION(e, tag, level)			LOG_WITH_SOURCE_INFO(e.what(), tag, level);

#else
#define LOG(msg, tag, level)
#define LOG_IF(p, msg, tag, level)
#define LOG_EXCEPTION(e, tag, level)			
#define LOG_STD_EXCEPTION(e, tag, level)

#endif