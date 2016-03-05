#include "pch.h"
#include "Mutex.h"
#ifdef HAWK_DEBUG
#include "Debug/Profiler.h"
#endif

namespace Hawk {

#ifdef HAWK_DEBUG
std::atomic_uint Mutex::m_uiCounter(0);
#endif

#ifdef HAWK_DEBUG
Mutex::Mutex(const std::string& p_DebugName)
{
	m_DebugName += "Mutex_" + std::to_string(m_uiCounter++) + (!p_DebugName.empty() ? " [" + p_DebugName + "]" : "");
}
#else
Mutex::Mutex(const std::string&)
{
}
#endif

void Mutex::lock()
{
#ifdef HAWK_DEBUG
	if (!m_DebugName.empty())
	{
		LockAndProfile();
	}
	else
	{
		_Lock();
	}
#else
	_Lock();
#endif
}

void Mutex::unlock()
{
	_Unlock();
}

void Mutex::_Lock()
{
#ifdef HAWK_MUTEX_IMPL_SPINLOCK
	while (m_Flag.test_and_set(std::memory_order_acquire));
#else
	m_Mutex.lock();
#endif
}

void Mutex::_Unlock()
{
#ifdef HAWK_MUTEX_IMPL_SPINLOCK
	m_Flag.clear(std::memory_order_release);
#else
	m_Mutex.unlock();
#endif
}

#ifdef HAWK_DEBUG
void Mutex::LockAndProfile()
{
	Profiler l_Profiler(m_DebugName);
	l_Profiler.Start();
	_Lock();
}
#endif
}