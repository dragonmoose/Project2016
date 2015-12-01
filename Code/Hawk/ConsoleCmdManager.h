#pragma once
#include "ConsoleInputHandler.h"

namespace Hawk {

class ConsoleCmdManager final
{
public:
	ConsoleCmdManager();
	ConsoleCmdManager(const ConsoleCmdManager&) = delete;
	ConsoleCmdManager& operator=(const ConsoleCmdManager&) = delete;
	void Update();

	void Start();
	void Stop();

private:
	ConsoleInputHandler m_InputHandler;
};

}