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


}
}