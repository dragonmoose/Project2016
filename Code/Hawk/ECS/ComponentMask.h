#pragma once
#include "Constants.h"
#include <string>
#include <bitset>

namespace Hawk {
namespace ECS {

class HAWK_DLL_EXPORT ComponentMask final
{
public:
	bool operator==(const ComponentMask& p_Other) const;
	ComponentMask& operator|(ComponentID p_ID);
	std::string ToString() const;
	void Clear();
	void Set(ComponentID p_ID, bool p_bValue);
	bool Get(ComponentID p_ID) const;
	std::size_t GetSize() const;
	std::size_t GetNumComponents() const;

private:
	using Bits = std::bitset<Constants::c_MaxComponents>;
	Bits m_Bits;
};

#define CMASK(c) Hawk::ECS::ComponentMask() | c

}
}