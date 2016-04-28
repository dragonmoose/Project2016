#include "pch.h"
#include "EntityModule.h"
#include "Input/InputEvents.h"
#include <utility>

namespace Hawk {
namespace ECS {

using CF = ComponentFlags;
const ComponentMask EntityModule::sc_FixedCameraMask(CF::Position | CF::Camera);
const ComponentMask EntityModule::sc_InteractiveCameraMask(CF::Position | CF::Camera | CF::KeyInput);
const ComponentMask EntityModule::sc_SuicidalEnemyMask(CF::Position | CF::KillTime);

EntityModule::EntityModule()
: m_NextEntityIndex(0)
{
	const std::size_t l_MaxEntities = Config::Instance().Get("ecs.maxEntities", 1000);

	m_Entities.resize(l_MaxEntities);
	for (int i = 0; i < m_Entities.size(); i++)
	{
		m_Entities[i].SetID(i);
	}

	m_Positions.resize(l_MaxEntities);
	m_Cameras.resize(l_MaxEntities);
	m_KillTimes.resize(l_MaxEntities);

	LOGM("EntityModule created. MaxEntities=" << l_MaxEntities, Info);
}

std::string EntityModule::GetName() const
{
	return "EntityModule";
}

void EntityModule::Initialize()
{
}

void EntityModule::RegisterEvents(EventManager& p_EventManager)
{
	p_EventManager.Register<InputEvents::KeyDownEvent>([this](const InputEvents::KeyDownEvent& p_Event)
	{
		m_Keys.set(static_cast<std::size_t>(p_Event.m_Code));
	});

	p_EventManager.Register<InputEvents::KeyUpEvent>([this](const InputEvents::KeyUpEvent& p_Event)
	{
		m_Keys.reset(static_cast<std::size_t>(p_Event.m_Code));
	});
}

void EntityModule::Update(const Duration& /*p_Duration*/)
{
	for (int i = 0; i < m_NextEntityIndex; i++)
	{
		Entity& l_Entity = m_Entities[i];
		if (l_Entity.HasMask(sc_InteractiveCameraMask))
		{
			if (IsKeyDown(KeyCode::W))
			{
				LOGM("Camera moving forward...", Debug);
			}
		}
		else if (l_Entity.HasMask(sc_SuicidalEnemyMask))
		{
			if (Time::Now() >= m_KillTimes[l_Entity.GetID()].m_Time)
			{
				l_Entity.MarkPendingKill();
			}
		}
	}

	bool l_bEntitiesKilled = false;
	for (int i = 0; i < m_NextEntityIndex; i++)
	{
		Entity& l_Entity = m_Entities[i];
		if (l_Entity.IsPendingKill())
		{
			l_Entity.Kill();
			l_bEntitiesKilled = true;
		}
	}
	if (l_bEntitiesKilled)
	{
		SortEntities();
	}
}

bool EntityModule::IsKeyDown(KeyCode p_Key) const
{
	return m_Keys.test(static_cast<std::size_t>(p_Key));
}

Entity& EntityModule::CreateEntity(const ComponentMask& p_Mask)
{
	THROW_IF_NOT(m_NextEntityIndex < m_Entities.size(), "Failed to create entity, max size exceeded. Max=" << m_Entities.size());

	Entity& l_Entity = m_Entities[m_NextEntityIndex];
	ASSERT(!l_Entity.IsAlive(), "Entity was alive");

	l_Entity.Revive(p_Mask);
	m_NextEntityIndex++;
	LOG_IF(m_NextEntityIndex == m_Entities.size(), "Entity count limit reached=" << m_Entities.size(), "ecs", Warning);

	return l_Entity;
}

void EntityModule::SortEntities()
{
	PROFILE();

	auto l_Itr1 = m_Entities.begin();
	auto l_Itr2 = m_Entities.begin() + std::min(m_NextEntityIndex, m_Entities.size() - 1);

	m_NextEntityIndex = 0;
	while (l_Itr1 < l_Itr2)
	{
		if (!l_Itr1->IsAlive() && l_Itr2->IsAlive())
		{
			std::iter_swap(l_Itr1, l_Itr2);
		}
		if (l_Itr1->IsAlive())
		{
			++l_Itr1;
			++m_NextEntityIndex;
		}
		if (!l_Itr2->IsAlive())
		{
			--l_Itr2;
		}
	}
	if (l_Itr1->IsAlive())
	{
		++m_NextEntityIndex;
	}
}

void EntityModule::CreateInteractiveCamera()
{
	CreateEntity(sc_InteractiveCameraMask);
}

void EntityModule::CreateFixedCamera()
{
	CreateEntity(sc_FixedCameraMask);
}

void EntityModule::CreateSuicidalEnemy(uint32 p_DurationSec)
{
	Entity& l_Entity = CreateEntity(sc_SuicidalEnemyMask);
	m_KillTimes[l_Entity.GetID()].m_Time = Time(Duration(p_DurationSec, Duration::Precision::Second));
}

std::ostream& operator<<(std::ostream& os, const EntityModule::EntityList& p_Entities)
{
	for (const Entity& l_Entity : p_Entities)
	{
		if (l_Entity.IsAlive())
		{
			os << "A";
		}
		else if (!l_Entity.IsAlive())
		{
			os << "D";
		}
		else
		{
			os << "P";
		}
	}
	return os;
}


}
}