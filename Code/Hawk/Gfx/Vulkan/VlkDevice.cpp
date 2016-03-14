#include "pch.h"
#include "VlkDevice.h"
#include "VlkPhysicalDevice.h"
#include "VlkUtil.h"
#include "Util/Algorithm.h"
#include "System/StreamOperators.h"

namespace Hawk {
namespace Gfx {

namespace
{
	std::vector<const char*> n_EnabledLayers = {};
	std::vector<const char*> n_EnabledExtensions = {};

#ifdef HAWK_DEBUG
	std::vector<const char*> n_EnabledDebugLayers = {
		"VK_LAYER_LUNARG_api_dump",
		"VK_LAYER_LUNARG_device_limits",
	};

	std::vector<const char*> n_EnabledDebugExtensions = {};
#endif
}

VlkDevice::VlkDevice(const VlkDeviceCreateInfo& p_CreateInfo)
: m_Instance(p_CreateInfo.GetInstance())
, m_Surface(p_CreateInfo.GetSurface())
, m_PhysicalDevice(p_CreateInfo.GetPhysicalDevice())
, m_Device(VK_NULL_HANDLE)
, m_QueueRequestMap(p_CreateInfo.GetQueueRequestMap())
{
	ASSERT(p_CreateInfo.IsFinalized(), "CreateInfo not finalized");
	Initialize();
	Validate();
}

VlkDevice::~VlkDevice()
{
	ASSERT(m_Device, "Device null");
	VkResult l_Result = vkDeviceWaitIdle(m_Device);
	LOG_IF(l_Result != VK_SUCCESS, "Failed to wait for device to become idle. Error: " << VlkSystem::ResultToString(l_Result), "vulkan", Error);

	vkDestroyDevice(m_Device, nullptr); // Will also destroy all queues created on device
	LOG("Vulkan device destroyed", "vulkan", Debug);
}

void VlkDevice::WaitUntilIdle()
{
	VkResult l_Result = vkDeviceWaitIdle(m_Device);
	if (l_Result == VK_ERROR_DEVICE_LOST)
	{
		OnDeviceLost();
	}
	else
	{
		VK_THROW_IF_ERR(l_Result, "Failed to wait for device to become idle");
	}
}

VkQueue VlkDevice::GetQueue(VlkQueueType p_Type, uint32_t p_uiIndex) const
{
	const auto& l_Itr = m_Queues.find(p_Type);
	THROW_IF(l_Itr == m_Queues.end(), "Queues of type " << p_Type << " not available");
	THROW_IF_NOT(p_uiIndex < l_Itr->second.size(), "Invalid queue index " << p_uiIndex << " for queue type " << p_Type);
	return l_Itr->second[p_uiIndex];
}

void VlkDevice::GetLayersToCreate(std::vector<const char*>& p_Layers)
{
	std::copy(n_EnabledLayers.begin(), n_EnabledLayers.end(), std::back_inserter(p_Layers));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugLayers.begin(), n_EnabledDebugLayers.end(), std::back_inserter(p_Layers));
#endif

	for (const auto& l_Layer : p_Layers)
	{
		THROW_IF_NOT(VlkPhysicalDevice::IsLayerAvailable(m_PhysicalDevice->GetHandle(), l_Layer), "Device layer not available. Name=" << l_Layer);
	}
}

void VlkDevice::GetExtensionsToCreate(std::vector<const char*>& p_Extensions)
{
	std::copy(n_EnabledExtensions.begin(), n_EnabledExtensions.end(), std::back_inserter(p_Extensions));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugExtensions.begin(), n_EnabledDebugExtensions.end(), std::back_inserter(p_Extensions));
#endif

	for (const auto& l_Extension : p_Extensions)
	{
		THROW_IF_NOT(VlkPhysicalDevice::IsExtensionAvailable(m_PhysicalDevice->GetHandle(), l_Extension), "Global device extension not available. Name=" << l_Extension);
	}
}

void VlkDevice::CreateDevice(const QueueFamilyCreateInfos_t& p_QueueFamilyCreateInfos)
{
	ValidateQueueFamilyCreateInfos(p_QueueFamilyCreateInfos);
	VkPhysicalDeviceFeatures l_Features = {};
	GetFeatures(l_Features);

	std::vector<const char*> l_EnabledLayers = {};
	GetLayersToCreate(l_EnabledLayers);

	std::vector<const char*> l_EnabledExtensions = {};
	GetExtensionsToCreate(l_EnabledExtensions);

	VkDeviceCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	l_Info.queueCreateInfoCount = p_QueueFamilyCreateInfos.size();
	l_Info.pQueueCreateInfos = p_QueueFamilyCreateInfos.data();
	l_Info.enabledLayerCount = l_EnabledLayers.size();
	l_Info.ppEnabledLayerNames = l_EnabledLayers.data();
	l_Info.enabledExtensionCount = l_EnabledExtensions.size();
	l_Info.ppEnabledExtensionNames = l_EnabledExtensions.data();
	l_Info.pEnabledFeatures = &l_Features;
	VK_THROW_IF_NOT_SUCCESS(vkCreateDevice(m_PhysicalDevice->GetHandle(), &l_Info, nullptr, &m_Device), "Failed to create device");
}

void VlkDevice::ExtractQueues(const QueueCreateInfoMap_t& p_Map)
{
	ASSERT(m_Device, "Device not created");
	for (const auto& l_Entry : p_Map)
	{
		VlkQueueType l_Type = l_Entry.first;
		for (const auto& l_Info : l_Entry.second)
		{
			VkQueue l_Queue = VK_NULL_HANDLE;
			vkGetDeviceQueue(m_Device, l_Info.m_uiFamilyIndex, l_Info.m_uiQueueIndex, &l_Queue);
			THROW_IF_NOT(l_Queue, "Failed to get handle to queue. QueueType=" << l_Type << " FamilyIndex=" << l_Info.m_uiFamilyIndex << " QueueIndex=" << l_Info.m_uiQueueIndex);
			
			m_Queues[l_Type].push_back(l_Queue);
			LOG("Extracted queue of type from device: " << l_Type << " TypeIndex=" << l_Info.m_uiTypeIndex << " QueueIndex=" << l_Info.m_uiQueueIndex, "vulkan", Debug);
		}
	}
}

void VlkDevice::Initialize()
{
	THROW_IF_NOT(m_PhysicalDevice, "PhysicalDevice is null");

	QueueCreateInfoMap_t l_QueueCreateInfoMap;
	GetQueueCreateInfoMap(l_QueueCreateInfoMap);

	CheckWSISupport(l_QueueCreateInfoMap);

	QueueFamilyCreateInfos_t l_QueueFamilyCreateInfos;
	GetQueueFamilyCreateInfos(l_QueueCreateInfoMap, l_QueueFamilyCreateInfos);

	CreateDevice(l_QueueFamilyCreateInfos);
	ExtractQueues(l_QueueCreateInfoMap);
}

void VlkDevice::Validate()
{
	THROW_IF_NOT(m_PhysicalDevice, "Physical device null");
	THROW_IF_NOT(m_Device, "Device null");
	for (const auto& l_Entry : m_QueueRequestMap)
	{
		auto l_Itr = m_Queues.find(l_Entry.first);
		THROW_IF(l_Itr == m_Queues.end(), "Queue type should be available: " << l_Entry.first);
		THROW_IF_NOT(l_Itr->second.size() == l_Entry.second.size(), "Queue count mismatch for queue type: " << l_Entry.first);
	}
}

void VlkDevice::ValidateQueueFamilyCreateInfos(const QueueFamilyCreateInfos_t& p_QueueFamilyCreateInfos)
{
	THROW_IF(p_QueueFamilyCreateInfos.empty(), "At least one queue needs to be specified when creating device");
	for (uint32_t i = 0; i < p_QueueFamilyCreateInfos.size(); i++)
	{
		for (uint32_t j = i + 1; j < p_QueueFamilyCreateInfos.size(); j++)
		{
			THROW_IF(p_QueueFamilyCreateInfos[i].queueFamilyIndex == p_QueueFamilyCreateInfos[j].queueFamilyIndex, "Duplicate queue family index when creating device");
		}
	}
}

void VlkDevice::OnDeviceLost()
{
	try
	{
		// TODO: Recreate device
	}
	catch (Exception&)
	{
		//FATAL("Failed to recrete device after lost state. Msg: " << e.what(), "vulkan");
	}
}

void VlkDevice::GetQueueFamilyCreateInfos(const QueueCreateInfoMap_t& p_QueueCreateMap, QueueFamilyCreateInfos_t& p_FamilyCreateInfos)
{
	struct FamilyInfo
	{
		uint32_t m_uiCount;
		std::unordered_map<uint32_t, uint32_t> m_Prios;
	};
	using QueueFamilyMap_t = std::unordered_map<uint32_t, FamilyInfo>;
	QueueFamilyMap_t l_Families;

	for (const auto& l_Entry : p_QueueCreateMap)
	{
		for (const auto& l_Info : l_Entry.second)
		{
			FamilyInfo& l_FamilyInfo = l_Families[l_Info.m_uiFamilyIndex];
			l_FamilyInfo.m_uiCount++;
			l_FamilyInfo.m_Prios[l_Info.m_uiQueueIndex] = l_Info.m_uiPrio;
		}
	}

	for (const auto& l_Family : l_Families)
	{
		const FamilyInfo& l_FamilyInfo = l_Family.second;
		VkDeviceQueueCreateInfo l_CreateInfo;
		l_CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		l_CreateInfo.queueFamilyIndex = l_Family.first;
		l_CreateInfo.queueCount = l_FamilyInfo.m_uiCount;

		std::vector<float> l_NormalizedPrios;
		l_NormalizedPrios.resize(l_FamilyInfo.m_Prios.size());

		float l_fSum = 0.0f;
		for (const auto& l_Prio : l_FamilyInfo.m_Prios)
		{
			l_fSum += l_Prio.second;
		}

		for (uint32_t i = 0; i < l_FamilyInfo.m_Prios.size(); i++)
		{
			l_NormalizedPrios[i] = l_FamilyInfo.m_Prios.at(i) / l_fSum;
		}
		l_CreateInfo.pQueuePriorities = l_NormalizedPrios.data();
		p_FamilyCreateInfos.push_back(l_CreateInfo);
		LOG("Queue family registered for creation. FamilyIndex=" << l_CreateInfo.queueFamilyIndex << " QueueCount=" << l_CreateInfo.queueCount << " Prios: " << l_NormalizedPrios, "vulkan", Debug)
	}
}

void VlkDevice::GetQueueCreateInfoMap(QueueCreateInfoMap_t& p_QueueCreateMap)
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
					p_QueueCreateMap[l_Type].emplace_back(l_uiFamilyIndex, l_Request.m_uiIndex, l_uiQueueIndex, l_Request.m_uiPrio);
					l_uiNumQueuesLeft--;
					l_bAdded = true;
					break;
				}
			}
			THROW_IF_NOT(l_bAdded, "Failed to add requested queue for creation. QueueType=" << l_Type << " Index=" << l_Request.m_uiIndex);
		}
	}
}

void VlkDevice::GetFeatures(VkPhysicalDeviceFeatures& /*p_Features*/)
{
}

void VlkDevice::CheckWSISupport(const QueueCreateInfoMap_t& p_QueueCreateMap)
{
	ASSERT(m_PhysicalDevice, "PhysicalDevice null");
	ASSERT(m_Surface, "Surface null");

	auto l_Itr = p_QueueCreateMap.find(VlkQueueType::Graphics);
	ASSERT(l_Itr != p_QueueCreateMap.end(), "No queues requiring graphics found when checking for WSI support");
	std::vector<uint32_t> l_FamilyIndices;
	for (const auto& l_Info : l_Itr->second)
	{
		l_FamilyIndices.push_back(l_Info.m_uiFamilyIndex);
	}
	std::sort(l_FamilyIndices.begin(), l_FamilyIndices.end());
	hwk::unique(l_FamilyIndices);

	for (uint32_t l_uiFamilyIndex : l_FamilyIndices)
	{
		VkBool32 l_bResult = {};
		VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice->GetHandle(), l_uiFamilyIndex, m_Surface->GetHandle(), &l_bResult), "Failed to check support for WSI");
		THROW_IF_NOT(l_bResult, "WSI-check failed: Queue family " << l_uiFamilyIndex << " does not support the specified surface");

		l_bResult = vkGetPhysicalDeviceWin32PresentationSupportKHR(m_PhysicalDevice->GetHandle(), l_uiFamilyIndex);
		THROW_IF_NOT(l_bResult, "WSI-check failed: Queue family " << l_uiFamilyIndex << " does not support presentation on Windows Desktop");
	}
}

}
}