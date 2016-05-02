#pragma once
#include "ComponentMask.h"
#include <boost/logic/tribool.hpp>
#include <bitset>

namespace Hawk {
namespace ECS {

class Entity final
{
public:
	Entity();
	Entity(const std::string& p_DebugName);

private:
	friend class EntityModule;
	friend std::ostream& operator<<(std::ostream&, const std::vector<Entity>&);
	friend std::ostream& operator<<(std::ostream& os, const Entity& p_Entity);

	void Revive();
	void MarkPendingKill();
	void Kill();
	bool IsAlive() const;
	bool IsPendingKill() const;

	EntityID m_ID;
	ComponentMask m_Mask;
	boost::tribool m_Alive;

#ifdef HAWK_DEBUG
	std::string m_DebugName;
#endif
};

std::ostream& operator<<(std::ostream& os, const Entity& p_Entity);

}
}