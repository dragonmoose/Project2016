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
	uint32 GetNumAttachments() const;

private:
	std::shared_ptr<Device> m_Device;
	VkRenderPass m_Handle;
	uint32 m_uiNumAttachments;
};

}
}
}