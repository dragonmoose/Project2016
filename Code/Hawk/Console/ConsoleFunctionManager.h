#pragma once
#ifdef HAWK_DEBUG
#include "Console/ConsoleFunction.h"
#include "Console/ConsoleFunctionRouter.h"
#include "Console/ConsoleInputHandler.h"
#include <string>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <iostream>
#include <functional>
#include <mutex>
#include <unordered_map>

namespace Hawk {

class ConsoleFunctionManager final
{
public:
	ConsoleFunctionManager();
	ConsoleFunctionManager(const ConsoleFunctionManager&) = delete;
	ConsoleFunctionManager& operator=(const ConsoleFunctionManager&) = delete;

	void Start();
	void Stop();
	void Update();

private:
	void CmdCls();
	void CmdQuit();
	void CmdContinueAfterInterrupt();

	std::shared_ptr<ConsoleFunctionRouter> m_Router;
	ConsoleInputHandler m_InputHandler;
};

}
#endif