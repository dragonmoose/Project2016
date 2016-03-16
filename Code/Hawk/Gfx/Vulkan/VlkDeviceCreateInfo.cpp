#include "pch.h"
#include "VlkDeviceCreateInfo.h"
#include "VlkUtil.h"

namespace Hawk {
namespace Gfx {

VlkDeviceCreateInfo::VlkDeviceCreateInfo(std::shared_ptr<VlkPhysicalDevice> p_PhysicalDevice)
: m_PhysicalDevice(p_PhysicalDevice)
, m_bFinalized(false)
{
}

void VlkDeviceCreateInfo::AddQueue(VlkQueueType p_Type, uint32_t p_uiIndex, uint32_t p_uiPrio)
{
	QueueRequest l_Request(p_uiIndex, p_uiPrio);
	m_QueueRequestMap[p_Type].push_back(l_Request);
}

const VlkDeviceCreateInfo::QueueCreateInfoMap_t& VlkDeviceCreateInfo::GetQueueCreateInfoMap() const
{
	return m_QueueCreateInfoMap;
}

std::shared_ptr<VlkPhysicalDevice> VlkDeviceCreateInfo::GetPhysicalDevice() const
{
	return m_PhysicalDevice;
}

void VlkDeviceCreateInfo::Finalize()
{
	ASSERT(!m_bFinalized, "Finalized has already been called");

	SortAndValidateQueueRequests();
	SetupQueueCreateInfoMap();
	m_bFinalized = true;
}

bool VlkDeviceCreateInfo::IsFinalized() const
{
	return m_bFinalized;
}

void VlkDeviceCreateInfo::SortAndValidateQueueRequests()
{
	auto l_Itr = m_QueueRequestMap.find(VlkQueueType::GraphicsPresentation);
	THROW_IF(l_Itr == m_QueueRequestMap.end(), "No queue added for graphics presentation");
	THROW_IF_NOT(l_Itr->second.size() == 1, "There should be exactly one graphics presentation queue");

	for (auto& l_Entry : m_QueueRequestMap)
	{
		QueueRequests_t& l_Requests = l_Entry.second;
		std::sort(l_Requests.begin(), l_Requests.end(), [](const QueueRequest& lhs, const QueueRequest& rhs)
		{
			return lhs.m_uiIndex < rhs.m_uiIndex;
		});

		for (uint32_t i = 0; i < l_Requests.size(); i++)
		{
			THROW_IF_NOT(l_Requests[i].m_uiIndex == i, "Invalid index for queue of type: " << l_Entry.first << " Index=" << l_Requests[i].m_uiIndex << " Expected=" << i);
		}
	}
}

void VlkDeviceCreateInfo::SetupQueueCreateInfoMap()
{
	VlkPhysicalDevice::QueueFamilyProperties_t l_PropsVec;
	VlkPhysicalDevice::GetQueueFamilyProperties(m_PhysicalDevice->GetHandle(), l_PropsVec);

	using FamilyNumQueuesLeft_t = std::unordered_map<uint32_t, uint32_t>;
	FamilyNumQueuesLeft_t l_NumQueuesLeft;
	for (uint32_t l_uiFamilyIndex = 0; l_uiFamilyIndex < l_PropsVec.size(); l_uiFamilyIndex++)
	{
		l_NumQueuesLeft[l_uiFamilyIndex] = l_PropsVec[l_uiFamilyIndex].queueCount;
	}

	for (const auto& l_Entry : m_QueueRequestMap)
	{
		VlkQueueType l_Type = l_Entry.first;
		for (const auto& l_Request : l_Entry.second)
		{
			bool l_bAdded = false;
			for (uint32_t l_uiFamilyIndex = 0; l_uiFamilyIndex < l_PropsVec.size(); l_uiFamilyIndex++)
			{
				VkQueueFamilyProperties& l_Props = l_PropsVec[l_uiFamilyIndex];
				uint32_t& l_uiNumQueuesLeft = l_NumQueuesLeft[l_uiFamilyIndex];
				VkQueueFlags l_Flag = VlkUtil::QueueTypeToFlag(l_Type);
				bool l_bSupportsType = (l_Flag & l_Props.queueFlags) == l_Flag;
				bool l_bQueuesLeft = l_uiNumQueuesLeft != 0;
				if (l_bSupportsType && l_bQueuesLeft)
				{
					uint32_t l_uiQueueIndex = l_Props.queueCount - l_uiNumQueuesLeft;
					m_QueueCreateInfoMap[l_Type].emplace_back(l_uiFamilyIndex, l_Request.m_uiIndex, l_uiQueueIndex, l_Request.m_uiPrio);
					l_uiNumQueuesLeft--;
					l_bAdded = true;
					break;
				}
			}
			THROW_IF_NOT(l_bAdded, "Failed to add requested queue for creation. QueueType=" << l_Type << " Index=" << l_Request.m_uiIndex);
		}
	}
}

}
}