#pragma once
#include "System.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;
class RenderPass;

class GraphicsPipeline final
{
public:
	GraphicsPipeline(std::shared_ptr<Device> p_Device, const RenderPass* p_RenderPass);
	~GraphicsPipeline();
	GraphicsPipeline(const GraphicsPipeline&) = delete;
	GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

	VkPipeline GetHandle() const;

private:

	VkPipeline m_Handle;
	std::shared_ptr<Device> m_Device;
};

}
}
}