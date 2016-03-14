#include "pch.h"
#include "VlkDevice.h"
#include "VlkUtil.h"
#include "Util/Algorithm.h"
#include "System/StreamOperators.h"
#include <iomanip>

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
, m_Device(VK_NULL_HANDLE)
, m_QueueRequestMap(p_CreateInfo.GetQueueRequestMap())
, m_Surface(p_CreateInfo.GetSurface())
{
	ASSERT(p_CreateInfo.IsFinalized(), "CreateInfo not finalized");
	m_PhysicalDevice = p_CreateInfo.UseDeviceID() ? GetDeviceByID(p_CreateInfo.GetDeviceID()) : GetDeviceByIndex(0);
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

#ifdef HAWK_DEBUG
void VlkDevice::CmdPrintDevices()
{
	Devices_t l_Devices;
	GetDevices(l_Devices);

	CONSOLE_WRITE_SCOPE();
	uint32_t l_uiIndex = 0;
	for (const auto l_Device : l_Devices)
	{
		VkPhysicalDeviceProperties l_Props;
		GetDeviceProperties(l_Device, l_Props);

		std::cout << "\n-" << l_Props.deviceName << "-------------------------------------------------------------------------------------\n";

		static const uint8_t w = 34;
		std::cout << std::left << std::setw(w) << "Index:" << l_uiIndex << "\n";
		std::cout << std::left << std::setw(w) << "DeviceID:" << l_Props.deviceID << "\n";
		std::cout << std::left << std::setw(w) << "DeviceType:" << DeviceTypeToString(l_Props.deviceType) << "\n";
		std::cout << std::left << std::setw(w) << "VendorID:" << l_Props.vendorID << "\n";
		std::cout << std::left << std::setw(w) << "DriverVersion:" << l_Props.driverVersion << "\n";
		std::cout << std::left << std::setw(w) << "Supported API-version:" << VlkUtil::SpecVersionToString(l_Props.apiVersion) << "\n";
		std::cout << std::left << std::setw(w) << "Limits:" << "[Not implemented]\n";
		std::cout << std::left << std::setw(w) << "PipelineCacheUUID:" << PipelineCacheUUIDToString(l_Props.pipelineCacheUUID) << "\n";
		std::cout << std::left << std::setw(w) << "SparseProperties:" << "[Not implemented]\n\n";
		l_uiIndex++;
	}
}

void VlkDevice::CmdPrintQueueFamilies(uint32_t p_uiDeviceIndex)
{
	QueueFamilyProperties_t l_QPropsVec;
	GetQueueFamilyProperties(GetDeviceByIndex(p_uiDeviceIndex), l_QPropsVec);

	CONSOLE_WRITE_SCOPE();
	uint32_t l_uiFamilyIndex = 0;
	for (const auto l_QProps : l_QPropsVec)
	{
		std::cout << "\n-Queue Family " << l_uiFamilyIndex << "-------------------------------------------------------------------------------------\n";

		static const uint8_t w = 34;
		std::cout << std::left << std::setw(w) << "Flags:" << QueueFlagsToString(l_QProps.queueFlags) << "\n";
		std::cout << std::left << std::setw(w) << "Num queues:" << l_QProps.queueCount << "\n";
		std::cout << std::left << std::setw(w) << "TimestampValidBits:" << TimestampValidBitsToString(l_QProps.timestampValidBits) << "\n";
		std::cout << std::left << std::setw(w) << "MinImageTransferGranularity:" << l_QProps.minImageTransferGranularity << "\n\n";
		l_uiFamilyIndex++;
	}
}

void VlkDevice::CmdPrintLayers(uint32_t p_uiDeviceIndex, bool p_bKeepUnsupported)
{
	LayerProperties_t l_Layers;
	GetAllLayers(GetDeviceByIndex(p_uiDeviceIndex), l_Layers, p_bKeepUnsupported);

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	for (const auto& l_Layer : l_Layers)
	{
		if (l_Layer.specVersion > VlkSystem::GetAPIVersion())
		{
			std::cout << "!Requires API " << VlkUtil::SpecVersionToString(l_Layer.specVersion) << "!";
		}

		std::cout << l_Layer.layerName << " (" << l_Layer.description << ")" <<
			" SpecVersion: " << VlkUtil::SpecVersionToString(l_Layer.specVersion) <<
			" ImplementationVersion: " << l_Layer.implementationVersion << "\n";
	}
	std::cout << "\n";
}

void VlkDevice::CmdPrintExtensions(uint32_t p_uiDeviceIndex, bool p_bKeepUnsupported)
{
	VkPhysicalDevice l_Device = GetDeviceByIndex(p_uiDeviceIndex);

	ExtensionProperties_t l_Extensions;
	GetAllExtensions(l_Device, l_Extensions);

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n-Global extensions-----------------------\n";
	for (const auto& l_Extension : l_Extensions)
	{
		std::cout << l_Extension.extensionName << " Version: " << l_Extension.specVersion << "\n";
	}
	std::cout << "\n-Layer extensions------------------------\n";

	LayerProperties_t l_Layers;
	GetAllLayers(l_Device, l_Layers, p_bKeepUnsupported);

	for (const auto& l_Layer : l_Layers)
	{
		l_Extensions.clear();
		GetAllExtensions(l_Device, l_Extensions, l_Layer.layerName);
		if (!l_Extensions.empty())
		{
			if (l_Layer.specVersion > VlkSystem::GetAPIVersion())
			{
				std::cout << "## N/A - requires API " << VlkUtil::SpecVersionToString(l_Layer.specVersion) << " ## ";
			}
			std::cout << l_Layer.layerName << ":\n";
			for (const auto& l_Extension : l_Extensions)
			{
				std::cout << "\t" << l_Extension.extensionName << " Version: " << l_Extension.specVersion << "\n";
			}
			std::cout << "\n";
		}
	}
	std::cout << "\n";
}
#endif

bool VlkDevice::IsLayerAvailable(VkPhysicalDevice p_Device, const std::string& p_Name)
{
	LayerProperties_t l_Layers;
	GetAllLayers(p_Device, l_Layers, false);
	return std::find_if(l_Layers.begin(), l_Layers.end(),
		[p_Name](const VkLayerProperties& p_Layer) { return p_Name == p_Layer.layerName && VlkSystem::GetAPIVersion() >= p_Layer.specVersion; }) != l_Layers.end();
}

bool VlkDevice::IsExtensionAvailable(VkPhysicalDevice p_Device, const std::string& p_Name, const std::string& p_LayerName)
{
	if (!p_LayerName.empty() && !IsLayerAvailable(p_Device, p_LayerName)) return false;

	ExtensionProperties_t l_Extensions;
	GetAllExtensions(p_Device, l_Extensions, p_LayerName);
	return std::find_if(l_Extensions.begin(), l_Extensions.end(),
		[p_Name](const VkExtensionProperties& p_Extension) { return p_Name == p_Extension.extensionName; }) != l_Extensions.end();
}

void VlkDevice::GetAllLayers(VkPhysicalDevice p_Device, LayerProperties_t& p_Layers, bool p_bKeepUnsupported)
{
	uint32_t l_uiCount = 0;
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateDeviceLayerProperties(p_Device, &l_uiCount, nullptr), "Failed to get device layer count");

	p_Layers.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateDeviceLayerProperties(p_Device, &l_uiCount, p_Layers.data()), "Failed to get device layers");

	if (!p_bKeepUnsupported)
	{
		hwk::erase_if(p_Layers, [](const VkLayerProperties& p_Layer) { return p_Layer.specVersion > VlkSystem::GetAPIVersion(); });
	}
}

void VlkDevice::GetAllExtensions(VkPhysicalDevice p_Device, ExtensionProperties_t& p_Extensions, const std::string& p_LayerName)
{
	uint32_t l_uiCount = 0;
	const char* l_LayerName = !p_LayerName.empty() ? p_LayerName.c_str() : nullptr;

	VK_THROW_IF_NOT_SUCCESS(vkEnumerateDeviceExtensionProperties(p_Device, l_LayerName, &l_uiCount, nullptr), "Failed to get device extension count. Layer=" << p_LayerName);

	p_Extensions.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateDeviceExtensionProperties(p_Device, l_LayerName, &l_uiCount, p_Extensions.data()), "Failed to get device extensions. Layer=" << p_LayerName);
}

void VlkDevice::GetLayersToCreate(VkPhysicalDevice p_Device, std::vector<const char*>& p_Layers)
{
	std::copy(n_EnabledLayers.begin(), n_EnabledLayers.end(), std::back_inserter(p_Layers));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugLayers.begin(), n_EnabledDebugLayers.end(), std::back_inserter(p_Layers));
#endif

	for (const auto& l_Layer : p_Layers)
	{
		THROW_IF_NOT(IsLayerAvailable(p_Device, l_Layer), "Device layer not available. Name=" << l_Layer);
	}
}

void VlkDevice::GetExtensionsToCreate(VkPhysicalDevice p_Device, std::vector<const char*>& p_Extensions)
{
	std::copy(n_EnabledExtensions.begin(), n_EnabledExtensions.end(), std::back_inserter(p_Extensions));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugExtensions.begin(), n_EnabledDebugExtensions.end(), std::back_inserter(p_Extensions));
#endif

	for (const auto& l_Extension : p_Extensions)
	{
		THROW_IF_NOT(IsExtensionAvailable(p_Device, l_Extension), "Global device extension not available. Name=" << l_Extension);
	}
}

void VlkDevice::CreateDevice(const QueueFamilyCreateInfos_t& p_QueueFamilyCreateInfos)
{
	ValidateQueueFamilyCreateInfos(p_QueueFamilyCreateInfos);
	VkPhysicalDeviceFeatures l_Features = {};
	GetFeatures(l_Features);

	std::vector<const char*> l_EnabledLayers = {};
	GetLayersToCreate(m_PhysicalDevice, l_EnabledLayers);

	std::vector<const char*> l_EnabledExtensions = {};
	GetExtensionsToCreate(m_PhysicalDevice, l_EnabledExtensions);

	VkDeviceCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	l_Info.queueCreateInfoCount = p_QueueFamilyCreateInfos.size();
	l_Info.pQueueCreateInfos = p_QueueFamilyCreateInfos.data();
	l_Info.enabledLayerCount = l_EnabledLayers.size();
	l_Info.ppEnabledLayerNames = l_EnabledLayers.data();
	l_Info.enabledExtensionCount = l_EnabledExtensions.size();
	l_Info.ppEnabledExtensionNames = l_EnabledExtensions.data();
	l_Info.pEnabledFeatures = &l_Features;
	VK_THROW_IF_NOT_SUCCESS(vkCreateDevice(m_PhysicalDevice, &l_Info, nullptr, &m_Device), "Failed to create device");
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

void VlkDevice::GetDevices(Devices_t& p_Devices) const
{
	uint32_t l_uiCount;
	VK_THROW_IF_NOT_SUCCESS(vkEnumeratePhysicalDevices(m_Instance, &l_uiCount, nullptr), "Failed to get physical devices count");

	p_Devices.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumeratePhysicalDevices(m_Instance, &l_uiCount, p_Devices.data()), "Failed to get physical devices");
}

VkPhysicalDevice VlkDevice::GetDeviceByIndex(uint32_t p_uiIndex) const
{
	Devices_t l_Devices;
	GetDevices(l_Devices);

	THROW_IF_NOT(p_uiIndex < l_Devices.size(), "Invalid device index");
	return l_Devices[p_uiIndex];
}

VkPhysicalDevice VlkDevice::GetDeviceByID(uint32_t p_uiDeviceID) const
{
	Devices_t l_Devices;
	GetDevices(l_Devices);

	for (const auto l_Device : l_Devices)
	{
		VkPhysicalDeviceProperties l_Props;
		GetDeviceProperties(l_Device, l_Props);
		if (l_Props.deviceID == p_uiDeviceID)
		{
			return l_Device;
		}
	}
	THROW("Failed to find device with ID=" << p_uiDeviceID);
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

void VlkDevice::GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties)
{
	vkGetPhysicalDeviceProperties(p_Device, &p_Properties);
}

void VlkDevice::GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties_t& p_Properties)
{
	uint32_t l_uiCount;
	vkGetPhysicalDeviceQueueFamilyProperties(p_Device, &l_uiCount, nullptr);

	p_Properties.resize(l_uiCount);
	vkGetPhysicalDeviceQueueFamilyProperties(p_Device, &l_uiCount, p_Properties.data());

	/*{	-------------------- test data --------------------
		VkQueueFamilyProperties l_Props;
		l_Props.queueFlags = VK_QUEUE_GRAPHICS_BIT;
		l_Props.queueCount = 2;
		p_Properties.push_back(l_Props);
	}
	{
		VkQueueFamilyProperties l_Props;
		l_Props.queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
		l_Props.queueCount = 2;
		p_Properties.push_back(l_Props);
	}
	{
		VkQueueFamilyProperties l_Props;
		l_Props.queueFlags = VK_QUEUE_TRANSFER_BIT;
		l_Props.queueCount = 1;
		p_Properties.push_back(l_Props);
	}
	{
		VkQueueFamilyProperties l_Props;
		l_Props.queueFlags = VK_QUEUE_TRANSFER_BIT;
		l_Props.queueCount = 1;
		p_Properties.push_back(l_Props);
	}*/
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
	QueueFamilyProperties_t l_PropsVec;
	GetQueueFamilyProperties(m_PhysicalDevice, l_PropsVec);

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
				VkQueueFlags l_Flag = QueueTypeToFlag(l_Type);
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
		VK_THROW_IF_NOT_SUCCESS(vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, l_uiFamilyIndex, m_Surface, &l_bResult), "Failed to check support for WSI");
		THROW_IF_NOT(l_bResult, "WSI-check failed: Queue family " << l_uiFamilyIndex << " does not support the specified surface");

		l_bResult = vkGetPhysicalDeviceWin32PresentationSupportKHR(m_PhysicalDevice, l_uiFamilyIndex);
		THROW_IF_NOT(l_bResult, "WSI-check failed: Queue family " << l_uiFamilyIndex << " does not support presentation on Windows Desktop");
	}
}

std::string VlkDevice::PipelineCacheUUIDToString(const uint8_t* p_UUID)
{
	std::ostringstream l_Stream;
	for (int i = 0; i < VK_UUID_SIZE; i++)
	{
		l_Stream << static_cast<uint32_t>(p_UUID[i]);
		if (i < VK_UUID_SIZE - 1)
		{
			l_Stream << ":";
		}
	}
	return l_Stream.str();
}

std::string VlkDevice::DeviceTypeToString(VkPhysicalDeviceType p_Type)
{
	switch (p_Type)
	{
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			return "Other";
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return "IntegratedGPU";
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return "DiscreteGPU";
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			return "VirtualGPU";
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return "CPU";
		default:
			THROW("DeviceType out of bounds: " << p_Type);
	}
}

std::string VlkDevice::QueueFlagsToString(VkQueueFlags p_Flags)
{
	std::ostringstream l_Stream;
	if (p_Flags & VK_QUEUE_GRAPHICS_BIT)
	{
		l_Stream << "Graphics;";
	}
	if (p_Flags & VK_QUEUE_COMPUTE_BIT)
	{
		l_Stream << "Compute;";
	}
	if (p_Flags & VK_QUEUE_TRANSFER_BIT)
	{
		l_Stream << "Transfer;";
	}
	if (p_Flags & VK_QUEUE_SPARSE_BINDING_BIT)
	{
		l_Stream << "SparseBinding";
	}
	return l_Stream.str();
}

std::string VlkDevice::TimestampValidBitsToString(uint32_t p_Bits)
{
	if (p_Bits == 0)
	{
		return "No timestamp support";
	}

	std::ostringstream l_Stream;
	l_Stream << p_Bits;
	if (p_Bits < 32 || p_Bits > 64)
	{
		l_Stream << " [Invalid bit count!]";
	}
	return l_Stream.str();
}

VkQueueFlags VlkDevice::QueueTypeToFlag(VlkQueueType p_Type)
{
	switch (p_Type)
	{
		case VlkQueueType::Graphics:
			return VK_QUEUE_GRAPHICS_BIT;
		case VlkQueueType::Compute:
			return VK_QUEUE_COMPUTE_BIT;
		case VlkQueueType::Transfer:
			return VK_QUEUE_TRANSFER_BIT;
		case VlkQueueType::SparseBinding:
			return VK_QUEUE_SPARSE_BINDING_BIT;
		default:
			THROW("Invalid type: " << (int)p_Type);
	}
}

}
}