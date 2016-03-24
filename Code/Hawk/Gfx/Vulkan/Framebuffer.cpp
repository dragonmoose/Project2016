#include "pch.h"
#include "Framebuffer.h"
#include "RenderPass.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

Framebuffer::Framebuffer(std::shared_ptr<Device> p_Device, const RenderPass* p_RenderPass)
: m_Device(p_Device)
, m_Handle(VK_NULL_HANDLE)
{
	VkFramebufferCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	l_Info.renderPass = p_RenderPass->GetHandle();
	l_Info.attachmentCount = p_RenderPass->GetNumAttachments();
	VK_THROW_IF_NOT_SUCCESS(vkCreateFramebuffer(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create Framebuffer");
	LOG("Framebuffer created", "vulkan", Debug);

}

Framebuffer::~Framebuffer()
{
	vkDestroyFramebuffer(m_Device->GetHandle(), m_Handle, nullptr);
	LOG("Framebuffer destroyed", "vulkan", Debug);
}

VkRenderPass Framebuffer::GetHandle() const
{
	return m_Handle;
}

}
}
}