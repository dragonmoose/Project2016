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
{
}

GPUWorkManager::~GPUWorkManager()
{
}

CommandBufferBatch* GPUWorkManager::CreateBatch(const std::string& p_Name, bool p_bEnqueue)
{
	ASSERT(hwk::find_if(m_Batches, [p_Name](const std::unique_ptr<CommandBufferBatch>& p_Batch) { return StringUtil::AreEqual(p_Name, p_Batch->GetName()); }) == m_Batches.end(), "Already added batch with name=" << p_Name);
	std::unique_ptr<CommandBufferBatch> l_Batch = std::make_unique<CommandBufferBatch>(p_Name, m_Device, m_Pool);
	CommandBufferBatch* l_Ptr = l_Batch.get();
	m_Batches.push_back(std::move(l_Batch));

	if (p_bEnqueue)
	{
		m_QueuedBatches.push_back(l_Ptr);
	}
	return l_Ptr;
	
}

CommandBufferBatch* GPUWorkManager::GetBatch(const std::string& p_Name) const
{
	auto l_Itr = hwk::find_if(m_Batches, [p_Name](const std::unique_ptr<CommandBufferBatch>& p_Batch) { return StringUtil::AreEqual(p_Batch->GetName(), p_Name, true); });
	ASSERT(l_Itr != m_Batches.end(), "Failed to find batch by name=" << p_Name);
	return l_Itr->get();
}

void GPUWorkManager::SubmitQueued() const
{
	m_Queue->Submit(m_QueuedBatches);
	VkResult l_Res = vkQueueWaitIdle(m_Queue->GetHandle());
}

void GPUWorkManager::Submit(CommandBufferBatch* p_Batch) const
{
	m_Queue->Submit(p_Batch);
}


}
}
}