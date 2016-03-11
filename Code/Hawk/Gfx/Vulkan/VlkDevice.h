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

	void WaitUntilIdle();

#ifdef HAWK_DEBUG
	void CmdPrintDevices();
	void CmdPrintQueueFamilies(uint32_t p_uiDeviceIndex);
	void CmdPrintLayers(uint32_t p_uiDeviceIndex, bool p_bKeepUnsupported);
	void CmdPrintExtensions(uint32_t p_uiDeviceIndex, bool p_bKeepUnsupported);
#endif

private:
	using Devices_t = std::vector<VkPhysicalDevice>;
	using QueueFamilyProperties_t = std::vector<VkQueueFamilyProperties>;
	using QueueCreateInfoVec_t = std::vector<VkDeviceQueueCreateInfo>;
	using LayerProperties_t = std::vector<VkLayerProperties>;
	using ExtensionProperties_t = std::vector<VkExtensionProperties>;

	static bool IsLayerAvailable(VkPhysicalDevice p_Device, const std::string& p_Name);
	static bool IsExtensionAvailable(VkPhysicalDevice p_Device, const std::string& p_Name, const std::string& p_LayerName = std::string());
	static void GetAllLayers(VkPhysicalDevice p_Device, LayerProperties_t& p_Layers, bool p_bKeepUnsupported);
	static void GetAllExtensions(VkPhysicalDevice p_Device, ExtensionProperties_t& p_Extensions, const std::string& p_LayerName = std::string());

	static void GetLayersToCreate(VkPhysicalDevice p_Device, std::vector<const char*>& p_Layers);
	static void GetExtensionsToCreate(VkPhysicalDevice p_Device, std::vector<const char*>& p_Extensions);

	static QueueCreateInfoVec_t GetQueueCreateInfo(VkPhysicalDevice p_Device);
	void CreateDevice(VkPhysicalDevice p_Device, const QueueCreateInfoVec_t& p_QueueCreateInfoVec);
	void CreateDevice(VkPhysicalDevice p_Device);
	static void ValidateQueueCreateInfoVec(const QueueCreateInfoVec_t& p_QueueCreateInfoVec);

	void GetDevices(Devices_t& p_Devices) const;
	VkPhysicalDevice GetDeviceByIndex(uint32_t p_uiIndex) const;
	VkPhysicalDevice GetDeviceByID(uint32_t p_uiDeviceID) const;
	void OnDeviceLost();

	static void GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties);
	static void GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties_t& p_Properties);
	static void FindMatchingQueueFamily(const VkPhysicalDevice p_Device, VkQueueFlags p_Flags, uint32_t p_uiCount, uint32_t& p_uiIndex);
	static void GetFeatures(VkPhysicalDeviceFeatures& p_Features);

	static std::string PipelineCacheUUIDToString(const uint8_t* p_UUID);
	static std::string DeviceTypeToString(VkPhysicalDeviceType p_Type);
	static std::string QueueFlagsToString(VkQueueFlags p_Flags);
	static std::string TimestampValidBitsToString(uint32_t p_Bits);

	VkInstance m_Instance;
	VkDevice m_Device;
	VkPhysicalDevice m_PhysicalDevice;
};

}
}