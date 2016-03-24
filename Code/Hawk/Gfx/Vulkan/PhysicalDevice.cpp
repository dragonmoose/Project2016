#include "pch.h"
#include "PhysicalDevice.h"
#include "Constants.h"
#include "Util.h"
#include "Util/Algorithm.h"
#include <iomanip>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

PhysicalDevice::PhysicalDevice(std::shared_ptr<Instance> p_Instance)
: m_Instance(p_Instance)
, m_Handle(GetDeviceByIndex(0))
{
	Init();
}

PhysicalDevice::PhysicalDevice(std::shared_ptr<Instance> p_Instance, uint32 p_uiDeviceID)
: m_Instance(p_Instance)
, m_Handle(GetDeviceByID(p_uiDeviceID))
{
	Init();
}

PhysicalDevice::~PhysicalDevice()
{
	LOG("Physical device destroyed", "vulkan", Debug);
}

bool PhysicalDevice::IsLayerAvailable(VkPhysicalDevice p_Device, const std::string& p_Name)
{
	LayerProperties l_Layers;
	GetAllLayers(p_Device, l_Layers, false);
	return std::find_if(l_Layers.begin(), l_Layers.end(),
		[p_Name](const VkLayerProperties& p_Layer) { return p_Name == p_Layer.layerName && System::GetAPIVersion() >= p_Layer.specVersion; }) != l_Layers.end();
}

bool PhysicalDevice::IsExtensionAvailable(VkPhysicalDevice p_Device, const std::string& p_Name, const std::string& p_LayerName)
{
	if (!p_LayerName.empty() && !IsLayerAvailable(p_Device, p_LayerName)) return false;

	ExtensionProperties l_Extensions;
	GetAllExtensions(p_Device, l_Extensions, p_LayerName);
	return std::find_if(l_Extensions.begin(), l_Extensions.end(),
		[p_Name](const VkExtensionProperties& p_Extension) { return p_Name == p_Extension.extensionName; }) != l_Extensions.end();
}

VkPhysicalDevice PhysicalDevice::GetHandle() const
{
	return m_Handle;
}

void PhysicalDevice::GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties& p_Properties)
{
	uint32 l_uiCount;
	vkGetPhysicalDeviceQueueFamilyProperties(p_Device, &l_uiCount, nullptr);

	p_Properties.resize(l_uiCount);
	vkGetPhysicalDeviceQueueFamilyProperties(p_Device, &l_uiCount, p_Properties.data());
}

VkFormat PhysicalDevice::GetBackBufferDepthFormat() const
{
	return m_BackBufferDepthFormat;
}

VkFormat PhysicalDevice::GetBackBufferColorFormat() const
{
	return m_BackBufferColorFormat;
}

#ifdef HAWK_DEBUG
void PhysicalDevice::CmdPrintDevices()
{
	Devices l_Devices;
	GetDevices(l_Devices);

	CONSOLE_WRITE_SCOPE();
	uint32 l_uiIndex = 0;
	for (const auto l_Device : l_Devices)
	{
		VkPhysicalDeviceProperties l_Props;
		GetDeviceProperties(l_Device, l_Props);

		std::cout << "\n-" << l_Props.deviceName << "-------------------------------------------------------------------------------------\n";

		static const uint8 w = 34;
		std::cout << std::left << std::setw(w) << "Index:" << l_uiIndex << "\n";
		std::cout << std::left << std::setw(w) << "DeviceID:" << l_Props.deviceID << "\n";
		std::cout << std::left << std::setw(w) << "DeviceType:" << Util::DeviceTypeToString(l_Props.deviceType) << "\n";
		std::cout << std::left << std::setw(w) << "VendorID:" << l_Props.vendorID << "\n";
		std::cout << std::left << std::setw(w) << "DriverVersion:" << l_Props.driverVersion << "\n";
		std::cout << std::left << std::setw(w) << "Supported API-version:" << Util::SpecVersionToString(l_Props.apiVersion) << "\n";
		std::cout << std::left << std::setw(w) << "Limits:" << "[Not implemented]\n";
		std::cout << std::left << std::setw(w) << "PipelineCacheUUID:" << Util::PipelineCacheUUIDToString(l_Props.pipelineCacheUUID) << "\n";
		std::cout << std::left << std::setw(w) << "SparseProperties:" << "[Not implemented]\n\n";
		l_uiIndex++;
	}
}

void PhysicalDevice::CmdPrintQueueFamilies(uint32 p_uiDeviceIndex)
{
	QueueFamilyProperties l_QPropsVec;
	GetQueueFamilyProperties(GetDeviceByIndex(p_uiDeviceIndex), l_QPropsVec);

	CONSOLE_WRITE_SCOPE();
	uint32 l_uiFamilyIndex = 0;
	for (const auto l_QProps : l_QPropsVec)
	{
		std::cout << "\n-Queue Family " << l_uiFamilyIndex << "-------------------------------------------------------------------------------------\n";

		static const uint8 w = 34;
		std::cout << std::left << std::setw(w) << "Flags:" << Util::QueueFlagsToString(l_QProps.queueFlags) << "\n";
		std::cout << std::left << std::setw(w) << "Num queues:" << l_QProps.queueCount << "\n";
		std::cout << std::left << std::setw(w) << "TimestampValidBits:" << Util::TimestampValidBitsToString(l_QProps.timestampValidBits) << "\n";
		std::cout << std::left << std::setw(w) << "MinImageTransferGranularity:" << l_QProps.minImageTransferGranularity << "\n\n";
		l_uiFamilyIndex++;
	}
}

void PhysicalDevice::CmdPrintLayers(uint32 p_uiDeviceIndex, bool p_bKeepUnsupported)
{
	LayerProperties l_Layers;
	GetAllLayers(GetDeviceByIndex(p_uiDeviceIndex), l_Layers, p_bKeepUnsupported);

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	for (const auto& l_Layer : l_Layers)
	{
		if (l_Layer.specVersion > System::GetAPIVersion())
		{
			std::cout << "!Requires API " << Util::SpecVersionToString(l_Layer.specVersion) << "!";
		}

		std::cout << l_Layer.layerName << " (" << l_Layer.description << ")" <<
			" SpecVersion: " << Util::SpecVersionToString(l_Layer.specVersion) <<
			" ImplementationVersion: " << l_Layer.implementationVersion << "\n";
	}
	std::cout << "\n";
}

void PhysicalDevice::CmdPrintExtensions(uint32 p_uiDeviceIndex, bool p_bKeepUnsupported)
{
	VkPhysicalDevice l_Device = GetDeviceByIndex(p_uiDeviceIndex);

	ExtensionProperties l_Extensions;
	GetAllExtensions(l_Device, l_Extensions);

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n-Global extensions-----------------------\n";
	for (const auto& l_Extension : l_Extensions)
	{
		std::cout << l_Extension.extensionName << " Version: " << l_Extension.specVersion << "\n";
	}
	std::cout << "\n-Layer extensions------------------------\n";

	LayerProperties l_Layers;
	GetAllLayers(l_Device, l_Layers, p_bKeepUnsupported);

	for (const auto& l_Layer : l_Layers)
	{
		l_Extensions.clear();
		GetAllExtensions(l_Device, l_Extensions, l_Layer.layerName);
		if (!l_Extensions.empty())
		{
			if (l_Layer.specVersion > System::GetAPIVersion())
			{
				std::cout << "## N/A - requires API " << Util::SpecVersionToString(l_Layer.specVersion) << " ## ";
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

void PhysicalDevice::CmdPrintDepthFormats(uint32 p_uiDeviceIndex, bool p_bKeepUnsupported)
{
	VkPhysicalDevice l_Device = GetDeviceByIndex(p_uiDeviceIndex);

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	for (const auto& l_Format : Util::GetDepthFormats())
	{
		if (IsDepthFormatSupported(l_Device, l_Format))
		{
			std::cout << Util::DepthFormatToString(l_Format) << "\n";
		}
		else if (p_bKeepUnsupported)
		{
			std::cout << Util::DepthFormatToString(l_Format) << "\t**NOT SUPPORTED**\n";
		}
	}
	std::cout << "\n";
}
#endif

void PhysicalDevice::GetAllLayers(VkPhysicalDevice p_Device, LayerProperties& p_Layers, bool p_bKeepUnsupported)
{
	uint32 l_uiCount = 0;
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateDeviceLayerProperties(p_Device, &l_uiCount, nullptr), "Failed to get device layer count");

	p_Layers.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateDeviceLayerProperties(p_Device, &l_uiCount, p_Layers.data()), "Failed to get device layers");

	if (!p_bKeepUnsupported)
	{
		hwk::erase_if(p_Layers, [](const VkLayerProperties& p_Layer) { return p_Layer.specVersion > System::GetAPIVersion(); });
	}
}

void PhysicalDevice::GetAllExtensions(VkPhysicalDevice p_Device, ExtensionProperties& p_Extensions, const std::string& p_LayerName)
{
	uint32 l_uiCount = 0;
	const char* l_LayerName = !p_LayerName.empty() ? p_LayerName.c_str() : nullptr;

	VK_THROW_IF_NOT_SUCCESS(vkEnumerateDeviceExtensionProperties(p_Device, l_LayerName, &l_uiCount, nullptr), "Failed to get device extension count. Layer=" << p_LayerName);

	p_Extensions.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateDeviceExtensionProperties(p_Device, l_LayerName, &l_uiCount, p_Extensions.data()), "Failed to get device extensions. Layer=" << p_LayerName);
}

VkPhysicalDevice PhysicalDevice::GetDeviceByIndex(uint32 p_uiIndex) const
{
	Devices l_Devices;
	GetDevices(l_Devices);

	THROW_IF_NOT(p_uiIndex < l_Devices.size(), "Invalid device index");
	return l_Devices[p_uiIndex];
}

void PhysicalDevice::GetDevices(Devices& p_Devices) const
{
	uint32 l_uiCount;
	VK_THROW_IF_NOT_SUCCESS(vkEnumeratePhysicalDevices(m_Instance->GetHandle(), &l_uiCount, nullptr), "Failed to get physical devices count");

	p_Devices.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumeratePhysicalDevices(m_Instance->GetHandle(), &l_uiCount, p_Devices.data()), "Failed to get physical devices");
}

VkPhysicalDevice PhysicalDevice::GetDeviceByID(uint32 p_uiDeviceID) const
{
	Devices l_Devices;
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

void PhysicalDevice::GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties)
{
	vkGetPhysicalDeviceProperties(p_Device, &p_Properties);
}

bool PhysicalDevice::IsDepthFormatSupported(VkPhysicalDevice p_Device, VkFormat p_Format)
{
	VkFormatProperties l_Props = {};
	vkGetPhysicalDeviceFormatProperties(p_Device, p_Format, &l_Props);
	return !!(l_Props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool PhysicalDevice::IsColorFormatSupported(VkPhysicalDevice p_Device, VkFormat p_Format)
{
	VkFormatProperties l_Props = {};
	vkGetPhysicalDeviceFormatProperties(p_Device, p_Format, &l_Props);
	return !!(l_Props.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);
}

void PhysicalDevice::Init()
{
	SelectBackBufferDepthFormat();
	SelectBackBufferColorFormat();
}

void PhysicalDevice::SelectBackBufferDepthFormat()
{
	VkFormat l_SelectedFormat = VK_FORMAT_UNDEFINED;
	if (IsDepthFormatSupported(m_Handle, Constants::c_PreferredDepthBufferFormat))
	{
		l_SelectedFormat = Constants::c_PreferredDepthBufferFormat;
	}
	else
	{
		LOG("Preferred depth buffer format not supported by device, attempting to choose another one", "vulkan", Warning);

		for (const auto& l_Format : Util::GetDepthFormats())
		{
			if (IsDepthFormatSupported(m_Handle, l_Format))
			{
				l_SelectedFormat = l_Format;
				break;
			}
		}
	}
	THROW_IF(l_SelectedFormat == VK_FORMAT_UNDEFINED, "Failed to find a depth buffer format");

	m_BackBufferDepthFormat = l_SelectedFormat;
	LOG("Selected depth format: " << Util::DepthFormatToString(l_SelectedFormat), "vulkan", Debug);
}

void PhysicalDevice::SelectBackBufferColorFormat()
{
	VkFormat l_SelectedFormat = VK_FORMAT_UNDEFINED;
	if (IsColorFormatSupported(m_Handle, Constants::c_PreferredBackBufferColorFormat))
	{
		l_SelectedFormat = Constants::c_PreferredBackBufferColorFormat;
	}
	THROW_IF(l_SelectedFormat == VK_FORMAT_UNDEFINED, "Failed to find a color format for the back buffer");
	m_BackBufferColorFormat = l_SelectedFormat;
}

}
}
}