#include "pch.h"
#include "ComponentMask.h"

namespace Hawk {
namespace ECS {

ComponentMask::ComponentMask(ComponentFlags p_Flag)
{
	Set(p_Flag);
}

bool ComponentMask::operator==(const ComponentMask& p_Other) const
{
	return m_Bits == p_Other.m_Bits;
}

ComponentMask& ComponentMask::operator|=(ComponentFlags p_Flag)
{
	ASSERT(p_Flag < ComponentFlags::_Count, "ComponentFlag out of bounds");
	Set(p_Flag);
	return *this;
}
ComponentMask& ComponentMask::operator|(ComponentFlags p_Flag)
{
	ASSERT(p_Flag < ComponentFlags::_Count, "ComponentFlag out of bounds");
	Set(p_Flag);
	return *this;
}

std::string ComponentMask::ToString() const
{
	return m_Bits.to_string();
}

void ComponentMask::Clear()
{
	m_Bits.reset();
}

void ComponentMask::Set(ComponentFlags p_Flag)
{
	m_Bits.set(static_cast<std::size_t>(p_Flag));
}


ComponentMask operator|(ComponentFlags lhs, ComponentFlags rhs)
{
	ComponentMask l_Mask;
	l_Mask |= lhs;
	l_Mask |= rhs;
	return l_Mask;
}

}
}