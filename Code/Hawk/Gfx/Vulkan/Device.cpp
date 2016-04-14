#include "pch.h"
#include "Device.h"
#include "PhysicalDevice.h"
#include "Util.h"
#include "Util/Algorithm.h"
#include "System/StreamOperators.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

namespace
{
	std::vector<const char*> n_EnabledLayers = {};
	std::vector<const char*> n_EnabledExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		"VK_NV_glsl_shader",
	};

#ifdef HAWK_DEBUG
	std::vector<const char*> n_EnabledDebugLayers = {
		/*"VK_LAYER_LUNARG_api_dump",
		"VK_LAYER_LUNARG_core_validation",
		"VK_LAYER_LUNARG_device_limits",
		"VK_LAYER_LUNARG_image",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_parameter_validation",
		"VK_LAYER_LUNARG_screenshot",
		"VK_LAYER_LUNARG_swapchain",
		"VK_LAYER_GOOGLE_threading",
		"VK_LAYER_GOOGLE_unique_objects",
		"VK_LAYER_LUNARG_vktrace",
		"VK_LAYER_LUNARG_standard_validation",
		"VK_LAYER_RENDERDOC_Capture",*/
	};

	std::vector<const char*> n_EnabledDebugExtensions = {};
#endif
}

Device::Device(const DeviceCreateInfo& p_CreateInfo)
: m_Handle(VK_NULL_HANDLE)
, m_PhysicalDevice(p_CreateInfo.GetPhysicalDevice())
{
	ASSERT(p_CreateInfo.IsFinalized(), "CreateInfo not finalized");
	Initialize(p_CreateInfo);
	Validate();
	LOG("Vulkan device created", "vulkan", Debug);
}

Device::~Device()
{
	ASSERT(m_Handle, "Device null");
	VkResult l_Result = vkDeviceWaitIdle(m_Handle);
	LOG_IF(l_Result != VK_SUCCESS, "Failed to wait for device to become idle. Error: " << System::ResultToString(l_Result), "vulkan", Error);

	vkDestroyDevice(m_Handle, nullptr); // Will also destroy all queues created on device
	LOG("Vulkan device destroyed", "vulkan", Debug);
}

void Device::WaitUntilIdle()
{
	VkResult l_Result = vkDeviceWaitIdle(m_Handle);
	if (l_Result == VK_ERROR_DEVICE_LOST)
	{
		OnDeviceLost();
	}
	else
	{
		VK_THROW_IF_ERR(l_Result, "Failed to wait for device to become idle");
	}
}

std::shared_ptr<Queue> Device::GetQueue(QueueType p_Type, uint32 p_uiIndex) const
{
	const auto& l_Itr = m_Queues.find(p_Type);
	THROW_IF(l_Itr == m_Queues.end(), "Queues of type " << p_Type << " not available");
	THROW_IF_NOT(p_uiIndex < l_Itr->second.size(), "Invalid queue index " << p_uiIndex << " for queue type " << p_Type);
	return l_Itr->second[p_uiIndex];
}

std::shared_ptr<Queue> Device::GetPresentationQueue() const
{
	return GetQueue(QueueType::GraphicsPresentation, 0);
}

std::shared_ptr<PhysicalDevice> Device::GetPhysicalDevice() const
{
	return m_PhysicalDevice;
}

VkDevice Device::GetHandle() const
{
	return m_Handle;
}

void Device::GetLayersToCreate(std::vector<const char*>& p_Layers)
{
	std::copy(n_EnabledLayers.begin(), n_EnabledLayers.end(), std::back_inserter(p_Layers));
#if defined(HAWK_DEBUG)
	if (Config::Instance().Get("vulkan.enableDeviceDebugLayers", false))
	{
		std::copy(n_EnabledDebugLayers.begin(), n_EnabledDebugLayers.end(), std::back_inserter(p_Layers));
	}
#endif

	for (const auto& l_Layer : p_Layers)
	{
		THROW_IF_NOT(PhysicalDevice::IsLayerAvailable(m_PhysicalDevice->GetHandle(), l_Layer), "Device layer not available. Name=" << l_Layer);
	}
}

void Device::GetExtensionsToCreate(std::vector<const char*>& p_Extensions)
{
	std::copy(n_EnabledExtensions.begin(), n_EnabledExtensions.end(), std::back_inserter(p_Extensions));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugExtensions.begin(), n_EnabledDebugExtensions.end(), std::back_inserter(p_Extensions));
#endif

	for (const auto& l_Extension : p_Extensions)
	{
		THROW_IF_NOT(PhysicalDevice::IsExtensionAvailable(m_PhysicalDevice->GetHandle(), l_Extension), "Global device extension not available. Name=" << l_Extension);
	}
}

void Device::CreateDevice(const QueueFamilyCreateInfos& p_QueueFamilyCreateInfos)
{
	ValidateQueueFamilyCreateInfos(p_QueueFamilyCreateInfos);
	VkPhysicalDeviceFeatures l_Features = {};
	GetFeatures(l_Features);

	// TODO: Match requested features again those reported by vkGetPhysicalDeviceFeatures

	std::vector<const char*> l_EnabledLayers = {};
	GetLayersToCreate(l_EnabledLayers);

	std::vector<const char*> l_EnabledExtensions = {};
	GetExtensionsToCreate(l_EnabledExtensions);

	VkDeviceCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	l_Info.queueCreateInfoCount = (uint32)p_QueueFamilyCreateInfos.size();
	l_Info.pQueueCreateInfos = p_QueueFamilyCreateInfos.data();
	l_Info.enabledLayerCount = (uint32)l_EnabledLayers.size();
	l_Info.ppEnabledLayerNames = l_EnabledLayers.data();
	l_Info.enabledExtensionCount = (uint32)l_EnabledExtensions.size();
	l_Info.ppEnabledExtensionNames = l_EnabledExtensions.data();
	l_Info.pEnabledFeatures = &l_Features;
	VK_THROW_IF_NOT_SUCCESS(vkCreateDevice(m_PhysicalDevice->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create device");

	for (auto& l_Elem : p_QueueFamilyCreateInfos)
	{
		delete[] l_Elem.pQueuePriorities;
	}
}

void Device::ExtractQueues(const DeviceCreateInfo::QueueCreateInfoMap& p_Map)
{
	ASSERT(m_Handle, "Device not created");
	for (const auto& l_Entry : p_Map)
	{
		QueueType l_Type = l_Entry.first;
		for (const auto& l_Info : l_Entry.second)
		{
			VkQueue l_Queue = VK_NULL_HANDLE;
			vkGetDeviceQueue(m_Handle, l_Info.m_uiFamilyIndex, l_Info.m_uiQueueIndex, &l_Queue);
			THROW_IF_NOT(l_Queue, "Failed to get handle to queue. QueueType=" << l_Type << " FamilyIndex=" << l_Info.m_uiFamilyIndex << " QueueIndex=" << l_Info.m_uiQueueIndex);
			
			m_Queues[l_Type].emplace_back(std::make_shared<Queue>(l_Queue, l_Type, l_Info.m_uiFamilyIndex));
			LOG("Extracted queue of type from device: " << l_Type << " TypeIndex=" << l_Info.m_uiTypeIndex << " QueueIndex=" << l_Info.m_uiQueueIndex, "vulkan", Debug);
		}
	}
}

void Device::Initialize(const DeviceCreateInfo& p_CreateInfo)
{
	THROW_IF_NOT(p_CreateInfo.GetPhysicalDevice(), "PhysicalDevice is null");

	QueueFamilyCreateInfos l_QueueFamilyCreateInfos;
	GetQueueFamilyCreateInfos(p_CreateInfo.GetQueueCreateInfoMap(), l_QueueFamilyCreateInfos);

	CreateDevice(l_QueueFamilyCreateInfos);
	ExtractQueues(p_CreateInfo.GetQueueCreateInfoMap());
}

void Device::Validate()
{
	THROW_IF_NOT(m_PhysicalDevice, "Physical device null");
	THROW_IF_NOT(m_Handle, "Device null");
	for (const auto& l_Entry : m_QueueRequestMap)
	{
		auto l_Itr = m_Queues.find(l_Entry.first);
		THROW_IF(l_Itr == m_Queues.end(), "Queue type should be available: " << l_Entry.first);
		THROW_IF_NOT(l_Itr->second.size() == l_Entry.second.size(), "Queue count mismatch for queue type: " << l_Entry.first);
	}
}

void Device::ValidateQueueFamilyCreateInfos(const QueueFamilyCreateInfos& p_QueueFamilyCreateInfos)
{
	THROW_IF(p_QueueFamilyCreateInfos.empty(), "At least one queue needs to be specified when creating device");
	for (uint32 i = 0; i < p_QueueFamilyCreateInfos.size(); i++)
	{
		for (uint32 j = i + 1; j < p_QueueFamilyCreateInfos.size(); j++)
		{
			THROW_IF(p_QueueFamilyCreateInfos[i].queueFamilyIndex == p_QueueFamilyCreateInfos[j].queueFamilyIndex, "Duplicate queue family index when creating device");
		}
	}
}

void Device::OnDeviceLost()
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

void Device::GetQueueFamilyCreateInfos(const DeviceCreateInfo::QueueCreateInfoMap& p_QueueCreateMap, QueueFamilyCreateInfos& p_FamilyCreateInfos)
{
	struct FamilyInfo
	{
		uint32 m_uiCount;
		std::unordered_map<uint32, uint32> m_Prios;
	};
	using QueueFamilyMap = std::unordered_map<uint32, FamilyInfo>;
	QueueFamilyMap l_Families;

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
		VkDeviceQueueCreateInfo l_CreateInfo = {};
		l_CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		l_CreateInfo.queueFamilyIndex = l_Family.first;
		l_CreateInfo.queueCount = l_FamilyInfo.m_uiCount;

		float* l_NormalizedPrios = new float[l_FamilyInfo.m_Prios.size()]; // deleted at the end of CreateDevice()

		float l_fSum = 0.0f;
		for (const auto& l_Prio : l_FamilyInfo.m_Prios)
		{
			l_fSum += l_Prio.second;
		}

		for (uint32 i = 0; i < l_FamilyInfo.m_Prios.size(); i++)
		{
			l_NormalizedPrios[i] = l_FamilyInfo.m_Prios.at(i) / l_fSum;
		}
		l_CreateInfo.pQueuePriorities = l_NormalizedPrios;
		p_FamilyCreateInfos.push_back(l_CreateInfo);
		LOG("Queue family registered for creation. FamilyIndex=" << l_CreateInfo.queueFamilyIndex << " QueueCount=" << l_CreateInfo.queueCount << " Prios: " << l_NormalizedPrios, "vulkan", Debug)
	}
}

void Device::GetFeatures(VkPhysicalDeviceFeatures& p_Features)
{
	p_Features.fillModeNonSolid = VK_TRUE;
}

}
}
}