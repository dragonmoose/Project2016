#include "pch.h"
#include "Entity.h"
#include <iomanip>

namespace Hawk {
namespace ECS {

Entity::Entity()
: m_ID(0)
{
}

Entity::Entity(const std::string& p_DebugName)
: Entity()
{
#ifdef HAWK_DEBUG
	m_DebugName = p_DebugName;
#endif
}

void Entity::Revive()
{
	ASSERT(!m_Alive, "Entity was not dead");
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

std::ostream& operator<<(std::ostream& os, const Entity& p_Entity)
{
	os << std::left;
	os << std::setw(8) << "Name:" << (p_Entity.m_DebugName.empty() ? "N/A" : p_Entity.m_DebugName) << "\n";
	os << std::setw(8) << "ID:" << p_Entity.m_ID << "\n";
	os << std::setw(8) << "Alive: " << p_Entity.m_Alive << "\n";
	return os;
}

}
}