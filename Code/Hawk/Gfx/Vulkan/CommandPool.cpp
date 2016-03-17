#include "pch.h"
#include "CommandPool.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

CommandPool::CommandPool(std::shared_ptr<Device> p_Device, uint32 p_uiQueueFamilyIndex, bool p_bIndividualBufferReset, bool p_bShortLifetimeObjects)
: m_Handle(VK_NULL_HANDLE)
, m_Device(p_Device)
{
	VkCommandPoolCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	l_Info.flags |= p_bShortLifetimeObjects ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0;
	l_Info.flags |= p_bIndividualBufferReset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;
	l_Info.queueFamilyIndex = p_uiQueueFamilyIndex;
	VK_THROW_IF_NOT_SUCCESS(vkCreateCommandPool(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create command pool");
}

CommandPool::~CommandPool()
{
	LOG("CommandPool destroyed", "vulkan", Debug);
}

VkCommandPool CommandPool::GetHandle() const
{
	return m_Handle;
}

}
}
}