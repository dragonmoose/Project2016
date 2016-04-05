#include "pch.h"
#include "DescriptorPool.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

DescriptorPool::DescriptorPool(std::shared_ptr<Device> p_Device, bool p_bAllowIndividualFree, uint32 p_uiMaxSets, const std::vector<VkDescriptorPoolSize>& p_PoolSizes)
: m_Handle(VK_NULL_HANDLE)
, m_Device(p_Device)
, m_bAllowsIndividualFree(p_bAllowIndividualFree)
{
	VkDescriptorPoolCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	l_Info.flags |= m_bAllowsIndividualFree ? VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0;
	l_Info.maxSets = p_uiMaxSets; // Maximum number of sets that can be allocated from pool (requesting more results in an error)
	l_Info.pPoolSizes = p_PoolSizes.data();
	l_Info.poolSizeCount = (uint32)p_PoolSizes.size();

	VK_THROW_IF_NOT_SUCCESS(vkCreateDescriptorPool(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create descriptor pool");
}

DescriptorPool::~DescriptorPool()
{
	// TODO: Make sure all descriptor sets allocated from pool have been destroyed
	vkDestroyDescriptorPool(m_Device->GetHandle(), m_Handle, nullptr);
}

bool DescriptorPool::AllowsIndividualFree() const
{
	return m_bAllowsIndividualFree;
}

VkDescriptorPool DescriptorPool::GetHandle() const
{
	return m_Handle;
}

void DescriptorPool::Reset() const
{
	VK_THROW_IF_NOT_SUCCESS(vkResetDescriptorPool(m_Device->GetHandle(), m_Handle, 0), "Failed to reset descriptor pool");
}


}
}
}