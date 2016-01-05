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

	void Print();
}

void ProfilerManager::Initialize(ConsoleCommandManager* p_ConsoleManager, Dispatcher* p_Dispatcher)
{
	p_ConsoleManager->Register("profiler.print", &ProfilerManager::Print, p_Dispatcher, "Prints profiling data", "[filter]", false);
}

void ProfilerManager::Add(const std::string& p_Name, const Duration& p_Duration)
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

void ProfilerManager::Print()
{
	static const int w1 = 40;
	static const int w2 = 20;

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n" << std::left << std::setw(w1) << "Name" << std::setw(w2) << "Peak time" << std::setw(w2) << "Total time" << "Count\n";
	std::cout << "-------------------------------------------------------------------------------------\n";

	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	for (const auto& l_KeyVal : m_DataMap)
	{
		const std::string& l_Name = l_KeyVal.first;
		const Data& l_Data = l_KeyVal.second;
		std::cout << std::left << std::setw(w1) << l_Name << std::setw(w2) << l_Data.m_PeakTime.ToString_Perf() << std::setw(w2) <<
			l_Data.m_TotalTime.ToString_Perf() << l_Data.m_uiCount << "\n";
	}
	std::cout << "\n";
}
}
#endif