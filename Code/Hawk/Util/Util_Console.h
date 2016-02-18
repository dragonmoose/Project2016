#pragma once
#include "Threading/Dispatcher.h"
#include <memory>

namespace Hawk {

class ConsoleCommandManager;

namespace Util_Console
{
	void Register(ConsoleCommandManager* p_pManager, std::shared_ptr<Dispatcher> p_Dispatcher);
}

}