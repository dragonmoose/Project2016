#pragma once
#include <memory>

namespace Hawk
{
	class SystemBase;

	class SystemManagerThread
	{
	public:
		SystemManagerThread();
		void AddSystem(std::unique_ptr<SystemBase> p_System);

		SystemManagerThread(const SystemManagerThread&) = delete;
		SystemManagerThread& operator=(const SystemManagerThread&) = delete;
	};
}