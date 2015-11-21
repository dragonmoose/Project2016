#pragma once
#include <memory>
#include <thread>
#include <vector>

namespace Hawk {

class SystemBase;
class EventRouter;

class SystemManager final
{
public:
	SystemManager(const std::string& p_ThreadName);
	void AddSystem(std::unique_ptr<SystemBase> p_System);
	void Initialize(std::shared_ptr<EventRouter> p_EventRouter);
	void Start();

	SystemManager(const SystemManager&) = delete;
	SystemManager& operator=(const SystemManager&) = delete;

private:
	void Run();
	void Update(const Duration& p_Duration);

	typedef std::vector<std::unique_ptr<SystemBase>> Systems_t;
	Systems_t m_Systems;
	std::thread m_Thread;
	const std::string m_ThreadName;
};
}