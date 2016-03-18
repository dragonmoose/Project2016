#include "pch.h"
#include "CommandBuffer.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

CommandBuffer::CommandBuffer()
: m_Handle(VK_NULL_HANDLE)
{
	LOG("Created CommandBuffer", "vulkan", Debug);
}

CommandBuffer::~CommandBuffer()
{
	LOG("CommandBuffer destroyed", "vulkan", Debug);
}

VkCommandBuffer CommandBuffer::GetHandle() const
{
	return m_Handle;
}

}
}
}