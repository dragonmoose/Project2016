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
	using QueueCreateInfoVec_t = std::vector<VkDeviceQueueCreateInfo>;

	QueueCreateInfoVec_t GetQueueCreateInfo(VkPhysicalDevice p_Device) const;
	void CreateDevice(VkPhysicalDevice p_Device, const QueueCreateInfoVec_t& p_QueueCreateInfoVec);
	void ValidateQueueCreateInfoVec(const QueueCreateInfoVec_t& p_QueueCreateInfoVec) const;
	void GetDevices(Devices_t& p_Devices) const;
	void GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties) const;
	void GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties_t& p_Properties) const;
	void FindMatchingQueueFamily(const VkPhysicalDevice p_Device, VkQueueFlags p_Flags, uint32_t p_uiCount, uint32_t& p_uiIndex) const;
	VkPhysicalDevice GetByDeviceID(uint32_t p_uiDeviceID) const;
	std::string PipelineCacheUUIDToString(const uint8_t* p_UUID) const;
	std::string DeviceTypeToString(VkPhysicalDeviceType p_Type) const;
	std::string QueueFlagsToString(VkQueueFlags p_Flags) const;
	std::string TimestampValidBitsToString(uint32_t p_Bits) const;

	VkInstance m_Instance;
	VkDevice m_Device;

#ifdef HAWK_DEBUG
#endif
};

}
}