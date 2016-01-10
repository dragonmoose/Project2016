#include "pch.h"
#ifdef HAWK_DEBUG
#include "ProfilerManager.h"
#include "Assert.h"
#include "Console/ConsoleAPI.h"
#include "Console/ConsoleCommandManager.h"
#include "System/Duration.h"
#include <mutex>
#include <unordered_map>
#include <iomanip>

namespace Hawk {
namespace ProfilerManager
{
	struct Data
	{
		Data(const Duration& p_FirstMeasure) : m_uiCount(1), m_PeakTime(p_FirstMeasure), m_TotalTime(p_FirstMeasure) {}
		unsigned int m_uiCount;
		Duration m_PeakTime;
		Duration m_TotalTime;
	};

	using DataMap_t = std::unordered_map<std::string, Data>;
	DataMap_t m_DataMap;
	std::mutex m_Mutex;
	bool m_bPaused = false;

	void Pause();
	void Resume();
	void Clear();
	void Print();
}

void ProfilerManager::Initialize(ConsoleCommandManager* p_ConsoleManager, Dispatcher* p_Dispatcher)
{
	p_ConsoleManager->Register("profiler.print", &ProfilerManager::Print, p_Dispatcher, "Prints profiling data", "");
	p_ConsoleManager->Register("profiler.pause", &ProfilerManager::Pause, p_Dispatcher, "Pauses the collection of profiling data", "");
	p_ConsoleManager->Register("profiler.resume", &ProfilerManager::Resume, p_Dispatcher, "Resumes the collection of profiling data", "");
	p_ConsoleManager->Register("profiler.clear", &ProfilerManager::Clear, p_Dispatcher, "Clears all profiling data", "");
}

void ProfilerManager::Add(const std::string& p_Name, const Duration& p_Duration)
{
	if (!m_bPaused)
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		auto l_Itr = m_DataMap.find(p_Name);
		if (l_Itr == m_DataMap.end())
		{
			std::pair<DataMap_t::iterator, bool> l_bResult = m_DataMap.insert(DataMap_t::value_type(p_Name, Data(p_Duration)));
			ASSERT(l_bResult.second, "Failed to insert profiler data. Name=" << p_Name);
		}
		else
		{
			Data& l_Data = l_Itr->second;
			l_Data.m_uiCount++;
			l_Data.m_TotalTime += p_Duration;

			if (p_Duration > l_Data.m_PeakTime)
			{
				l_Data.m_PeakTime = p_Duration;
			}
		}
	}
}

void ProfilerManager::Pause()
{
	CONSOLE_WRITE_SCOPE();
	if (!m_bPaused)
	{
		std::cout << "\nProfiler paused.\n\n";
	}
	else
	{
		std::cout << "\nProfiler already paused.\n\n";
	}
	m_bPaused = true;
}

void ProfilerManager::Resume()
{
	CONSOLE_WRITE_SCOPE();
	if (m_bPaused)
	{
		std::cout << "\nProfiler resumed.\n\n";
	}
	else
	{
		std::cout << "\nProfiler already running.\n\n";
	}
	m_bPaused = false;
}

void ProfilerManager::Clear()
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	m_DataMap.clear();

	CONSOLE_WRITE_SCOPE();
	std::cout << "Cleared all profiling data.\n\n";

}

void ProfilerManager::Print()
{
	static const int w1 = 40;
	static const int w2 = 20;

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n" << std::left << std::setw(w1) << "Name" << std::setw(w2) << "Peak time (s)" << std::setw(w2) << "Total time (s)" << "Count\n";
	std::cout << "-------------------------------------------------------------------------------------\n";

	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	for (const auto& l_KeyVal : m_DataMap)
	{
		const std::string& l_Name = l_KeyVal.first;
		const Data& l_Data = l_KeyVal.second;
		std::cout << std::left << std::setw(w1) << l_Name << std::setw(w2) << l_Data.m_PeakTime.ToString(Duration::Format::Seconds) << std::setw(w2) <<
			l_Data.m_TotalTime.ToString(Duration::Format::Seconds) << l_Data.m_uiCount << "\n";
	}
	std::cout << "\n";
}
}
#endif