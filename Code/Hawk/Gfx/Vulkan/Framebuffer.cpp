#include "pch.h"
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "ImageView.h"
#include "Swapchain.h"
#include <array>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

FrameBuffer::FrameBuffer(std::shared_ptr<Device> p_Device, const RenderPass* p_RenderPass, const ImageView* p_ColorView, const ImageView* p_DepthStencilView, VkExtent2D p_Extent)
: m_Device(p_Device)
, m_Handle(VK_NULL_HANDLE)
{
	std::array<VkImageView, 2> l_Attachments;
	l_Attachments[0] = p_DepthStencilView->GetHandle();
	l_Attachments[1] = p_ColorView->GetHandle();

	VkFramebufferCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	l_Info.renderPass = p_RenderPass->GetHandle();
	l_Info.attachmentCount = p_RenderPass->GetNumAttachments();
	l_Info.pAttachments = l_Attachments.data();

	uint32 l_uiMaxWidth;
	uint32 l_uiMaxHeight;
	uint32 l_uiMaxLayers;
	p_Device->GetPhysicalDevice()->GetFrameBufferLimits(l_uiMaxWidth, l_uiMaxHeight, l_uiMaxLayers);
	LOG("FrameBuffer limits. MaxWidth=" << l_uiMaxWidth << " MaxHeight=" << l_uiMaxHeight << " MaxLayers=" << l_uiMaxLayers, "vulkan", Debug);

	THROW_IF_NOT(p_Extent.width <= l_uiMaxWidth, "Surface width exceeds framebuffer width limit. Width=" << p_Extent.width << " MaxWidth=" << l_uiMaxWidth);
	THROW_IF_NOT(p_Extent.height <= l_uiMaxHeight, "Surface height exceeds framebuffer height limit. Height=" << p_Extent.height << " MaxHeight=" << l_uiMaxHeight);

	l_Info.width = p_Extent.width;
	l_Info.height = p_Extent.height;
	l_Info.layers = 1;

	VK_THROW_IF_NOT_SUCCESS(vkCreateFramebuffer(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create FrameBuffer");
	LOG("FrameBuffer created", "vulkan", Debug);

}

FrameBuffer::~FrameBuffer()
{
	vkDestroyFramebuffer(m_Device->GetHandle(), m_Handle, nullptr);
	LOG("FrameBuffer destroyed", "vulkan", Debug);
}

VkRenderPass FrameBuffer::GetHandle() const
{
	return m_Handle;
}

}
}
}