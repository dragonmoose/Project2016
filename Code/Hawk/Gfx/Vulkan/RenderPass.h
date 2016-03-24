#pragma once
#include "System.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class RenderPass final
{
public:
	RenderPass(std::shared_ptr<Device> p_Device);
	~RenderPass();
	RenderPass(const RenderPass&) = delete;
	RenderPass& operator=(const RenderPass&) = delete;

	VkRenderPass GetHandle() const;

private:
	std::shared_ptr<Device> m_Device;
	VkRenderPass m_Handle;
};

}
}
}