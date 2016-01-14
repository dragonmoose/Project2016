#include "pch.h"
#ifdef HAWK_DEBUG
#include "ProfilerManager.h"
#include "Assert.h"
#include "Console/ConsoleAPI.h"
#include "Console/ConsoleCommandManager.h"
#include "System/Duration.h"
#include "Util/Algorithm.h"
#include <unordered_map>
#include <iomanip>
#include <mutex>

namespace Hawk {
namespace ProfilerManager
{
	struct Data
	{
		Data() : m_uiCount(0) {}
		Data(const Duration& p_FirstMeasure) : m_uiCount(1), m_PeakTime(p_FirstMeasure), m_TotalTime(p_FirstMeasure) {}

		unsigned int m_uiCount;
		Duration m_PeakTime;
		Duration m_TotalTime;
	};

	struct ViewData
	{
		ViewData() {}
		ViewData(const std::string& p_Name, Duration p_AvgTime, const Data& p_Data)
		: m_Name(p_Name)
		, m_AvgTime(p_AvgTime)
		, m_Data(p_Data)
		{}

		std::string m_Name;
		Duration m_AvgTime;
		Data m_Data;
	};
	using ViewDataVec_t = std::vector<ViewData>;

	using DataMap_t = std::unordered_map<std::string, Data>;
	DataMap_t n_DataMap;
	std::mutex n_Mutex;
	bool n_bPaused = false;

	void Pause();
	void Resume();
	void Clear();
	void Print(const std::string& p_SortMode, int p_iMax, const std::string& p_Filter);
	ViewDataVec_t GetViewDataVec(const std::string& p_SortMode, int p_iMax, const std::string& p_Filter);
}

void ProfilerManager::Initialize(ConsoleCommandManager* p_ConsoleManager, Dispatcher* p_Dispatcher)
{
	p_ConsoleManager->Register("profiler.print", &ProfilerManager::Print, p_Dispatcher, "Prints profiling data", "[peak|avg|total|count] [max] [filter]", false);
	p_ConsoleManager->Register("profiler.pause", &ProfilerManager::Pause, p_Dispatcher, "Pauses the collection of profiling data", "");
	p_ConsoleManager->Register("profiler.resume", &ProfilerManager::Resume, p_Dispatcher, "Resumes the collection of profiling data", "");
	p_ConsoleManager->Register("profiler.clear", &ProfilerManager::Clear, p_Dispatcher, "Clears all profiling data", "");
}

void ProfilerManager::Add(const std::string& p_Name, const Duration& p_Duration)
{
	if (!n_bPaused)
	{
		std::lock_guard<std::mutex> l_MutexScope(n_Mutex);
		auto l_Itr = n_DataMap.find(p_Name);
		if (l_Itr == n_DataMap.end())
		{
			std::pair<DataMap_t::iterator, bool> l_bResult = n_DataMap.insert(DataMap_t::value_type(p_Name, Data(p_Duration)));
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
	if (!n_bPaused)
	{
		std::cout << "\nProfiler paused.\n\n";
	}
	else
	{
		std::cout << "\nProfiler already paused.\n\n";
	}
	n_bPaused = true;
}

void ProfilerManager::Resume()
{
	CONSOLE_WRITE_SCOPE();
	if (n_bPaused)
	{
		std::cout << "\nProfiler resumed.\n\n";
	}
	else
	{
		std::cout << "\nProfiler already running.\n\n";
	}
	n_bPaused = false;
}

void ProfilerManager::Clear()
{
	std::lock_guard<std::mutex> l_MutexScope(n_Mutex);
	n_DataMap.clear();

	CONSOLE_WRITE_SCOPE();
	std::cout << "Cleared all profiling data.\n\n";
}

void ProfilerManager::Print(const std::string& p_SortMode, int p_iMax, const std::string& p_Filter)
{
	ViewDataVec_t l_DataVec = GetViewDataVec(p_SortMode, p_iMax, p_Filter);

	static const int w1 = 40;
	static const int w2 = 20;

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n" << std::left << std::setw(w1) << "Name" << std::setw(w2) << "Peak time (s)" << std::setw(w2) << "Avg time (s)" << std::setw(w2) << "Total time (s)" << "Count\n";
	std::cout << "---------------------------------------------------------------------------------------------------------\n";

	for (const auto& l_Data : l_DataVec)
	{
		std::string l_PeakTimeStr = l_Data.m_Data.m_PeakTime.ToString(Duration::Format::Seconds) + " (" + l_Data.m_Data.m_PeakTime.ToString(Duration::Format::LargestUnit) + ")";
		std::string l_TotalTimeStr = l_Data.m_Data.m_TotalTime.ToString(Duration::Format::Seconds) + " (" + l_Data.m_Data.m_TotalTime.ToString(Duration::Format::LargestUnit) + ")";
		
		std::string l_AvgTimeStr = l_Data.m_AvgTime.ToString(Duration::Format::Seconds) + " (" + l_Data.m_AvgTime.ToString(Duration::Format::LargestUnit) + ")";

		std::cout << std::left << std::setw(w1) << l_Data.m_Name << std::setw(w2) << l_PeakTimeStr << std::setw(w2) <<
			l_AvgTimeStr << std::setw(w2) << l_TotalTimeStr << l_Data.m_Data.m_uiCount << "\n";
	}
	std::cout << "\n";
}

ProfilerManager::ViewDataVec_t ProfilerManager::GetViewDataVec(const std::string& p_SortMode, int p_iMax, const std::string& p_Filter)
{
	std::lock_guard<std::mutex> l_MutexScope(n_Mutex);
	ViewDataVec_t l_DataVec;
	l_DataVec.reserve(n_DataMap.size());

	for (const auto& l_KeyVal : n_DataMap)
	{
		l_DataVec.push_back(ViewData(l_KeyVal.first, l_KeyVal.second.m_TotalTime / l_KeyVal.second.m_uiCount, l_KeyVal.second));
	}

	if (!p_SortMode.empty())
	{
		if (StringUtil::AreEqual(p_SortMode, "peak"))
		{
			std::sort(l_DataVec.begin(), l_DataVec.end(), [](const ViewData& p_lhs, const ViewData& p_rhs)
			{ return p_lhs.m_Data.m_PeakTime > p_rhs.m_Data.m_PeakTime; });
		}
		else if (StringUtil::AreEqual(p_SortMode, "avg"))
		{
			std::sort(l_DataVec.begin(), l_DataVec.end(), [](const ViewData& p_lhs, const ViewData& p_rhs)
			{ return p_lhs.m_AvgTime > p_rhs.m_AvgTime; });
		}
		else if (StringUtil::AreEqual(p_SortMode, "total"))
		{
			std::sort(l_DataVec.begin(), l_DataVec.end(), [](const ViewData& p_lhs, const ViewData& p_rhs)
			{ return p_lhs.m_Data.m_TotalTime > p_rhs.m_Data.m_TotalTime; });
		}
		else if (StringUtil::AreEqual(p_SortMode, "count"))
		{
			std::sort(l_DataVec.begin(), l_DataVec.end(), [](const ViewData& p_lhs, const ViewData& p_rhs)
			{ return p_lhs.m_Data.m_uiCount > p_rhs.m_Data.m_uiCount; });
		}
	}

	if (!p_Filter.empty())
	{
		hwk::erase_if(l_DataVec, [&p_Filter](const ViewData& p_Data) { return !StringUtil::Contains(p_Data.m_Name, p_Filter); });
	}

	if (p_iMax < l_DataVec.size())
	{
		l_DataVec.resize(p_iMax);
	}

	return l_DataVec;
}
}
#endif