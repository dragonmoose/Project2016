#pragma once
#include "System/Dispatcher.h"
#include <atomic>
#include <thread>
#include <functional>
#include <string>

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
	Dispatcher& GetDispatcher();

private:
	void Run();

	std::string m_Name;
	Dispatcher m_Dispatcher;
	std::thread m_Thread;
	UpdateFunc_t m_UpdateFunc;
	std::atomic_bool m_bStopSignal;
};

}