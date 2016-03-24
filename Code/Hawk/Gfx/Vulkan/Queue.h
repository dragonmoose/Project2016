#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class CommandBuffer;

class Queue final
{
public:
	Queue(VkQueue p_Queue, QueueType p_Type, uint32 p_uiFamilyIndex);
	~Queue();
	Queue(const Queue&) = delete;
	Queue& operator=(const Queue&) = delete;

	void Add(const CommandBuffer* p_CommandBuffer);
	void Submit();
	void Clear();

	VkQueue GetHandle() const;
	uint32 GetFamilyIndex() const;

private:
	VkQueue m_Handle;
	QueueType m_Type;
	uint32 m_uiFamilyIndex;
	std::vector<VkCommandBuffer> m_PendingCommandBuffers;
	VkSubmitInfo m_SubmitInfo;
};

}
}
}