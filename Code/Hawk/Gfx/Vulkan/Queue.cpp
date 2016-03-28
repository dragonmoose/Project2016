#include "pch.h"
#include "Queue.h"
#include "CommandBufferBatch.h"
#include "Util/Algorithm.h"

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

void Queue::Submit(const CommandBufferBatch* p_Batch) const
{
	VkSubmitInfo l_Info = p_Batch->GetSubmitInfo();
	VK_THROW_IF_NOT_SUCCESS(vkQueueSubmit(m_Handle, 1, &l_Info, VK_NULL_HANDLE), "Failed to submit single batch to queue");
}

void Queue::Submit(const std::vector<CommandBufferBatch*>& p_Batches) const
{
	ASSERT(!p_Batches.empty(), "Empty list of batches");

	std::vector<VkSubmitInfo> l_InfoVec;
	l_InfoVec.resize(p_Batches.size());

	for (uint32 i = 0; i < p_Batches.size(); i++)
	{
		l_InfoVec[i] = p_Batches[i]->GetSubmitInfo();
	}
	VK_THROW_IF_NOT_SUCCESS(vkQueueSubmit(m_Handle, (uint32)l_InfoVec.size(), l_InfoVec.data(), VK_NULL_HANDLE), "Failed to submit " << l_InfoVec.size() << " batches to queue");
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