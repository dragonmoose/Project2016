#include "pch.h"
#include <atomic>
#include <array>

namespace Hawk {
namespace Logger
{
	std::atomic_bool m_bFatalFlag(false);
	void SetFatalFlag() { m_bFatalFlag = true; }
	bool FatalFlagSet() { return m_bFatalFlag; }
}
}

#ifdef HAWK_DEBUG
#include "Logger.h"
#include "ConsoleAPI.h"
#include "System/Time.h"
#include "System/Duration.h"
#include "Util/StringUtil.h"
#include <unordered_map>
#include <mutex>

namespace Hawk {
namespace Logger
{
	bool ShouldLog(const std::string& p_Msg, const std::string& p_ThreadInfo, const std::string& p_Tag, Level p_Level);
	std::string GetThreadInfo();
	bool MatchesFilter(const std::string& p_Msg, const std::string& p_Filter);
	Level StringToLevel(const std::string& p_Level);
	void GetLevelColors(Level p_Level, ConsoleAPI::Color& p_Color, ConsoleAPI::Color& p_BgColor);

	struct ThreadInfo
	{
		ThreadInfo(const std::string& p_Name, ThreadID p_ID) : m_Name(p_Name), m_ID(p_ID) {}
		std::string m_Name;
		ThreadID m_ID;
	};
	using ThreadInfoMap_t = std::unordered_map<std::thread::id, ThreadInfo>;
	ThreadInfoMap_t m_ThreadInfoMap;
	std::mutex m_ThreadInfoMutex;

	const std::array<std::string, 6> c_LogLevels = { "trace", "debug", "info", "warning", "error", "fatal" };
}

void Logger::RegisterThread(const std::string& p_Name, std::thread::id p_SysThreadID, ThreadID p_ID)
{
	std::lock_guard<std::mutex> l_Lock(m_ThreadInfoMutex);
	THROW_IF_NOT(m_ThreadInfoMap.insert(ThreadInfoMap_t::value_type(p_SysThreadID, ThreadInfo(p_Name, p_ID))).second, "Failed to insert thread name lookup. Name=" << p_Name << " ID=" << p_ID);
}

void Logger::Log(const std::string& p_Msg, const std::string& p_Tag, const std::string& p_FileInfo, Level p_Level)
{
	if (!ConsoleAPI::Initialized()) return;

	std::string l_ThreadInfo = GetThreadInfo();
	if (!ShouldLog(p_Msg, l_ThreadInfo, p_Tag, p_Level)) return;

	CONSOLE_WRITE_SCOPE();
	ConsoleAPI::Write(Time::Now().ToString().append(" "), ConsoleAPI::Color::White, ConsoleAPI::Color::None);

	std::ostringstream l_Stream;
	l_Stream << l_ThreadInfo << " " << p_Tag;
	ConsoleAPI::Write(l_Stream.str(), ConsoleAPI::Color::BrightWhite, ConsoleAPI::Color::Cyan);
	ConsoleAPI::Write(" ", ConsoleAPI::Color::None, ConsoleAPI::Color::None);

	l_Stream.str("");
	l_Stream.clear();

	l_Stream << p_Msg << " [" << p_FileInfo << "]";
	
	ConsoleAPI::Color l_Color;
	ConsoleAPI::Color l_BgColor;
	GetLevelColors(p_Level, l_Color, l_BgColor);
	ConsoleAPI::WriteLine(l_Stream.str(), l_Color, l_BgColor);
}

bool Logger::IsValidLogLevelString(const std::string& p_Level)
{
	return std::find(c_LogLevels.cbegin(), c_LogLevels.cend(), StringUtil::ToLower(p_Level)) != c_LogLevels.cend();
}

bool Logger::ShouldLog(const std::string& p_Msg, const std::string& p_ThreadInfo, const std::string& p_Tag, Level p_Level)
{
	if (!Config::Instance().Get("log.enabled", true)) return false;
	if (p_Level < StringToLevel(Config::Instance().Get<std::string>("log.level", ""))) return false;
	if (!MatchesFilter(p_Msg, "log.filter")) return false;
	if (!MatchesFilter(p_ThreadInfo, "log.thread")) return false;
	if (!MatchesFilter(p_Tag, "log.tag")) return false;
	return true;
}

std::string Logger::GetThreadInfo()
{
	std::thread::id l_SysThreadID = std::this_thread::get_id();
	std::ostringstream l_Stream;

	std::lock_guard<std::mutex> l_Lock(m_ThreadInfoMutex);
	auto l_Itr = m_ThreadInfoMap.find(l_SysThreadID);
	if (l_Itr != m_ThreadInfoMap.end())
	{
		l_Stream << l_Itr->second.m_Name;
		if (l_Itr->second.m_ID != ThreadID_Invalid)
		{
			l_Stream << " #" << l_Itr->second.m_ID;
		}
	}
	else
	{
		l_Stream << "N/A";
	}
	l_Stream << " [" << l_SysThreadID << "]";
	return l_Stream.str();
}

bool Logger::MatchesFilter(const std::string& p_Msg, const std::string& p_Filter)
{
	std::string l_Filter = Config::Instance().Get<std::string>(p_Filter, "");
	if (!l_Filter.empty())
	{
		using namespace StringUtil;
		return Contains(ToLower(p_Msg), ToLower(l_Filter));
	}
	return true;
}

Logger::Level Logger::StringToLevel(const std::string& p_Level)
{
	std::string l_Level = StringUtil::ToLower(p_Level);
	if (l_Level == "trace") { return Level::Trace; }
	else if (l_Level == "debug") { return Level::Debug; }
	else if (l_Level == "info") { return Level::Info; }
	else if (l_Level == "warning") { return Level::Warning; }
	else if (l_Level == "error") { return Level::Error; }
	else if (l_Level == "fatal") { return Level::Fatal; }
	else { return Level::Info; }
}

void Logger::GetLevelColors(Level p_Level, ConsoleAPI::Color& p_Color, ConsoleAPI::Color& p_BgColor)
{
	if (p_Level == Level::Trace)
	{
		p_Color = ConsoleAPI::Color::Cyan;
		p_BgColor = ConsoleAPI::Color::None;
	}
	else if (p_Level == Level::Debug)
	{
		p_Color = ConsoleAPI::Color::BrightGreen;
		p_BgColor = ConsoleAPI::Color::None;
	}
	else if (p_Level == Level::Info)
	{
		p_Color = ConsoleAPI::Color::BrightWhite;
		p_BgColor = ConsoleAPI::Color::None;
	}
	else if (p_Level == Level::Warning)
	{
		p_Color = ConsoleAPI::Color::BrightYellow;
		p_BgColor = ConsoleAPI::Color::None;
	}
	else if (p_Level == Level::Error)
	{
		p_Color = ConsoleAPI::Color::BrightRed;
		p_BgColor = ConsoleAPI::Color::None;
	}
	else if (p_Level == Level::Fatal)
	{
		p_Color = ConsoleAPI::Color::BrightWhite;
		p_BgColor = ConsoleAPI::Color::BrightRed;
	}
}

}
#endif