#include "pch.h"
#include "RenderPass.h"
#include <array>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

RenderPass::RenderPass(std::shared_ptr<Device> p_Device)
: m_Device(p_Device)
, m_Handle(VK_NULL_HANDLE)
, m_uiNumAttachments(2)
{
	std::vector<VkAttachmentDescription> l_Attachments;
	l_Attachments.resize(m_uiNumAttachments);
	VkAttachmentDescription& l_DepthAttachment = l_Attachments[0];
	VkAttachmentDescription& l_ColorAttachment = l_Attachments[1];

	l_DepthAttachment.format = m_Device->GetPhysicalDevice()->GetBackBufferDepthStencilFormat();
	l_DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	l_DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	l_DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	l_DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	l_DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	l_DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	l_DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference l_DepthAttachmentRef = {};
	l_DepthAttachmentRef.attachment = 0;
	l_DepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	l_ColorAttachment.format = m_Device->GetPhysicalDevice()->GetBackBufferColorFormat();
	l_ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	l_ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	l_ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	l_ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	l_ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	l_ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	l_ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference l_ColorAttachmentRef = {};
	l_ColorAttachmentRef.attachment = 1;
	l_ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription l_Subpass = {};
	l_Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	l_Subpass.colorAttachmentCount = 1;
	l_Subpass.pColorAttachments = &l_ColorAttachmentRef;
	l_Subpass.pDepthStencilAttachment = &l_DepthAttachmentRef;

	VkRenderPassCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	l_Info.attachmentCount = m_uiNumAttachments;
	l_Info.pAttachments = l_Attachments.data();
	l_Info.subpassCount = 1;
	l_Info.pSubpasses = &l_Subpass;
		
	VK_THROW_IF_NOT_SUCCESS(vkCreateRenderPass(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create render pass");
	LOG("RenderPass created", "vulkan", Debug);
}

RenderPass::~RenderPass()
{
	vkDestroyRenderPass(m_Device->GetHandle(), m_Handle, nullptr);
	LOG("RenderPass destroyed", "vulkan", Debug);
}

VkRenderPass RenderPass::GetHandle() const
{
	return m_Handle;
}

uint32 RenderPass::GetNumAttachments() const
{
	return m_uiNumAttachments;
}

}
}
}