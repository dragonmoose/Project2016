#include "pch.h"
#include "Framebuffer.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

Framebuffer::Framebuffer(std::shared_ptr<Device> p_Device)
: m_Device(p_Device)
, m_Handle(VK_NULL_HANDLE)
{
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