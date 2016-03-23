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
	CommandBuffer(std::shared_ptr<Device> p_Device, std::shared_ptr<CommandPool> p_CommandPool, bool p_bOneTimeSubmit = false);
	~CommandBuffer();
	CommandBuffer(const CommandBuffer&) = delete;
	CommandBuffer& operator=(const CommandBuffer&) = delete;

	std::shared_ptr<CommandBuffer> CreateSecondary();
	void Reset(bool p_bReleaseResources);

	VkCommandBuffer GetHandle() const;

	void Begin();
	void End();

private:
	void InitBeginInfo(bool p_bOneTimeSubmit);

	VkCommandBuffer m_Handle;
	std::shared_ptr<CommandPool> m_CommandPool;
	std::shared_ptr<Device> m_Device;
	VkCommandBufferBeginInfo m_BeginInfo;
};

}
}
}