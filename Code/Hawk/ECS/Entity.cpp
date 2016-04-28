#include "pch.h"
#include "Entity.h"

namespace Hawk {
namespace ECS {

Entity::Entity()
: m_ID(0)
{
}

void Entity::SetID(EntityID p_ID)
{
	m_ID = p_ID;
}

EntityID Entity::GetID() const
{
	return m_ID;
}

bool Entity::HasMask(const ComponentMask& p_Mask) const
{
	return m_Mask == p_Mask;
}

void Entity::Revive(const ComponentMask& p_Mask)
{
	ASSERT(!m_Alive, "Entity was not dead");
	m_Mask = p_Mask;
	m_Alive = true;
}

void Entity::MarkPendingKill()
{
	ASSERT(m_Alive, "Entity was not alive");
	m_Alive = boost::indeterminate;
}

void Entity::Kill()
{
	ASSERT(boost::indeterminate(m_Alive), "Entity not in pending kill state");
	m_Mask.Clear();
	m_Alive = false;
	LOG("Entity killed. ID=" << m_ID, "ecs", Trace);
}

bool Entity::IsAlive() const
{
	return m_Alive;
}

bool Entity::IsPendingKill() const
{
	return boost::indeterminate(m_Alive);
}

}
}