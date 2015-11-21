#include "pch.h"
#include "SystemManager.h"
#include "SystemBase.h"
#include "Time.h"
#include "Duration.h"
#include "EventManager.h"

namespace Hawk {


SystemManager::SystemManager(const std::string& p_ThreadName)
: m_ThreadName(p_ThreadName)
{
}
void SystemManager::AddSystem(std::unique_ptr<SystemBase> p_System)
{
	m_Systems.push_back(std::move(p_System));
}

void SystemManager::Initialize(std::shared_ptr<EventRouter> p_EventRouter)
{
	for (auto& l_System : m_Systems)
	{
		l_System->InternalInitialize(std::make_unique<EventManager>(p_EventRouter));
	}
}

void SystemManager::Start()
{
	m_Thread = std::thread(&SystemManager::Run, this);
#ifdef HAWK_DEBUG
	Logger::RegisterThreadName(m_ThreadName, m_Thread.get_id());
#endif
	m_Thread.detach();
}

void SystemManager::Update(const Duration& p_Duration)
{
	for (auto& l_System : m_Systems)
	{
		l_System->InternalUpdate(p_Duration);
	}
}

void SystemManager::Run()
{
	LOG_INFO("Running " << m_Systems.size() << " systems on thread: " << m_ThreadName);

	Time l_OldTime;
	while (Config::Instance().Get<bool>("Core.run", true))
	{
		Time l_CurrTime;
		l_CurrTime.SetToNow();
		Duration l_Delta(l_CurrTime - l_OldTime);
		l_OldTime = l_CurrTime;

		Update(l_Delta);
	}
	LOG_INFO("*** Thread exit ***");
}

}