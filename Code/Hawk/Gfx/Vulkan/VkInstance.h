#pragma once
#include "VkSystem.h"

namespace Hawk {
namespace Gfx {

class VkInstance final
{
public:
	VkInstance();
	~VkInstance();
	VkInstance(const VkInstance&) = delete;
	VkInstance& operator=(const VkInstance&) = delete;

private:
	::VkInstance m_Handle;
};

}
}