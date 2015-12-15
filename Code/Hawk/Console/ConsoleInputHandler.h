#ifdef HAWK_DEBUG
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
	using InputLines_t = std::vector<std::string>;

	ConsoleInputHandler();
	ConsoleInputHandler(const ConsoleInputHandler&) = delete;
	ConsoleInputHandler& operator=(const ConsoleInputHandler&) = delete;

	void Start();
	void Stop();

	bool TryPopQueued(InputLines_t& p_InputLines);

private:
	void RunInputLoop();

	InputLines_t m_InputLines;
	std::thread m_Thread;
	std::atomic_bool m_bStopSignal;
	mutable std::mutex m_Mutex;
};
}
#endif