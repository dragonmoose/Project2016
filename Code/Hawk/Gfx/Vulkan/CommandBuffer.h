#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class CommandBuffer final
{
public:
	CommandBuffer();
	~CommandBuffer();
	CommandBuffer(const CommandBuffer&) = delete;
	CommandBuffer& operator=(const CommandBuffer&) = delete;

	VkCommandBuffer GetHandle() const;

private:
	VkCommandBuffer m_Handle;
};

}
}
}