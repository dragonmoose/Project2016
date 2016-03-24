#include "pch.h"
#include "Queue.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

Queue::Queue(VkQueue p_Queue, QueueType p_Type, uint32 p_uiFamilyIndex)
: m_Handle(p_Queue)
, m_Type(p_Type)
, m_uiFamilyIndex(p_uiFamilyIndex)
{
	THROW_IF_NOT(m_Handle, "VkQueue handle null");
}

Queue::~Queue()
{
	LOG("Queue of type " << m_Type << " destroyed", "vulkan", Debug);
}

VkQueue Queue::GetHandle() const
{
	return m_Handle;
}

uint32 Queue::GetFamilyIndex() const
{
	return m_uiFamilyIndex;
}

}
}
}