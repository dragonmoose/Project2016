#include "pch.h"
#include "Queue.h"
#include "CommandBuffer.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

Queue::Queue(VkQueue p_Queue, QueueType p_Type, uint32 p_uiFamilyIndex)
: m_Handle(p_Queue)
, m_Type(p_Type)
, m_uiFamilyIndex(p_uiFamilyIndex)
{
	THROW_IF_NOT(m_Handle, "VkQueue handle null");
	Clear();
}

Queue::~Queue()
{
	LOG("Queue of type " << m_Type << " destroyed", "vulkan", Debug);
}

void Queue::Add(const CommandBuffer* p_CommandBuffer)
{
	m_PendingCommandBuffers.push_back(p_CommandBuffer->GetHandle());
}

void Queue::Submit()
{
	ASSERT(!m_PendingCommandBuffers.empty(), "Nothing to submit");
	m_SubmitInfo.commandBufferCount = m_PendingCommandBuffers.size();
	m_SubmitInfo.pCommandBuffers = m_PendingCommandBuffers.data();
	VK_THROW_IF_NOT_SUCCESS(vkQueueSubmit(m_Handle, 1, &m_SubmitInfo, VK_NULL_HANDLE), "Failed to submit queue");
	Clear();
}

void Queue::Clear()
{
	m_PendingCommandBuffers.clear();

	m_SubmitInfo = {};
	m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	// TODO: Synchronization
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