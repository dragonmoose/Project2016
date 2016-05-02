#pragma once
#include "Constants.h"
#include "System/Types.h"
#include "Debug/Assert.h"
#include <boost/bimap.hpp>
#include <boost/type_index.hpp>
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
		Register(T::ID, sizeof(T));
#if HAWK_DEBUG
		std::string l_Name = boost::typeindex::type_id<T>().pretty_name();
		std::size_t l_Pos = l_Name.find_last_of(':');
		if (l_Pos != std::string::npos)
		{
			l_Name = l_Name.substr(l_Pos + 1);
		}
		m_DebugNameMap.left.insert(DebugNameMap::left_value_type(T::ID, l_Name));
#endif
	}

	template<class T>
	T& Get(EntityID p_EntityID) const
	{
		return reinterpret_cast<T&>(Get(p_EntityID, T::ID, sizeof(T)));
	}
	
	std::size_t GetNumComponents() const;

#ifdef HAWK_DEBUG
	ComponentID GetIDFromName(const std::string& p_Name) const;
	std::string GetNameFromID(ComponentID p_ID) const;
#endif

private:
	void Register(ComponentID p_ID, std::size_t p_Size);
	uint8& Get(EntityID p_EntityID, ComponentID p_ComponentID, std::size_t p_Size) const;

	const std::size_t c_MaxEntities;

	using Components = std::vector<uint8*>;
	Components m_Components;
	ComponentID m_NextRegisterID;
#ifdef HAWK_DEBUG
	using DebugNameMap = boost::bimap<ComponentID, std::string>;
	DebugNameMap m_DebugNameMap;
#endif
};

}
}