#pragma once
#include <thread>
#include <atomic>

namespace Hawk {

class ConsoleManager final
{
public:
	ConsoleManager();
	void Start();
	void Stop();
	ConsoleManager(const ConsoleManager&) = delete;
	ConsoleManager& operator=(const ConsoleManager&) = delete;

private:
	void Run_Thread();
	std::thread m_Thread;
	std::atomic_bool m_bStopSignal;
};

}