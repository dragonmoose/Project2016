#include "pch.h"
#include "Queue.h"
#include "CommandBufferBatch.h"

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

void Queue::AddBatch(std::shared_ptr<CommandBufferBatch> p_Batch)
{
	m_Batches.push_back(p_Batch);
}

void Queue::Submit()
{
	std::vector<VkSubmitInfo> l_InfoVec;
	l_InfoVec.resize(m_Batches.size());

	for (uint32 i = 0; i < l_InfoVec.size(); i++)
	{
		l_InfoVec[i] = m_Batches[i]->GetSubmitInfo();
	}
	VK_THROW_IF_NOT_SUCCESS(vkQueueSubmit(m_Handle, l_InfoVec.size(), l_InfoVec.data(), VK_NULL_HANDLE), "Failed to submit queue");
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