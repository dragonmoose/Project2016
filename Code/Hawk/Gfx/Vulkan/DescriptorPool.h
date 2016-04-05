#pragma once
#include "System.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;

class DescriptorPool final
{
public:
	DescriptorPool(std::shared_ptr<Device> p_Device, bool p_bAllowIndividualFree, uint32 p_uiMaxSets, const std::vector<VkDescriptorPoolSize>& p_PoolSizes);
	~DescriptorPool();
	DescriptorPool(const DescriptorPool&) = delete;
	DescriptorPool& operator=(const DescriptorPool&) = delete;

	bool AllowsIndividualFree() const;
	VkDescriptorPool GetHandle() const;
	void Reset() const;

private:
	VkDescriptorPool m_Handle;
	std::shared_ptr<Device> m_Device;
	bool m_bAllowsIndividualFree;
};

}
}
}