#include "pch.h"
#include "VlkDeviceCreateInfo.h"

namespace Hawk {
namespace Gfx {

VlkDeviceCreateInfo::VlkDeviceCreateInfo(VkInstance p_Instance, VkSurfaceKHR p_Surface)
: m_Instance(p_Instance)
, m_Surface(p_Surface)
, m_uiDeviceID(0)
, m_bUseDeviceID(false)
, m_bFinalized(false)
{
}

void VlkDeviceCreateInfo::SetDeviceID(uint32_t p_uiDeviceID)
{
	m_uiDeviceID = p_uiDeviceID;
	m_bUseDeviceID = true;
}

void VlkDeviceCreateInfo::AddQueue(VlkQueueType p_Type, uint32_t p_uiIndex, uint32_t p_uiPrio)
{
	VlkQueueRequest l_Request(p_uiIndex, p_uiPrio);
	m_QueueRequestMap[p_Type].push_back(l_Request);
}

const VlkQueueRequestMap_t& VlkDeviceCreateInfo::GetQueueRequestMap() const
{
	return m_QueueRequestMap;
}

bool VlkDeviceCreateInfo::UseDeviceID() const
{
	return m_bUseDeviceID;
}
uint32_t VlkDeviceCreateInfo::GetDeviceID() const
{
	return m_uiDeviceID;
}

VkInstance VlkDeviceCreateInfo::GetInstance() const
{
	return m_Instance;
}

VkSurfaceKHR VlkDeviceCreateInfo::GetSurface() const
{
	return m_Surface;
}

void VlkDeviceCreateInfo::Finalize()
{
	ASSERT(!m_bFinalized, "Finalized has already been called");

	THROW_IF_NOT(m_Instance, "Instance null");
	THROW_IF_NOT(m_Surface, "Surface null");

	for (auto& l_Entry : m_QueueRequestMap)
	{
		VlkQueueRequests_t& l_Requests = l_Entry.second;
		std::sort(l_Requests.begin(), l_Requests.end(), [](const VlkQueueRequest& lhs, const VlkQueueRequest& rhs)
		{
			return lhs.m_uiIndex < rhs.m_uiIndex;
		});

		for (uint32_t i = 0; i < l_Requests.size(); i++)
		{
			THROW_IF_NOT(l_Requests[i].m_uiIndex == i, "Invalid index for queue of type: " << l_Entry.first << " Index=" << l_Requests[i].m_uiIndex << " Expected=" << i);
		}
	}
	m_bFinalized = true;
}

bool VlkDeviceCreateInfo::IsFinalized() const
{
	return m_bFinalized;
}

}
}