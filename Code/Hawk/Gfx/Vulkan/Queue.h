#pragma once
#include "System.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class CommandBufferBatch;

class Queue final
{
public:
	Queue(VkQueue p_Queue, QueueType p_Type, uint32 p_uiFamilyIndex);
	~Queue();
	Queue(const Queue&) = delete;
	Queue& operator=(const Queue&) = delete;

	void AddBatch(std::shared_ptr<CommandBufferBatch> p_Batch);
	void Submit();

	VkQueue GetHandle() const;
	uint32 GetFamilyIndex() const;

private:
	VkQueue m_Handle;
	QueueType m_Type;
	uint32 m_uiFamilyIndex;
	std::vector<std::shared_ptr<CommandBufferBatch>> m_Batches;
};

}
}
}