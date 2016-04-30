#pragma once
#include "Constants.h"
#include <string>
#include <bitset>

namespace Hawk {
namespace ECS {

class ComponentMask final
{
public:
	bool operator==(const ComponentMask& p_Other) const;
	ComponentMask& operator|(ComponentID p_ID);
	std::string ToString() const;
	void Clear();

private:
	void Set(ComponentID p_ID);
	using Bits = std::bitset<Constants::c_MaxComponents>;
	Bits m_Bits;
};

}
}