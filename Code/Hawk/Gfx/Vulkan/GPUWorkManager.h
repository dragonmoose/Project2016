#pragma once
#include "System.h"
#include "CommandPool.h"
#include "CommandBufferBatch.h"
#include <memory>
#include <vector>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Queue;
class CommandPool;
class Device;

struct GPUWorkManagerInitInfo
{
	std::shared_ptr<Device> m_Device;
	std::shared_ptr<Queue> m_Queue;
	bool m_bAllowIndividualBufferReset;
	bool m_bShortLifetimeObjects;
};

class GPUWorkManager final
{
public:
	GPUWorkManager(const GPUWorkManagerInitInfo& p_InitInfo);
	~GPUWorkManager();
	GPUWorkManager(const CommandBuffer&) = delete;
	GPUWorkManager& operator=(const CommandBuffer&) = delete;

	CommandBufferBatch* CreateBatch(const std::string& p_Name, bool p_bEnqueue);
	CommandBufferBatch* GetBatch(const std::string& p_Name) const;

	void SubmitQueued() const;
	void Submit(CommandBufferBatch* p_Batch) const;

private:
	std::shared_ptr<Device> m_Device;
	std::shared_ptr<CommandPool> m_Pool;
	std::shared_ptr<Queue> m_Queue;

	using Batches = std::vector<std::unique_ptr<CommandBufferBatch>>;
	Batches m_Batches;

	using EnqueuedBatches = std::vector<CommandBufferBatch*>;
	EnqueuedBatches m_QueuedBatches;
};

}
}
}