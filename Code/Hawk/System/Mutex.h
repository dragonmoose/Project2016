#pragma once
#include "DllExport.h"
#include <mutex>
#ifdef HAWK_DEBUG
#include <atomic>
#endif

namespace Hawk {

class Mutex;
using MutexScope_t = std::lock_guard<Mutex>;

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
	std::mutex m_Mutex;

#ifdef HAWK_DEBUG
	void LockAndProfile();
	std::string m_DebugName;
	static std::atomic_uint m_uiCounter;
#endif

};
}