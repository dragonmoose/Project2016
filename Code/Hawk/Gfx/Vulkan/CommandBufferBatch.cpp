#include "pch.h"
#include "CommandBufferBatch.h"
#include "CommandBuffer.h"
#include "Semaphore.h"
#include "Util/Algorithm.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

CommandBufferBatch::CommandBufferBatch(const std::string& p_Name, std::shared_ptr<Device> p_Device, std::shared_ptr<CommandPool> p_CommandPool)
: m_Name(p_Name)
, m_Device(p_Device)
, m_CommandPool(p_CommandPool)
{
	m_Buffers.reserve(3);
	m_PipelineWaitStages.reserve(3);
	m_vkWaitSemaphores.reserve(3);
	m_vkSignalSemaphores.reserve(3);
}

CommandBufferBatch::~CommandBufferBatch()
{
}

CommandBuffer* CommandBufferBatch::CreateBuffer(const std::string& p_Name, bool p_bOneTimeSubmit)
{
	ASSERT(hwk::find_first(m_Buffers, p_Name) == m_Buffers.cend(), "Batch already contains a command buffer with name=" << p_Name);
	std::unique_ptr<CommandBuffer> l_Buffer = std::make_unique<CommandBuffer>(m_Device, m_CommandPool, p_bOneTimeSubmit);
	CommandBuffer* l_Ptr = l_Buffer.get();
	THROW_IF_NOT(m_Buffers.insert(Buffers::value_type(p_Name, std::move(l_Buffer))).second, "Failed to insert CommandBuffer");
	m_vkBuffers.push_back(l_Ptr->GetHandle());
	return l_Ptr;

}

CommandBuffer* CommandBufferBatch::GetBuffer(const std::string& p_Name)
{
	auto l_Itr = hwk::find_first(m_Buffers, p_Name);
	ASSERT(l_Itr != m_Buffers.cend(), "Failed to find buffer with name=" << p_Name);
	return l_Itr->second.get();
}

const std::string& CommandBufferBatch::GetName() const
{
	return m_Name;
}

void CommandBufferBatch::AddWaitSemaphore(const Semaphore* p_Semaphore, VkPipelineStageFlags p_PipelineWaitStages)
{
	m_PipelineWaitStages.push_back(p_PipelineWaitStages);
	m_vkWaitSemaphores.push_back(p_Semaphore->GetHandle());
}

void CommandBufferBatch::AddSignalSemaphore(const Semaphore* p_Semaphore)
{
	m_vkSignalSemaphores.push_back(p_Semaphore->GetHandle());
}

VkSubmitInfo CommandBufferBatch::GetSubmitInfo() const
{
	VkSubmitInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	l_Info.commandBufferCount = (uint32)m_vkBuffers.size();
	l_Info.pCommandBuffers = m_vkBuffers.data();
	l_Info.waitSemaphoreCount = (uint32)m_vkWaitSemaphores.size();
	l_Info.pWaitSemaphores = m_vkWaitSemaphores.data();
	l_Info.pWaitDstStageMask = m_PipelineWaitStages.data();
	l_Info.signalSemaphoreCount = (uint32)m_vkSignalSemaphores.size();
	l_Info.pSignalSemaphores = m_vkSignalSemaphores.data();
	return l_Info;
}

}
}
}