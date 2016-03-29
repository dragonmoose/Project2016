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

	bool m_bAllowIndividualBufferReset; // Buffers can be reset individually and will also be reset when Begin() is called. If false it is an error to call Begin() without first calling ResetPool() on the GPUWorkManager.
	bool m_bShortLifetimeObjects;
	uint32 m_uiNumCopies;
};

// TODO: Improve this interface (how to handle multiple copies better?)
class GPUWorkManager final
{
public:
	GPUWorkManager(const GPUWorkManagerInitInfo& p_InitInfo);
	~GPUWorkManager();
	GPUWorkManager(const CommandBuffer&) = delete;
	GPUWorkManager& operator=(const CommandBuffer&) = delete;

	void CreateBatch(const std::string& p_Name, bool p_bEnqueue);
	CommandBufferBatch* GetBatch(const std::string& p_Name, uint32 p_uiCopy) const;

	void SubmitQueued(uint32 p_uiCopy) const;
	void Submit(CommandBufferBatch* p_Batch) const;
	void Submit(std::vector<CommandBufferBatch*>& p_Batches) const;

	void ResetPool(bool p_bReleaseResources);
	void WaitUntilIdle();

private:
	std::shared_ptr<Device> m_Device;
	std::shared_ptr<CommandPool> m_Pool;
	std::shared_ptr<Queue> m_Queue;

	using BatchCopies = std::vector<std::unique_ptr<CommandBufferBatch>>;
	using Batches = std::unordered_map<std::string, BatchCopies>;
	Batches m_Batches;

	using EnqueuedBatches = std::vector<std::vector<CommandBufferBatch*>>;
	EnqueuedBatches m_QueuedBatches;

	const uint32 m_uiNumCopies;
};

}
}
}