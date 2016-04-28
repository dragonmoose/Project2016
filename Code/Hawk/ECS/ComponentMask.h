#pragma once
#include "ComponentFlags.h"
#include <string>
#include <bitset>

namespace Hawk {
namespace ECS {

class ComponentMask final
{
public:
	ComponentMask() = default;
	ComponentMask(ComponentFlags p_Flag);
	bool operator==(const ComponentMask& p_Other) const;
	ComponentMask& operator|=(ComponentFlags p_Flag);
	ComponentMask& operator|(ComponentFlags p_Flag);
	std::string ToString() const;
	void Clear();

private:
	void Set(ComponentFlags p_Flag);
	using Bits = std::bitset<static_cast<std::size_t>(ComponentFlags::_Count)>;
	Bits m_Bits;
};

ComponentMask operator|(ComponentFlags lhs, ComponentFlags rhs);

}
}