#include "pch.h"
#include "Mutex.h"
#ifdef HAWK_DEBUG
#include "Debug/Profiler.h"
#endif

namespace Hawk {

#ifdef HAWK_DEBUG
std::atomic_uint Mutex::m_uiCounter(0);
#endif

Mutex::Mutex(const std::string& p_DebugName)
{
#ifdef HAWK_DEBUG
	m_DebugName += "Mutex_" + std::to_string(m_uiCounter++) + (!p_DebugName.empty() ? " [" + p_DebugName + "]" : "");
#endif
}

void Mutex::lock()
{
#ifdef HAWK_DEBUG
	if (!m_DebugName.empty())
	{
		LockAndProfile();
	}
	else
	{
		m_Mutex.lock();
	}
#else
	m_Mutex.lock();
#endif
}

void Mutex::unlock()
{
	m_Mutex.unlock();
}

#ifdef HAWK_DEBUG
void Mutex::LockAndProfile()
{
	Profiler l_Profiler(m_DebugName);
	l_Profiler.Start();
	m_Mutex.lock();
}
#endif
}