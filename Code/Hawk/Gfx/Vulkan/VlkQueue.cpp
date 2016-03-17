#include "pch.h"
#include "VlkQueue.h"

namespace Hawk {
namespace Gfx {

VlkQueue::VlkQueue(VkQueue p_Queue, VlkQueueType p_Type, uint32 p_uiFamilyIndex)
: m_Queue(p_Queue)
, m_Type(p_Type)
, m_uiFamilyIndex(p_uiFamilyIndex)
{
	THROW_IF_NOT(m_Queue, "VkQueue handle null");
}

VlkQueue::~VlkQueue()
{
	LOG("Queue of type " << m_Type << " destroyed", "vulkan", Debug);
}

VkQueue VlkQueue::GetHandle() const
{
	return m_Queue;
}

uint32 VlkQueue::GetFamilyIndex() const
{
	return m_uiFamilyIndex;
}

}
}