#pragma once
#ifdef HAWK_DEBUG
#include <string>

namespace Hawk {

class Duration;
class ConsoleCommandManager;
class Dispatcher;

namespace ProfilerManager
{
	void Initialize(ConsoleCommandManager* p_ConsoleManager, Dispatcher* p_Dispatcher);
	void Add(const std::string& p_Name, const Duration& p_Duration);
}

}
#endif