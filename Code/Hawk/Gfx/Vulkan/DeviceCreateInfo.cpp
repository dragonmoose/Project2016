#include "pch.h"
#include "DeviceCreateInfo.h"
#include "Util.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

DeviceCreateInfo::DeviceCreateInfo(std::shared_ptr<PhysicalDevice> p_PhysicalDevice)
: m_PhysicalDevice(p_PhysicalDevice)
, m_bFinalized(false)
{
}

void DeviceCreateInfo::AddQueue(QueueType p_Type, uint32 p_uiIndex, uint32 p_uiPrio)
{
	QueueRequest l_Request(p_uiIndex, p_uiPrio);
	m_QueueRequestMap[p_Type].push_back(l_Request);
}

const DeviceCreateInfo::QueueCreateInfoMap& DeviceCreateInfo::GetQueueCreateInfoMap() const
{
	return m_QueueCreateInfoMap;
}

std::shared_ptr<PhysicalDevice> DeviceCreateInfo::GetPhysicalDevice() const
{
	return m_PhysicalDevice;
}

void DeviceCreateInfo::Finalize()
{
	ASSERT(!m_bFinalized, "Finalized has already been called");

	SortAndValidateQueueRequests();
	SetupQueueCreateInfoMap();
	m_bFinalized = true;
}

bool DeviceCreateInfo::IsFinalized() const
{
	return m_bFinalized;
}

void DeviceCreateInfo::SortAndValidateQueueRequests()
{
	auto l_Itr = m_QueueRequestMap.find(QueueType::GraphicsPresentation);
	THROW_IF(l_Itr == m_QueueRequestMap.end(), "No queue added for graphics presentation");
	THROW_IF_NOT(l_Itr->second.size() == 1, "There should be exactly one graphics presentation queue");

	for (auto& l_Entry : m_QueueRequestMap)
	{
		QueueRequests& l_Requests = l_Entry.second;
		std::sort(l_Requests.begin(), l_Requests.end(), [](const QueueRequest& lhs, const QueueRequest& rhs)
		{
			return lhs.m_uiIndex < rhs.m_uiIndex;
		});

		for (uint32 i = 0; i < l_Requests.size(); i++)
		{
			THROW_IF_NOT(l_Requests[i].m_uiIndex == i, "Invalid index for queue of type: " << l_Entry.first << " Index=" << l_Requests[i].m_uiIndex << " Expected=" << i);
		}
	}
}

void DeviceCreateInfo::SetupQueueCreateInfoMap()
{
	PhysicalDevice::QueueFamilyProperties l_PropsVec;
	PhysicalDevice::GetQueueFamilyProperties(m_PhysicalDevice->GetHandle(), l_PropsVec);

	using FamilyNumQueuesLeft = std::unordered_map<uint32, uint32>;
	FamilyNumQueuesLeft l_NumQueuesLeft;
	for (uint32 l_uiFamilyIndex = 0; l_uiFamilyIndex < l_PropsVec.size(); l_uiFamilyIndex++)
	{
		l_NumQueuesLeft[l_uiFamilyIndex] = l_PropsVec[l_uiFamilyIndex].queueCount;
	}

	for (const auto& l_Entry : m_QueueRequestMap)
	{
		QueueType l_Type = l_Entry.first;
		for (const auto& l_Request : l_Entry.second)
		{
			bool l_bAdded = false;
			for (uint32 l_uiFamilyIndex = 0; l_uiFamilyIndex < l_PropsVec.size(); l_uiFamilyIndex++)
			{
				VkQueueFamilyProperties& l_Props = l_PropsVec[l_uiFamilyIndex];
				uint32& l_uiNumQueuesLeft = l_NumQueuesLeft[l_uiFamilyIndex];
				VkQueueFlags l_Flag = Util::QueueTypeToFlag(l_Type);
				bool l_bSupportsType = (l_Flag & l_Props.queueFlags) == l_Flag;
				bool l_bQueuesLeft = l_uiNumQueuesLeft != 0;
				if (l_bSupportsType && l_bQueuesLeft)
				{
					uint32 l_uiQueueIndex = l_Props.queueCount - l_uiNumQueuesLeft;
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
}