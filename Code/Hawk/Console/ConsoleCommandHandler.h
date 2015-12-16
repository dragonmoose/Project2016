#pragma once
#ifdef HAWK_DEBUG
#include "Console/ModuleConsoleRouter.h"
#include "Console/ConsoleInputHandler.h"
#include <string>
#include <functional>

namespace Hawk {

class ConsoleCommandHandler final
{
public:
	ConsoleCommandHandler();
	ConsoleCommandHandler(const ConsoleCommandHandler&) = delete;
	ConsoleCommandHandler& operator=(const ConsoleCommandHandler&) = delete;

	void Start();
	void Stop();
	void Update();
	std::shared_ptr<ModuleConsoleRouter> GetModuleRouter() const;

private:
	void CmdCls();
	void CmdQuit();
	void CmdContinueAfterInterrupt();

	std::shared_ptr<ModuleConsoleRouter> m_ModuleRouter;
	ConsoleInputHandler m_InputHandler;
};

}
#endif