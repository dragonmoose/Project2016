#pragma once
#include "System.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;
class DescriptorPool;
class DescriptorSetLayout;
class PipelineLayout;

class DescriptorSet final
{
public:
	DescriptorSet(std::shared_ptr<Device> p_Device, std::shared_ptr<DescriptorPool> p_DescriptorPool, std::shared_ptr<PipelineLayout> p_PipelineLayout, const DescriptorSetLayout* p_SetLayout);
	~DescriptorSet();
	DescriptorSet(const DescriptorSet&) = delete;
	DescriptorSet& operator=(const DescriptorSet&) = delete;

	VkDescriptorSet GetHandle() const;
	void Reset() const;
	void Issue(VkCommandBuffer p_CommandBufferHandle) const;
	void Bind(const VkDescriptorBufferInfo* p_BufferInfo, VkDescriptorType p_Type, uint32 p_uiBinding) const;

private:
	VkDescriptorSet m_Handle;
	std::shared_ptr<PipelineLayout> m_PipelineLayout;
	std::shared_ptr<Device> m_Device;
	std::shared_ptr<DescriptorPool> m_Pool;
};

}
}
}