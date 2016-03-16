#pragma once
#include "VlkSystem.h"

namespace Hawk {
namespace Gfx {

class VlkQueue final
{
public:
	VlkQueue(VkQueue p_Queue, VlkQueueType p_Type, uint32_t p_uiFamilyIndex);
	~VlkQueue();
	VlkQueue(const VlkQueue&) = delete;
	VlkQueue& operator=(const VlkQueue&) = delete;

	VkQueue GetHandle() const;
	uint32_t GetFamilyIndex() const;

private:
	VkQueue m_Queue;
	VlkQueueType m_Type;
	uint32_t m_uiFamilyIndex;	
};

}
}