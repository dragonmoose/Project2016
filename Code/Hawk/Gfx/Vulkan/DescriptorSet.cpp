#include "pch.h"
#include "DescriptorSet.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

DescriptorSet::DescriptorSet(std::shared_ptr<Device> p_Device, std::shared_ptr<DescriptorPool> p_DescriptorPool, const DescriptorSetLayout* p_SetLayout)
: m_Handle(VK_NULL_HANDLE)
, m_Device(p_Device)
, m_Pool(p_DescriptorPool)
{
	VkDescriptorSetAllocateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	l_Info.descriptorPool = m_Pool->GetHandle();
	l_Info.descriptorSetCount = 1;
	VkDescriptorSetLayout l_SetLayout = p_SetLayout->GetHandle();
	l_Info.pSetLayouts = &l_SetLayout;
	VK_THROW_IF_NOT_SUCCESS(vkAllocateDescriptorSets(m_Device->GetHandle(), &l_Info, &m_Handle), "Failed to allocate descriptor set");
}

DescriptorSet::~DescriptorSet()
{
	VkResult l_Result = vkFreeDescriptorSets(m_Device->GetHandle(), m_Pool->GetHandle(), 1, &m_Handle);
	LOG_IF(l_Result != VK_SUCCESS, "Failed to free descriptor set", "vulkan", Error);
}

VkDescriptorSet DescriptorSet::GetHandle() const
{
	return m_Handle;
}

void DescriptorSet::Reset() const
{
	// TODO: Possible to reuse this instance?
	ASSERT(m_Pool->AllowsIndividualFree(), "Set not created from a descriptor pool that allows sets to be freed individually");
	VK_THROW_IF_NOT_SUCCESS(vkFreeDescriptorSets(m_Device->GetHandle(), m_Pool->GetHandle(), 1, &m_Handle), "Failed to reset descriptor set");
}


}
}
}