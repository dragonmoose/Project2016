#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Queue final
{
public:
	Queue(VkQueue p_Queue, QueueType p_Type, uint32 p_uiFamilyIndex);
	~Queue();
	Queue(const Queue&) = delete;
	Queue& operator=(const Queue&) = delete;

	VkQueue GetHandle() const;
	uint32 GetFamilyIndex() const;

private:
	VkQueue m_Queue;
	QueueType m_Type;
	uint32 m_uiFamilyIndex;	
};

}
}
}