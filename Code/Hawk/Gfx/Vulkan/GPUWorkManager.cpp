#include "pch.h"
#include "GPUWorkManager.h"
#include "Queue.h"
#include "CommandPool.h"
#include "Device.h"
#include "Util/Algorithm.h"
#include "Util/StringUtil.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

GPUWorkManager::GPUWorkManager(const GPUWorkManagerInitInfo& p_InitInfo)
: m_Device(p_InitInfo.m_Device)
, m_Queue(p_InitInfo.m_Queue)
, m_Pool(std::make_unique<CommandPool>(p_InitInfo.m_Device, p_InitInfo.m_Queue->GetFamilyIndex(), p_InitInfo.m_bAllowIndividualBufferReset, p_InitInfo.m_bShortLifetimeObjects))
, m_uiNumCopies(p_InitInfo.m_uiNumCopies)
{ 
	m_QueuedBatches.resize(m_uiNumCopies);
}

GPUWorkManager::~GPUWorkManager()
{
}

void GPUWorkManager::CreateBatch(const std::string& p_Name, bool p_bEnqueue)
{
	ASSERT(hwk::find_first(m_Batches, p_Name) == m_Batches.end(), "Already added batch with name = " << p_Name);
	auto& l_Copies = m_Batches[p_Name];
	l_Copies.resize(m_uiNumCopies);

	for (uint32 i = 0; i < m_uiNumCopies; i++)
	{
		l_Copies[i] = std::make_unique<CommandBufferBatch>(p_Name, m_Device, m_Pool);
		if (p_bEnqueue)
		{
			m_QueuedBatches[i].push_back(l_Copies[i].get());
		}
	}
}

CommandBufferBatch* GPUWorkManager::GetBatch(const std::string& p_Name, uint32 p_uiCopy) const
{
	const auto& l_Itr = hwk::find_first(m_Batches, p_Name);
	ASSERT(l_Itr != m_Batches.end(), "Failed to find batch by name=" << p_Name);

	const BatchCopies& l_Copies = l_Itr->second;
	ASSERT(p_uiCopy < l_Copies.size(), "Copy index out of bounds. Copy=" << p_uiCopy << " Size=" << l_Copies.size());

	return l_Copies[p_uiCopy].get();
}

void GPUWorkManager::SubmitQueued(uint32 p_uiCopy) const
{
	m_Queue->Submit(m_QueuedBatches[p_uiCopy]);
}

void GPUWorkManager::Submit(CommandBufferBatch* p_Batch) const
{
	m_Queue->Submit(p_Batch);
}

void GPUWorkManager::Submit(std::vector<CommandBufferBatch*>& p_Batches) const
{
	m_Queue->Submit(p_Batches);
}

void GPUWorkManager::ResetPool(bool p_bReleaseResources)
{
	m_Pool->Reset(p_bReleaseResources);
}

void GPUWorkManager::WaitUntilIdle()
{
	VkResult l_Result = vkQueueWaitIdle(m_Queue->GetHandle());
	if (l_Result == VK_ERROR_DEVICE_LOST)
	{
		// TODO: Respond to device lost
	}
	else
	{
		VK_THROW_IF_ERR(l_Result, "Failed to wait for device to become idle");
	}
}

}
}
}