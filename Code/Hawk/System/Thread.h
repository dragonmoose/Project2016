#pragma once
#include "System/Dispatcher.h"
#include "System/Types.h"
#include <atomic>
#include <thread>
#include <functional>
#include <string>
#include <memory>

namespace Hawk {

class Thread final
{
public:
	using UpdateFunc_t = std::function<void(void)>;
	Thread(const std::string& p_Name, UpdateFunc_t p_UpdateFunc);

	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	void Start();
	void Stop();
	const std::string& GetName() const;
	ThreadID GetID() const;
	std::shared_ptr<Dispatcher> GetDispatcher() const;

	static std::string MainThreadName;

private:
	void Run();

	std::string m_Name;
	std::shared_ptr<Dispatcher> m_Dispatcher;
	std::thread m_Thread;
	UpdateFunc_t m_UpdateFunc;
	std::atomic_bool m_bStopSignal;

	static std::atomic_uint s_uiNextThreadID;
	ThreadID m_ID;
	
};

}