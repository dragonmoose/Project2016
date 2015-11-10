#pragma once
#include <memory>

namespace Hawk
{
	class SystemBase;

	class SystemThread
	{
	public:
		SystemThread();
		void AddSystem(std::unique_ptr<SystemBase> p_System);

		SystemThread(const SystemThread&) = delete;
		SystemThread& operator=(const SystemThread&) = delete;
	};
}