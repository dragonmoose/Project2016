#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class CommandPool final
{
public:
	CommandPool();
	~CommandPool();
	CommandPool(const CommandPool&) = delete;
	CommandPool& operator=(const CommandPool&) = delete;

	VkCommandPool GetHandle() const;

private:
	VkCommandPool m_Handle;
};

}
}
}