#include "pch.h"
#include "VlkDevice.h"
#include "VlkUtil.h"
#include <iomanip>

namespace Hawk {
namespace Gfx {

VlkDevice::VlkDevice(VkInstance p_Instance)
: m_Instance(p_Instance)
, m_Device(nullptr)
{
	Devices_t l_Devices;
	GetDevices(l_Devices);

	THROW_IF(l_Devices.empty(), "No Vulkan compatible devices found");

	CreateDevice(l_Devices[0]);
	LOG("Created auto-selected vulkan device", "vulkan", Info);
}

VlkDevice::VlkDevice(VkInstance p_Instance, uint32_t p_uiDeviceID)
: m_Instance(p_Instance)
, m_Device(nullptr)
{
	VkPhysicalDevice l_Device = GetByDeviceID(p_uiDeviceID);
	CreateDevice(l_Device);
	LOG("Created vulkan device with ID=" << p_uiDeviceID, "vulkan", Info);
}

VlkDevice::~VlkDevice()
{
	ASSERT(m_Device, "Device null");
	vkDestroyDevice(m_Device, nullptr);
	LOG("Vulkan device destroyed", "vulkan", Debug);
}

void VlkDevice::CmdPrintDevices()
{
	Devices_t l_Devices;
	GetDevices(l_Devices);

	CONSOLE_WRITE_SCOPE();
	for (const auto l_Device : l_Devices)
	{
		VkPhysicalDeviceProperties l_Props;
		GetDeviceProperties(l_Device, l_Props);

		static const uint8_t w = 34;
		std::cout << "\n-" << l_Props.deviceName << "-------------------------------------------------------------------------------------\n";
		std::cout << std::left << std::setw(w) << "DeviceID:" << l_Props.deviceID << "\n";
		std::cout << std::left << std::setw(w) << "DeviceType:" << DeviceTypeToString(l_Props.deviceType) << "\n";
		std::cout << std::left << std::setw(w) << "VendorID:" << l_Props.vendorID << "\n";
		std::cout << std::left << std::setw(w) << "DriverVersion:" << l_Props.driverVersion << "\n";
		std::cout << std::left << std::setw(w) << "Supported API-version:" << VlkUtil::SpecVersionToString(l_Props.apiVersion) << "\n";
		std::cout << std::left << std::setw(w) << "Limits:" << "[Not implemented]\n";
		std::cout << std::left << std::setw(w) << "PipelineCacheUUID:" << PipelineCacheUUIDToString(l_Props.pipelineCacheUUID) << "\n";
		std::cout << std::left << std::setw(w) << "SparseProperties:" << "[Not implemented]\n\n";

		QueueFamilyProperties_t l_QPropsVec;
		GetQueueFamilyProperties(l_Device, l_QPropsVec);

		uint32_t l_uiNum = 1;
		for (const auto l_QProps : l_QPropsVec)
		{
			std::cout << "Queue Family " << l_uiNum << ":\n";
			std::cout << std::left << std::setw(w) << "   Flags:" << QueueFlagsToString(l_QProps.queueFlags) << "\n";
			std::cout << std::left << std::setw(w) << "   Num queues:" << l_QProps.queueCount << "\n";
			std::cout << std::left << std::setw(w) << "   TimestampValidBits:" << TimestampValidBitsToString(l_QProps.timestampValidBits) << "\n";
			std::cout << std::left << std::setw(w) << "   MinImageTransferGranularity:" << l_QProps.minImageTransferGranularity << "\n\n";
			l_uiNum++;
		}
	}
}

void VlkDevice::CreateDevice(VkPhysicalDevice p_Device)
{
	/*VkDeviceCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	l_Info.pNext = nullptr; // must be null or pointer to an extension-specific structure
	l_Info.flags = 0;		// Reserved for future use
	VK_THROW_IF_NOT_SUCCESS(vkCreateDevice(p_Device, &l_Info, nullptr, &m_Device), "Failed to create device");*/
}

void VlkDevice::GetDevices(Devices_t& p_Devices) const
{
	uint32_t l_uiCount;
	VK_THROW_IF_NOT_SUCCESS(vkEnumeratePhysicalDevices(m_Instance, &l_uiCount, nullptr), "Failed to get physical devices count");

	p_Devices.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumeratePhysicalDevices(m_Instance, &l_uiCount, p_Devices.data()), "Failed to get physical devices");
}

void VlkDevice::GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties) const
{
	vkGetPhysicalDeviceProperties(p_Device, &p_Properties);
}

void VlkDevice::GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties_t& p_Properties) const
{
	uint32_t l_uiCount;
	vkGetPhysicalDeviceQueueFamilyProperties(p_Device, &l_uiCount, nullptr);

	p_Properties.resize(l_uiCount);
	vkGetPhysicalDeviceQueueFamilyProperties(p_Device, &l_uiCount, p_Properties.data());
}

VkPhysicalDevice VlkDevice::GetByDeviceID(uint32_t p_uiDeviceID) const
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


std::string VlkDevice::PipelineCacheUUIDToString(const uint8_t* p_UUID) const
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

std::string VlkDevice::DeviceTypeToString(VkPhysicalDeviceType p_Type) const
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