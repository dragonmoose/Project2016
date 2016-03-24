#pragma once
#include "System.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class RenderPass;

class Framebuffer final
{
public:
	Framebuffer(std::shared_ptr<Device> p_Device, const RenderPass* p_RenderPass);
	~Framebuffer();
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer& operator=(const Framebuffer&) = delete;

	VkFramebuffer GetHandle() const;

private:
	std::shared_ptr<Device> m_Device;
	VkFramebuffer m_Handle;
};

}
}
}