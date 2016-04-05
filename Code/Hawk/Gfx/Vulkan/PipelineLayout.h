#pragma once
#include "System.h"
#include "DescriptorSetLayout.h"
#include <memory>
#include <vector>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;

class PipelineLayout final
{
public:
	PipelineLayout(std::shared_ptr<Device> p_Device, const std::vector<VkDescriptorSetLayout>& p_SetLayouts);
	~PipelineLayout();
	PipelineLayout(const PipelineLayout&) = delete;
	PipelineLayout& operator=(const PipelineLayout&) = delete;

	VkPipelineLayout GetHandle() const;

private:
	VkPipelineLayout m_Handle;
	std::shared_ptr<Device> m_Device;
};

}
}
}