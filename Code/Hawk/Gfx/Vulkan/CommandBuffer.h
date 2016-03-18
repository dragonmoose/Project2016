#pragma once
#include "System.h"
#include "CommandPool.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class CommandBuffer final
{
public:
	CommandBuffer(std::shared_ptr<Device> p_Device, std::shared_ptr<CommandPool> p_CommandPool);
	~CommandBuffer();
	CommandBuffer(const CommandBuffer&) = delete;
	CommandBuffer& operator=(const CommandBuffer&) = delete;

	std::shared_ptr<CommandBuffer> CreateSecondary();
	void Reset(bool p_bReleaseResources) const;

	VkCommandBuffer GetHandle() const;

private:
	VkCommandBuffer m_Handle;
	std::shared_ptr<CommandPool> m_CommandPool;
	std::shared_ptr<Device> m_Device;
};

}
}
}