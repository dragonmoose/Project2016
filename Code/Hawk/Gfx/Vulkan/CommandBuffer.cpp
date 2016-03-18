#include "pch.h"
#include "CommandBuffer.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

CommandBuffer::CommandBuffer(VkCommandBuffer p_Handle)
: m_Handle(p_Handle)
{
	ASSERT(p_Handle, "Null handle");
	LOG("CommandBuffer created", "vulkan", Debug);
}

CommandBuffer::~CommandBuffer()
{
	LOG("CommandBuffer destroyed", "vulkan", Debug);
}

VkCommandBuffer CommandBuffer::GetHandle() const
{
	return m_Handle;
}

std::shared_ptr<CommandBuffer> CommandBuffer::CreateSecondary()
{
	NOT_IMPLEMENTED();
	return nullptr;
}

void CommandBuffer::Reset(bool p_bReleaseResources) const
{
	VK_THROW_IF_NOT_SUCCESS(vkResetCommandBuffer(m_Handle, p_bReleaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0), "Failed to reset command buffer");
}

}
}
}