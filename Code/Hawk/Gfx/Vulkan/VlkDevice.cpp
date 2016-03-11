#include "pch.h"
#include "VlkDevice.h"
#include "VlkUtil.h"
#include "Util/Algorithm.h"
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

VlkDevice::VlkDevice(VkInstance p_Instance)
: m_Instance(p_Instance)
, m_Device(nullptr)
, m_PhysicalDevice(nullptr)
{
	CreateDevice(GetDeviceByIndex(0));
	LOG("Created auto-selected vulkan device", "vulkan", Info);
}

VlkDevice::VlkDevice(VkInstance p_Instance, uint32_t p_uiDeviceID)
: m_Instance(p_Instance)
, m_Device(nullptr)
, m_PhysicalDevice(nullptr)
{
	CreateDevice(GetDeviceByID(p_uiDeviceID));
	LOG("Created vulkan device from given ID=" << p_uiDeviceID, "vulkan", Info);
}

VlkDevice::~VlkDevice()
{
	ASSERT(m_Device, "Device null");
	vkDestroyDevice(m_Device, nullptr);
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
		VK_THROW_IF_ERR(l_Result, "Failed to wait until device idle");
	}
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

VlkDevice::QueueCreateInfoVec_t VlkDevice::GetQueueCreateInfo(VkPhysicalDevice p_Device)
{
	static const uint32_t sc_uiNumGfxQueues = 1;
	QueueCreateInfoVec_t l_Vec;

	uint32_t l_uiGfxQueueIndex;
	FindMatchingQueueFamily(p_Device, VK_QUEUE_GRAPHICS_BIT, sc_uiNumGfxQueues, l_uiGfxQueueIndex);

	VkDeviceQueueCreateInfo l_Info;
	l_Info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	l_Info.pNext = nullptr; // null or pointer to extension-specific structure
	l_Info.flags = 0; // reserved for future use
	l_Info.queueFamilyIndex = l_uiGfxQueueIndex;
	l_Info.queueCount = sc_uiNumGfxQueues;

	float l_Priorities[1] = {1.0f};
	l_Info.pQueuePriorities = l_Priorities;
	l_Vec.push_back(l_Info);

	return l_Vec;
}

void VlkDevice::CreateDevice(VkPhysicalDevice p_Device, const QueueCreateInfoVec_t& p_QueueCreateInfoVec)
{
	ValidateQueueCreateInfoVec(p_QueueCreateInfoVec);
	VkPhysicalDeviceFeatures l_Features = {};
	GetFeatures(l_Features);

	std::vector<const char*> l_EnabledLayers = {};
	GetLayersToCreate(p_Device, l_EnabledLayers);

	std::vector<const char*> l_EnabledExtensions = {};
	GetExtensionsToCreate(p_Device, l_EnabledExtensions);

	VkDeviceCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	l_Info.pNext = nullptr; // must be null or pointer to an extension-specific structure
	l_Info.flags = 0;		// Reserved for future use
	l_Info.queueCreateInfoCount = p_QueueCreateInfoVec.size();
	l_Info.pQueueCreateInfos = p_QueueCreateInfoVec.data();
	l_Info.enabledLayerCount = l_EnabledLayers.size();
	l_Info.ppEnabledLayerNames = l_EnabledLayers.data();
	l_Info.enabledExtensionCount = l_EnabledExtensions.size();
	l_Info.ppEnabledExtensionNames = l_EnabledExtensions.data();
	l_Info.pEnabledFeatures = &l_Features;
	VK_THROW_IF_NOT_SUCCESS(vkCreateDevice(p_Device, &l_Info, nullptr, &m_Device), "Failed to create device");
	m_PhysicalDevice = p_Device;
}

void VlkDevice::CreateDevice(VkPhysicalDevice p_Device)
{
	CreateDevice(p_Device, GetQueueCreateInfo(p_Device));
}

void VlkDevice::ValidateQueueCreateInfoVec(const QueueCreateInfoVec_t& p_QueueCreateInfoVec)
{
	THROW_IF(p_QueueCreateInfoVec.empty(), "At least one queue needs to be specified when creating device");
	for (uint32_t i = 0; i < p_QueueCreateInfoVec.size(); i++)
	{
		for (uint32_t j = i + 1; j < p_QueueCreateInfoVec.size(); j++)
		{
			THROW_IF(p_QueueCreateInfoVec[i].queueFamilyIndex == p_QueueCreateInfoVec[j].queueFamilyIndex, "Duplicate queue family index when creating device");
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
		CreateDevice(m_PhysicalDevice);
	}
	catch (Exception& e)
	{
		FATAL_EXCEPTION(e, "vulkan");
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
}

void VlkDevice::FindMatchingQueueFamily(const VkPhysicalDevice p_Device, VkQueueFlags p_Flags, uint32_t p_uiCount, uint32_t& p_uiIndex)
{
	QueueFamilyProperties_t l_PropsVec;
	GetQueueFamilyProperties(p_Device, l_PropsVec);

	for (uint32_t i = 0; i < l_PropsVec.size(); i++)
	{
		const VkQueueFamilyProperties& l_Props = l_PropsVec[i];
		bool l_bHasFlags = (p_Flags & l_Props.queueFlags) == p_Flags;
		bool l_bEnoughQueues = p_uiCount <= l_Props.queueCount;
		if (l_bHasFlags && l_bEnoughQueues)
		{
			p_uiIndex = i;
			return;
		}
	}
	THROW("Failed to find queue family with the following requirements. Flags: " << QueueFlagsToString(p_Flags) << " Count: " << p_uiCount);
}

void VlkDevice::GetFeatures(VkPhysicalDeviceFeatures& /*p_Features*/)
{
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

}
}