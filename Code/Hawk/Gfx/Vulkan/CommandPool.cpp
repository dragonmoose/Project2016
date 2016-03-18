#include "pch.h"
#include "CommandPool.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

CommandPool::CommandPool(std::shared_ptr<Device> p_Device, uint32 p_uiQueueFamilyIndex, bool p_bAllowIndividualBufferReset, bool p_bShortLifetimeObjects)
: m_Handle(VK_NULL_HANDLE)
, m_Device(p_Device)
, m_bAllowIndividualBufferReset(p_bAllowIndividualBufferReset)
{
	VkCommandPoolCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	l_Info.flags |= p_bShortLifetimeObjects ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;
	l_Info.flags |= p_bAllowIndividualBufferReset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;
	l_Info.queueFamilyIndex = p_uiQueueFamilyIndex;

	VK_THROW_IF_NOT_SUCCESS(vkCreateCommandPool(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create command pool");
	LOG("CommandPool created. QueueFamilyIndex=" << p_uiQueueFamilyIndex << " IndividualBufferReset=" << p_bAllowIndividualBufferReset << " ShortLifetimeObjects=" << p_bShortLifetimeObjects, "vulkan", Debug);
}

CommandPool::~CommandPool()
{
	// TODO: Make sure that no command buffers allocated from pool are not in pending execution state
	vkDestroyCommandPool(m_Device->GetHandle(), m_Handle, nullptr);
	LOG("CommandPool destroyed", "vulkan", Debug);
}

bool CommandPool::AllowsIndividualBufferReset() const
{
	return m_bAllowIndividualBufferReset;
}

VkCommandPool CommandPool::GetHandle() const
{
	return m_Handle;
}

void CommandPool::Reset(bool p_bReleaseResources) const
{
	VK_THROW_IF_NOT_SUCCESS(vkResetCommandPool(m_Device->GetHandle(), m_Handle, p_bReleaseResources ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0), "Failed to reset command pool. ReleaseResources=" << p_bReleaseResources);
}

}
}
}