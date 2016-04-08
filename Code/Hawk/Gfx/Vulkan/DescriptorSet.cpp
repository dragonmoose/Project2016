#include "pch.h"
#include "DescriptorSet.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "PipelineLayout.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

DescriptorSet::DescriptorSet(std::shared_ptr<Device> p_Device, std::shared_ptr<DescriptorPool> p_DescriptorPool, std::shared_ptr<PipelineLayout> p_PipelineLayout, const DescriptorSetLayout* p_SetLayout)
: m_Handle(VK_NULL_HANDLE)
, m_Device(p_Device)
, m_Pool(p_DescriptorPool)
, m_PipelineLayout(p_PipelineLayout)
{
	VkDescriptorSetAllocateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	l_Info.descriptorPool = m_Pool->GetHandle();
	l_Info.descriptorSetCount = 1; // Number of sets to allocate, must match number of layouts in pSetLayouts
	VkDescriptorSetLayout l_SetLayout = p_SetLayout->GetHandle();
	l_Info.pSetLayouts = &l_SetLayout;

	// Returns array of handles to sets, but for now we always specify descriptorSetCount = 1
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

void DescriptorSet::Issue(VkCommandBuffer p_CommandBufferHandle) const
{
	vkCmdBindDescriptorSets(p_CommandBufferHandle, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout->GetHandle(), 0, 1, &m_Handle, 0, nullptr);
}

void DescriptorSet::Bind(const VkDescriptorBufferInfo* p_BufferInfo, VkDescriptorType p_Type, uint32 p_uiBinding) const
{
	VkWriteDescriptorSet l_WriteDescSet = {};
	l_WriteDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	l_WriteDescSet.dstSet = m_Handle;
	l_WriteDescSet.dstBinding = p_uiBinding;
	l_WriteDescSet.descriptorCount = 1;
	l_WriteDescSet.descriptorType = p_Type;
	l_WriteDescSet.pBufferInfo = p_BufferInfo;
	vkUpdateDescriptorSets(m_Device->GetHandle(), 1, &l_WriteDescSet, 0, nullptr);
}


}
}
}