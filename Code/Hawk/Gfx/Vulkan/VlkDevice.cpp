#include "pch.h"
#include "VlkDevice.h"
#include "VlkUtil.h"
#include <iomanip>

namespace Hawk {
namespace Gfx {

VlkDevice::VlkDevice(VkInstance p_Instance)
: m_Instance(p_Instance)
{
	LOG("Created auto-selected vulkan device", "vulkan", Info);
}

VlkDevice::VlkDevice(VkInstance p_Instance, uint32_t p_uiDeviceID)
: m_Instance(p_Instance)
{
	LOG("Created vulkan device with ID=" << p_uiDeviceID, "vulkan", Info);
}

VlkDevice::~VlkDevice()
{
	LOG("Vulkan device destroyed", "vulkan", Debug);
}

void VlkDevice::CmdPrintDevices()
{
	Devices_t l_Devices;
	GetDevices(l_Devices);

	CONSOLE_WRITE_SCOPE();
	for (const auto l_Device : l_Devices)
	{
		VkPhysicalDeviceProperties l_Prop;
		GetDeviceProperties(l_Device, l_Prop);

		static const uint8_t w = 30;
		std::cout << "\n-" << l_Prop.deviceName << "---------------------------------------------------------------\n";
		std::cout << std::left << std::setw(w) << "DeviceID:" << l_Prop.deviceID << "\n";
		std::cout << std::left << std::setw(w) << "DeviceType:" << DeviceTypeToString(l_Prop.deviceType) << "\n";
		std::cout << std::left << std::setw(w) << "VendorID:" << l_Prop.vendorID << "\n";
		std::cout << std::left << std::setw(w) << "DriverVersion:" << l_Prop.driverVersion << "\n";
		std::cout << std::left << std::setw(w) << "Supported API-version:" << VlkUtil::SpecVersionToString(l_Prop.apiVersion) << "\n";
		//std::cout << std::left << std::setw(w) << "Limits:";
		std::cout << std::left << std::setw(w) << "PipelineCacheUUID:" << PipelineCacheUUIDToString(l_Prop.pipelineCacheUUID) << "\n";
		//std::cout << std::left << std::setw(w) << "SparseProperties:";
	}
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
}
}