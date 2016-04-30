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
#include "Threading/Mutex.h"
#include "Threading/ThreadInfoManager.h"
#include "Util/StringUtil.h"
#include <unordered_map>

namespace Hawk {
namespace Logger
{
	bool ShouldLog(const std::string& p_Msg, const std::string& p_ThreadInfo, const std::string& p_Tag, Level p_Level);
	bool MatchesFilter(const std::string& p_Msg, const std::string& p_Filter);
	Level StringToLevel(const std::string& p_Level);
	void GetLevelColors(Level p_Level, ConsoleAPI::Color& p_Color, ConsoleAPI::Color& p_BgColor);

	const std::array<std::string, 7> c_LogLevels = { "trace", "debug", "watch", "info", "warning", "error", "fatal" };
	bool n_bLogToOutputWindow = false;
}

void Logger::Log(const std::string& p_Msg, const std::string& p_Tag, const std::string& p_FileInfo, Level p_Level)
{
	if (n_bLogToOutputWindow)
	{
		std::ostringstream l_Stream;
		l_Stream << StringUtil::ToUpper(c_LogLevels[static_cast<std::size_t>(p_Level)]);
		l_Stream << ": " << p_Msg << " (" << p_Tag << ") [" << p_FileInfo << "]\n";
		::OutputDebugString(l_Stream.str().c_str());
	}
	if (!ConsoleAPI::Initialized()) return;

	// TODO: Lock is acquired in ThreadInfoManager::GetLogText even when logging is disabled - prevent this
	const std::string& l_ThreadInfo = ThreadInfoManager::GetLogText(std::this_thread::get_id());
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

	ConsoleAPI::SetTextColor(ConsoleAPI::Color::White, ConsoleAPI::Color::None);
}

bool Logger::IsValidLogLevelString(const std::string& p_Level)
{
	return std::find(c_LogLevels.cbegin(), c_LogLevels.cend(), StringUtil::ToLower(p_Level)) != c_LogLevels.cend();
}

void Logger::SetLogToOutputWindow(bool p_bValue)
{
	n_bLogToOutputWindow = p_bValue;
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
	else if (l_Level == "watch") { return Level::Watch; }
	else if (l_Level == "info") { return Level::Info; }
	else if (l_Level == "warning") { return Level::Warning; }
	else if (l_Level == "error") { return Level::Error; }
	else if (l_Level == "fatal") { return Level::_Fatal; }
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
	else if (p_Level == Level::Watch)
	{
		p_Color = ConsoleAPI::Color::BrightWhite;
		p_BgColor = ConsoleAPI::Color::BrightMagenta;
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
	else if (p_Level == Level::_Fatal)
	{
		p_Color = ConsoleAPI::Color::BrightWhite;
		p_BgColor = ConsoleAPI::Color::BrightRed;
	}
}

}
#endif