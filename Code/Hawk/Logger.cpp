#include "pch.h"
#include <atomic>

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
#include "StringUtil.h"
#include "Time.h"
#include "Duration.h"
#include <unordered_map>
#include <mutex>

namespace Hawk {
namespace Logger
{
	bool ShouldLog(const std::string& p_Msg, const std::string& p_ThreadInfo, const std::string& p_Module, Level p_Level);
	std::string GetThreadInfo();
	bool MatchesFilter(const std::string& p_Msg, const std::string& p_Filter);
	Level StringToLevel(const std::string& p_Level);
	void GetLevelColors(Level p_Level, ConsoleAPI::Color& p_Color, ConsoleAPI::Color& p_BgColor);

	using ThreadNames_t = std::unordered_map<std::thread::id, std::string>;
	ThreadNames_t n_ThreadNames;
	std::mutex m_ThreadInfoMutex;
}

void Logger::RegisterThreadName(const std::string& p_Name, std::thread::id p_ID)
{
	std::lock_guard<std::mutex> l_Lock(m_ThreadInfoMutex);
	THROW_IF_NOT(n_ThreadNames.insert(ThreadNames_t::value_type(p_ID, p_Name)).second, "Failed to insert thread name lookup. Name=" << p_Name << " ID=" << p_ID);
}

void Logger::Log(const std::string& p_Msg, const std::string& p_Module, const std::string& p_FileInfo, Level p_Level)
{
	std::string l_ThreadInfo = GetThreadInfo();
	if (!ShouldLog(p_Msg, l_ThreadInfo, p_Module, p_Level)) return;

	std::ostringstream l_Stream;
	l_Stream << Time::Now() << " " << l_ThreadInfo << " " << p_Module << " " << p_FileInfo << "] " << p_Msg;

	ConsoleAPI::Color l_Color;
	ConsoleAPI::Color l_BgColor;
	GetLevelColors(p_Level, l_Color, l_BgColor);
	ConsoleAPI::WriteLine(l_Stream.str(), l_Color, l_BgColor);
}

bool Logger::ShouldLog(const std::string& p_Msg, const std::string& p_ThreadInfo, const std::string& p_Module, Level p_Level)
{
	if (!Config::Instance().Get("Log.enabled", true)) return false;
	if (p_Level < StringToLevel(Config::Instance().Get<std::string>("Log.level", ""))) return false;
	if (!MatchesFilter(p_Msg, "Log.filter")) return false;
	if (!MatchesFilter(p_ThreadInfo, "Log.thread")) return false;
	if (!MatchesFilter(p_Module, "Log.module")) return false;
	return true;
}

std::string Logger::GetThreadInfo()
{
	std::thread::id l_ID = std::this_thread::get_id();
	std::ostringstream l_Stream;

	std::lock_guard<std::mutex> l_Lock(m_ThreadInfoMutex);
	auto l_Itr = n_ThreadNames.find(l_ID);
	if (l_Itr != n_ThreadNames.end())
	{
		l_Stream << l_Itr->second;
	}
	else
	{
		l_Stream << "N/A";
	}
	l_Stream << " #" << l_ID;
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