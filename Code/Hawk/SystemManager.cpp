#include "pch.h"
#include "SystemManager.h"
#include "SystemBase.h"

namespace Hawk {

	SystemManager::SystemManager()
{

}

void SystemManager::AddSystem(std::unique_ptr<SystemBase> p_System)
{
	m_Systems.push_back(std::move(p_System));
}

void SystemManager::Prepare()
{

}

void SystemManager::Update(const Duration& p_Duration)
{
	for (auto& l_System : m_Systems)
	{
		l_System->InternalUpdate(p_Duration);
	}
}


}