#pragma once
#include "System.h"
#include "CommandBuffer.h"
#include "Semaphore.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;
class CommandPool;

class CommandBufferBatch final
{
public:
	CommandBufferBatch(std::shared_ptr<Device> p_Device, std::shared_ptr<CommandPool> p_CommandPool);
	~CommandBufferBatch();
	CommandBufferBatch(const CommandBufferBatch&) = delete;
	CommandBufferBatch& operator=(const CommandBufferBatch&) = delete;

	CommandBuffer& CreateBuffer(const std::string& p_Name, bool p_bOneTimeSubmit);
	CommandBuffer& GetBuffer(const std::string& p_Name);

	void AddWaitSemaphore(const Semaphore* p_Semaphore, VkPipelineStageFlags p_PipelineWaitStages);
	void AddSignalSemaphore(const Semaphore* p_Semaphore);

	VkSubmitInfo GetSubmitInfo() const;

private:
	using Buffers = std::unordered_map<std::string, std::unique_ptr<CommandBuffer>>;
	Buffers m_Buffers;
	std::vector<VkPipelineStageFlags> m_PipelineWaitStages;

	std::shared_ptr<Device> m_Device;
	std::shared_ptr<CommandPool> m_CommandPool;

	std::vector<VkCommandBuffer> m_vkBuffers;
	std::vector<VkSemaphore> m_vkWaitSemaphores;
	std::vector<VkSemaphore> m_vkSignalSemaphores;
};

}
}
}