#include "pch.h"
#include "EntityModule.h"
#include "Input/InputEvents.h"
#include <utility>

namespace Hawk {
namespace ECS {

const ComponentMask EntityModule::sc_FixedCameraMask = CMASK(CtPosition::ID | CtCamera::ID);
const ComponentMask EntityModule::sc_InteractiveCameraMask = CMASK(CtPosition::ID | CtCamera::ID | CtKeyInput::ID);
const ComponentMask EntityModule::sc_SuicidalEnemyMask = CMASK(CtPosition::ID | CtKillTime::ID);

EntityModule::EntityModule()
: m_NextEntityIndex(0)
{
	const std::size_t l_MaxEntities = Config::Instance().Get("ecs.maxEntities", 1000);
	m_ComponentManager = std::make_unique<ComponentManager>(l_MaxEntities);
	m_ComponentManager->Register<CtPosition>();
	m_ComponentManager->Register<CtCamera>();
	m_ComponentManager->Register<CtKillTime>();
	m_ComponentManager->Register<CtKeyInput>();

	m_Entities.resize(l_MaxEntities);
	for (uint32 i = 0; i < m_Entities.size(); i++)
	{
		m_Entities[i].m_ID = i;
	}

	LOGM("EntityModule created. MaxEntities=" << l_MaxEntities, Info);
}

std::string EntityModule::GetName() const
{
	return "EntityModule";
}

std::shared_ptr<ComponentManager> EntityModule::GetComponentManager() const
{
	return m_ComponentManager;
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
		if (l_Entity.m_Mask == sc_InteractiveCameraMask)
		{
			if (IsKeyDown(KeyCode::W))
			{
				LOGM("Camera moving forward...", Debug);
			}
		}
		else if (l_Entity.m_Mask == sc_SuicidalEnemyMask)
		{
			if (Time::Now() >= m_ComponentManager->Get<CtKillTime>(l_Entity.m_ID).m_Time)
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

void EntityModule::AttachComponent(EntityID p_EntityID, ComponentID p_ComponentID)
{
	ASSERT(p_EntityID < m_Entities.size(), "Entity ID out of bounds. ID=" << p_EntityID);
	Entity& l_Entity = m_Entities[p_EntityID];
	ASSERT(!l_Entity.m_Mask.Get(p_ComponentID), "Component already attached to entity");
	l_Entity.m_Mask.Set(p_ComponentID, true);
}

void EntityModule::DetachComponent(EntityID p_EntityID, ComponentID p_ComponentID)
{
	ASSERT(p_EntityID < m_Entities.size(), "Entity ID out of bounds. ID=" << p_EntityID);
	Entity& l_Entity = m_Entities[p_EntityID];
	ASSERT(l_Entity.m_Mask.Get(p_ComponentID), "Component was not attached to entity");
	l_Entity.m_Mask.Set(p_ComponentID, false);
}

std::size_t EntityModule::CreateEntity()
{
	std::size_t l_Index = m_NextEntityIndex;
	THROW_IF_NOT(l_Index < m_Entities.size(), "Failed to create entity, max size exceeded. Max=" << m_Entities.size());

	Entity& l_Entity = m_Entities[l_Index];
	ASSERT(!l_Entity.IsAlive(), "Entity was alive");

	l_Entity.Revive();
	m_NextEntityIndex++;
	LOG_IF(m_NextEntityIndex == m_Entities.size(), "Entity count limit reached=" << m_Entities.size(), "ecs", Warning);

	return l_Index;
}

bool EntityModule::IsKeyDown(KeyCode p_Key) const
{
	return m_Keys.test(static_cast<std::size_t>(p_Key));
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
	m_Entities[CreateEntity()].m_Mask = sc_InteractiveCameraMask;
}

void EntityModule::CreateFixedCamera()
{
	m_Entities[CreateEntity()].m_Mask = sc_FixedCameraMask;
}

void EntityModule::CreateSuicidalEnemy(uint32 p_DurationSec)
{
	Entity& l_Entity = m_Entities[CreateEntity()];
	l_Entity.m_Mask = sc_SuicidalEnemyMask;
	m_ComponentManager->Get<CtKillTime>(l_Entity.m_ID).m_Time =
		Time(Duration(p_DurationSec, Duration::Precision::Second));
}

std::ostream& operator<<(std::ostream& os, const EntityModule::EntityList& p_Entities)
{
	for (const Entity& l_Entity : p_Entities)
	{
		if (l_Entity.m_Alive)
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