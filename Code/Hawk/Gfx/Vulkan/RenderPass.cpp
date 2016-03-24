#include "pch.h"
#include "RenderPass.h"
#include <array>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

RenderPass::RenderPass(std::shared_ptr<Device> p_Device)
: m_Device(p_Device)
, m_Handle(VK_NULL_HANDLE)
{
	std::array<VkAttachmentDescription, 2> l_Attachments;
	VkAttachmentDescription& l_DepthAttachment = l_Attachments[0];
	VkAttachmentDescription& l_ColorAttachment = l_Attachments[1];

	l_DepthAttachment.format = m_Device->GetPhysicalDevice()->GetBackBufferDepthFormat();
	l_DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	l_DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	l_DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	l_DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	l_DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	l_DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	l_DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	l_ColorAttachment.format = m_Device->GetPhysicalDevice()->GetBackBufferColorFormat();
	l_ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	l_ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	l_ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	l_ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	l_ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	l_ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	l_ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkRenderPassCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	l_Info.attachmentCount = l_Attachments.size();
	l_Info.pAttachments = l_Attachments.data();
		
	//VK_THROW_IF_NOT_SUCCESS(vkCreateRenderPass(m_Device->GetHandle(), &l_Info, nullptr, &l_RenderPass), "Failed to create render pass");*/

}

RenderPass::~RenderPass()
{
	//vkDestroyRenderPass(m_Device->GetHandle(), m_Handle, nullptr);
	LOG("RenderPass destroyed", "vulkan", Debug);
}

VkRenderPass RenderPass::GetHandle() const
{
	return m_Handle;
}

}
}
}