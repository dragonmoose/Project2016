#include "pch.h"
#include "CommandBuffer.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

CommandBuffer::CommandBuffer(std::shared_ptr<Device> p_Device, std::shared_ptr<CommandPool> p_CommandPool, bool p_bOneTimeSubmit)
: m_Device(p_Device)
, m_CommandPool(p_CommandPool)
, m_Handle(VK_NULL_HANDLE)
{
	VkCommandBufferAllocateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	l_Info.commandPool = m_CommandPool->GetHandle();
	l_Info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	l_Info.commandBufferCount = 1;

	InitBeginInfo(p_bOneTimeSubmit);

	VK_THROW_IF_NOT_SUCCESS(vkAllocateCommandBuffers(m_Device->GetHandle(), &l_Info, &m_Handle), "Failed to allocate primary command buffer");
	LOG("Primary command buffer created", "vulkan", Debug);
}

CommandBuffer::~CommandBuffer()
{
	// TODO: Make sure no elements of buffer are in pending execution state
	vkFreeCommandBuffers(m_Device->GetHandle(), m_CommandPool->GetHandle(), 1, &m_Handle);
	LOG("CommandBuffer destroyed", "vulkan", Debug);
}

VkCommandBuffer CommandBuffer::GetHandle() const
{
	return m_Handle;
}

void CommandBuffer::Begin()
{
	VK_THROW_IF_NOT_SUCCESS(vkBeginCommandBuffer(m_Handle, &m_BeginInfo), "Failed to begin recording of command buffer");
}

void CommandBuffer::End()
{
	VK_THROW_IF_NOT_SUCCESS(vkEndCommandBuffer(m_Handle), "Failed to end recording of command buffer");
}

std::shared_ptr<CommandBuffer> CommandBuffer::CreateSecondary()
{
	NOT_IMPLEMENTED();
	return nullptr;
}

void CommandBuffer::Reset(bool p_bReleaseResources)
{
	ASSERT(m_CommandPool->AllowsIndividualBufferReset(), "Buffer not created from a command pool that allows buffers to be reset individually");
	VK_THROW_IF_NOT_SUCCESS(vkResetCommandBuffer(m_Handle, p_bReleaseResources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0), "Failed to reset command buffer");
}

void CommandBuffer::InitBeginInfo(bool p_bOneTimeSubmit)
{
	m_BeginInfo = {};
	m_BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	m_BeginInfo.flags |= p_bOneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;
	// TODO: Handle additional flags and secondary command buffers
}

}
}
}