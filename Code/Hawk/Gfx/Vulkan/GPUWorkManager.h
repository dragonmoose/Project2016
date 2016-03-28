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

	// Buffers can be reset individually and will also be reset when Begin() is called.
	// If false it is an error to call Begin() without first calling ResetPool() on the GPUWorkManager.
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

	void ResetPool(bool p_bReleaseResources);
	void WaitUntilIdle();

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