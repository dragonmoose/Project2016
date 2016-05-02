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
	Set(p_ID, true);
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

void ComponentMask::Set(ComponentID p_ID, bool p_bValue)
{
	ASSERT(p_ID < m_Bits.size(), "ComponentID out of bounds");
	m_Bits.set(p_ID, p_bValue);
}

bool ComponentMask::Get(ComponentID p_ID) const
{
	ASSERT(p_ID < m_Bits.size(), "ComponentID out of bounds");
	return m_Bits.test(p_ID);
}

std::size_t ComponentMask::GetSize() const
{
	return m_Bits.size();
}

std::size_t ComponentMask::GetNumComponents() const
{
	return m_Bits.count();
}

}
}