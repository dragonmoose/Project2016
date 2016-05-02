#include "pch.h"
#include "ComponentManager.h"

namespace Hawk {
namespace ECS {

ComponentManager::ComponentManager(std::size_t p_MaxEntities)
: c_MaxEntities(p_MaxEntities)
, m_NextRegisterID(0)
{
}

ComponentManager::~ComponentManager()
{
	for (auto l_List : m_Components)
	{
		delete[] l_List;
	}
}

std::size_t ComponentManager::GetNumComponents() const
{
	return m_Components.size();
}

#ifdef HAWK_DEBUG
ComponentID ComponentManager::GetIDFromName(const std::string& p_Name) const
{
	auto l_Itr = m_DebugNameMap.right.find(p_Name);
	ASSERT(l_Itr != m_DebugNameMap.right.end(), "");
	return l_Itr->second;
}

std::string ComponentManager::GetNameFromID(ComponentID p_ID) const
{
	auto l_Itr = m_DebugNameMap.left.find(p_ID);
	ASSERT(l_Itr != m_DebugNameMap.left.end(), "");
	return l_Itr->second;
}
#endif

void ComponentManager::Register(ComponentID p_ID, std::size_t p_Size)
{
	ASSERT(p_ID == m_NextRegisterID, "Given ComponentID=" << p_ID << " Expected=" << m_NextRegisterID);
	ASSERT(m_NextRegisterID < Constants::c_MaxComponents, "MaxComponents exceeded, constant needs to be adjusted");

	if (p_ID == m_Components.size())
	{
		m_Components.resize(m_Components.size() + 1);
	}
	const std::size_t c_MemSize = p_Size * c_MaxEntities;
	m_Components[p_ID] = new uint8[c_MemSize];
	std::memset(m_Components[p_ID], 0u, c_MemSize);

	++m_NextRegisterID;
}

uint8& ComponentManager::Get(EntityID p_EntityID, ComponentID p_ComponentID, std::size_t p_Size) const
{
	ASSERT(p_EntityID < c_MaxEntities, "EntityID out of bounds=" << p_EntityID);
	ASSERT(p_ComponentID < m_Components.size(), "ComponentID out of bounds=" << p_ComponentID);
	return m_Components[p_ComponentID][p_EntityID * p_Size];
}

}
}