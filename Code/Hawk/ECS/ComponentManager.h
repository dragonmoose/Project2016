#pragma once
#include "Constants.h"
#include "System/Types.h"
#include "Debug/Assert.h"
#include <vector>

namespace Hawk {
namespace ECS {

class HAWK_DLL_EXPORT ComponentManager
{
public:
	ComponentManager(std::size_t m_MaxEntities);
	~ComponentManager();
	ComponentManager(ComponentManager&) = delete;
	ComponentManager& operator=(const ComponentManager&) = delete;

	template<class T>
	void Register()
	{
		ASSERT(T::ID == m_NextRegisterID, "Given ComponentID=" << T::ID << " Expected=" << m_NextRegisterID);
		ASSERT(m_NextRegisterID < Constants::c_MaxComponents, "MaxComponents exceeded, constant needs to be adjusted");

		if (T::ID == m_Components.size())
		{
			m_Components.resize(m_Components.size() + 1);
		}
		const std::size_t c_MemSize = sizeof(T) * c_MaxEntities;
		m_Components[T::ID] = new uint8[c_MemSize];
		std::memset(m_Components[T::ID], 0u, c_MemSize);

		++m_NextRegisterID;
	}

	template<class T>
	T& Get(EntityID p_EntityID) const
	{
		ASSERT(p_EntityID < c_MaxEntities, "EntityID out of bounds=" << p_EntityID);
		ASSERT(T::ID < m_Components.size(), "ComponentID out of bounds=" << T::ID);
		return reinterpret_cast<T&>(m_Components[T::ID][p_EntityID * sizeof(T)]);
	}
	
	std::size_t GetNumComponents() const;

private:
	const std::size_t c_MaxEntities;

	using Components = std::vector<uint8*>;
	Components m_Components;
	ComponentID m_NextRegisterID;
};

}
}