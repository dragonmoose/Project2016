#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>

namespace Hawk {

class ConsoleInputHandler final
{
public:
	using Commands_t = std::vector<std::string>;

	ConsoleInputHandler();
	ConsoleInputHandler(const ConsoleInputHandler&) = delete;
	ConsoleInputHandler& operator=(const ConsoleInputHandler&) = delete;

	void Start();
	void Stop();

	bool TryPopQueued(Commands_t& p_Commands);

private:
	void RunInputLoop();

	Commands_t m_Commands;
	std::thread m_Thread;
	std::atomic_bool m_bStopSignal;
	mutable std::mutex m_Mutex;
};
}
