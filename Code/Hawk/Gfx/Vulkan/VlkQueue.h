#pragma once
#include "VlkSystem.h"

namespace Hawk {
namespace Gfx {

class VlkQueue final
{
public:
	VlkQueue(VkQueue p_Queue, VlkQueueType p_Type, uint32 p_uiFamilyIndex);
	~VlkQueue();
	VlkQueue(const VlkQueue&) = delete;
	VlkQueue& operator=(const VlkQueue&) = delete;

	VkQueue GetHandle() const;
	uint32 GetFamilyIndex() const;

private:
	VkQueue m_Queue;
	VlkQueueType m_Type;
	uint32 m_uiFamilyIndex;	
};

}
}