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
		Entity& l_Entity = CreateEntity(sc_SuicidalEnemyMask);
		m_ComponentManager->Get<KillTimeComponent>(l_Entity.GetID()).m_Time =
			Time(Duration(Random::GetInt(0, 30), Duration::Precision::Second));
	}
}

}
}
#endif