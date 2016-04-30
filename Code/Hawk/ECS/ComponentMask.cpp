#include "pch.h"
#include "ComponentMask.h"

namespace Hawk {
namespace ECS {

bool ComponentMask::operator==(const ComponentMask& p_Other) const
{
	return m_Bits == p_Other.m_Bits;
}

ComponentMask& ComponentMask::operator|(ComponentID p_ID)
{
	ASSERT(p_ID < m_Bits.size(), "ComponentID out of bounds");
	Set(p_ID);
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

void ComponentMask::Set(ComponentID p_ID)
{
	m_Bits.set(p_ID);
}

}
}