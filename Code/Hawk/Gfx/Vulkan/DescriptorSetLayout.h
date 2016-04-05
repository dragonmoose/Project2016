#pragma once
#include "System.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;

class DescriptorSetLayout final
{
public:
	DescriptorSetLayout(std::shared_ptr<Device> p_Device, const std::vector<VkDescriptorSetLayoutBinding>& p_Bindings);
	~DescriptorSetLayout();
	DescriptorSetLayout(const DescriptorSetLayout&) = delete;
	DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

	VkDescriptorSetLayout GetHandle() const;

private:
	VkDescriptorSetLayout m_Handle;
	std::shared_ptr<Device> m_Device;
};

}
}
}