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
	void SetID(EntityID p_ID);
	EntityID GetID() const;
	bool HasMask(const ComponentMask& p_Mask) const;
	void Revive(const ComponentMask& p_Mask);
	void MarkPendingKill();
	void Kill();
	bool IsAlive() const;
	bool IsPendingKill() const;

private:
	EntityID m_ID;
	ComponentMask m_Mask;
	boost::tribool m_Alive;
};

}
}