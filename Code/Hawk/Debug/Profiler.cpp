#include "pch.h"
#include "Profiler.h"
#include "ProfilerManager.h"
#include "System/Duration.h"

namespace Hawk {

Profiler::Profiler(const std::string& p_Name)
: m_bRunning(false)
, m_Name(p_Name)
{
}

Profiler::~Profiler()
{
#ifdef HAWK_DEBUG
	if (m_bRunning)
	{
		Stop();
	}
#endif
}

void Profiler::Start()
{
#ifdef HAWK_DEBUG
	if (!m_bRunning)
	{
		m_StartTime.SetToNow();
		m_bRunning = true;
	}
	else
	{
		LOG("Attempting to start an already running profiler. Name: " << m_Name, "profiler", Warning);
	}
#endif
}

void Profiler::Stop()
{
#ifdef HAWK_DEBUG
	if (m_bRunning)
	{
		ProfilerManager::Add(m_Name, Time::Now() - m_StartTime);
		m_bRunning = false;
	}
	else
	{
		LOG("Stopping profiler that was never started, or calling Stop() more than once. Name: " << m_Name, "profiler", Warning);
	}
#endif
}
}