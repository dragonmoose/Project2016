#pragma once
#include <memory>
#include <vector>

namespace Hawk
{
	class SystemBase;

	class SystemManager
	{
	public:
		SystemManager();

		void AddSystem(std::unique_ptr<SystemBase> p_System);
		void Prepare();
		void Update(const Duration& p_Duration);

		SystemManager(const SystemManager&) = delete;
		SystemManager& operator=(const SystemManager&) = delete;

	private:
		typedef std::vector<std::unique_ptr<SystemBase>> Systems_t;
		Systems_t m_Systems;
	};
}