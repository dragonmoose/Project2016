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
	ASSERT(!m_bRunning, "Attempting to start an already running profiler. Name: " << m_Name);
	m_bRunning = true;
	m_StartTime.SetToNow();
#endif
}

void Profiler::Stop()
{
#ifdef HAWK_DEBUG
	Duration l_Duration = Time::Now() - m_StartTime;
	ASSERT(m_bRunning, "Attempting to stop profiler that was never started, or calling Stop() more than once. Name: " << m_Name);
	ProfilerManager::Add(m_Name, l_Duration);
	m_bRunning = false;
#endif
}
}