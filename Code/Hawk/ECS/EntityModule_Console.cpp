#include "pch.h"
#include "EntityModule.h"
#include "ComponentManager.h"
#include "Util/Random.h"

#ifdef HAWK_DEBUG
namespace Hawk {
namespace ECS {

void EntityModule::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("ecs.addInteractiveCamera", this, &EntityModule::CreateInteractiveCamera, "Adds an interactive camera", "");
	p_Console->Register("ecs.addFixedCamera", this, &EntityModule::CreateFixedCamera, "Adds a fixed camera", "");
	p_Console->Register("ecs.addSuicidalEnemy", this, &EntityModule::CreateSuicidalEnemy, "Adds an entity with a fixed lifespan", "[duration in seconds]");
	p_Console->Register("ecs.addRandom", this, &EntityModule::CmdAddRandom, "Adds random", "[count]");
	p_Console->Register("ecs.stats", this, &EntityModule::CmdStats, "Print stats", "[print entity states 0|1]", false);
	p_Console->Register("ecs.list", this, &EntityModule::CmdList, "List alive entities", "");
	p_Console->Register("ecs.print", this, &EntityModule::CmdPrint, "Prints the given entity", "[id]");
	p_Console->Register("ecs.listComponents", m_ComponentManager.get(), &ComponentManager::CmdList, "Lists all registered components", "");
}

void EntityModule::CmdStats(bool p_bStates)
{
	CONSOLE_WRITE_SCOPE();
	std::cout << "\nMax entities: " << m_Entities.size() << "\n";
	std::cout << "Next entity index: " << m_NextEntityIndex << "\n";

	if (p_bStates)
	{
		std::cout << "EntityState: " << m_Entities;
	}
	std::cout << "\n";
}

void EntityModule::CmdAddRandom(std::size_t m_Count)
{
	for (int i = 0; i < m_Count; i++)
	{
		Entity& l_Entity = m_Entities[CreateEntity()];
		l_Entity.m_Mask = sc_SuicidalEnemyMask;
		m_ComponentManager->Get<CtKillTime>(l_Entity.m_ID).m_Time =
			Time(Duration(Random::GetInt(0, 30), Duration::Precision::Second));
	}
}

void EntityModule::CmdList()
{
	CONSOLE_WRITE_SCOPE();
	for (uint32 i = 0; i < m_NextEntityIndex; i++)
	{
		Entity& l_Entity = m_Entities[i];
		std::cout << l_Entity.m_ID;
		if (!l_Entity.m_DebugName.empty())
		{
			std::cout << " [" << l_Entity.m_DebugName << "]";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

void EntityModule::CmdPrint(EntityID p_ID)
{
	CONSOLE_WRITE_SCOPE();
	THROW_IF_NOT(p_ID < m_Entities.size(), "Invalid id=" << p_ID << " MaxID=" << m_Entities.size());

	Entity& l_Entity = m_Entities[p_ID];
	std::cout << "\n" << l_Entity << "\n";
	std::cout << "Components: ";
	std::size_t l_iComp = 0;
	for (int i = 0; i < l_Entity.m_Mask.GetSize(); i++)
	{
		if (l_Entity.m_Mask.Get(i))
		{
			std::cout << m_ComponentManager->GetNameFromID(i);
			if (l_iComp < l_Entity.m_Mask.GetNumComponents() - 1)
			{
				std::cout << ", ";
			}
			l_iComp++;
			if (l_iComp == l_Entity.m_Mask.GetNumComponents()) break;
		}
	}
	std::cout << "\n";
}

}
}
#endif