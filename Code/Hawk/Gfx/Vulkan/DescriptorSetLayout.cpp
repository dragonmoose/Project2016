#include "pch.h"
#include "DescriptorSetLayout.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

DescriptorSetLayout::DescriptorSetLayout(std::shared_ptr<Device> p_Device, const std::vector<VkDescriptorSetLayoutBinding>& p_Bindings)
: m_Handle(VK_NULL_HANDLE)
, m_Device(p_Device)
{
	VkDescriptorSetLayoutCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	l_Info.bindingCount = (uint32)p_Bindings.size();
	l_Info.pBindings = p_Bindings.data();
	VK_THROW_IF_NOT_SUCCESS(vkCreateDescriptorSetLayout(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create descriptor set layout");
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(m_Device->GetHandle(), m_Handle, nullptr);
}

VkDescriptorSetLayout DescriptorSetLayout::GetHandle() const
{
	return m_Handle;
}


}
}
}