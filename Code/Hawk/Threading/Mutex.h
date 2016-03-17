#pragma once
#include "System/DllExport.h"
#include <mutex>
#ifdef HAWK_DEBUG
#include <atomic>
#endif

//#define HAWK_MUTEX_IMPL_SPINLOCK 1

namespace Hawk {

class Mutex;
using MutexScope = std::lock_guard<Mutex>;

class HAWK_DLL_EXPORT Mutex final
{
public:
	Mutex() = default;
	Mutex(const std::string& m_DebugName);
	Mutex(const Mutex&) = delete;
	Mutex& operator=(const Mutex&) = delete;

	void lock();
	void unlock();

private:
	void _Lock();
	void _Unlock();

#ifdef HAWK_MUTEX_IMPL_SPINLOCK
	std::atomic_flag m_Flag = ATOMIC_FLAG_INIT;
#else
	std::mutex m_Mutex;
#endif

#ifdef HAWK_DEBUG
	void LockAndProfile();
	std::string m_DebugName;
	static std::atomic_uint m_uiCounter;
#endif

};
}