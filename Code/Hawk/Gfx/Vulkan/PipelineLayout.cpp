#include "pch.h"
#include "PipelineLayout.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

PipelineLayout::PipelineLayout(std::shared_ptr<Device> p_Device, const std::vector<VkDescriptorSetLayout>& p_SetLayouts)
: m_Handle(VK_NULL_HANDLE)
, m_Device(p_Device)
{
	VkPipelineLayoutCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	l_Info.setLayoutCount = (uint32)p_SetLayouts.size();
	l_Info.pSetLayouts = p_SetLayouts.data();
	// TODO: Push constants

	VK_THROW_IF_NOT_SUCCESS(vkCreatePipelineLayout(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create pipeline layout");
}

PipelineLayout::~PipelineLayout()
{
	vkDestroyPipelineLayout(m_Device->GetHandle(), m_Handle, nullptr);
}

VkPipelineLayout PipelineLayout::GetHandle() const
{
	return m_Handle;
}

}
}
}