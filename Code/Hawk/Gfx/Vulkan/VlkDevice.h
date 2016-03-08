#pragma once
#include "VlkSystem.h"
#include <vector>

namespace Hawk {
namespace Gfx {

class VlkDevice final
{
public:
	VlkDevice(VkInstance p_Instance);
	VlkDevice(VkInstance p_Instance, uint32_t p_uiDeviceID);
	~VlkDevice();
	VlkDevice(const VlkDevice&) = delete;
	VlkDevice& operator=(const VlkDevice&) = delete;

	void CmdPrintDevices();

private:
	using Devices_t = std::vector<VkPhysicalDevice>;
	using QueueFamilyProperties_t = std::vector<VkQueueFamilyProperties>;

	void CreateDevice(VkPhysicalDevice p_Device);
	void GetDevices(Devices_t& p_Devices) const;
	void GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties) const;
	void GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties_t& p_Properties) const;
	std::string PipelineCacheUUIDToString(const uint8_t* p_UUID) const;
	std::string DeviceTypeToString(VkPhysicalDeviceType p_Type) const;
	std::string QueueFlagsToString(VkQueueFlags p_Flags);
	std::string TimestampValidBitsToString(uint32_t p_Bits);

	VkInstance m_Instance;

#ifdef HAWK_DEBUG
#endif
};

}
}