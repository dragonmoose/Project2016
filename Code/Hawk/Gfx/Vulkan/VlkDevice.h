#pragma once
#include "VlkSystem.h"
#include "VlkTypes.h"
#include "VlkDeviceCreateInfo.h"
#include <vector>

namespace Hawk {
namespace Gfx {

class VlkDevice final
{
public:
	VlkDevice(const VlkDeviceCreateInfo& p_CreateInfo);
	~VlkDevice();
	VlkDevice(const VlkDevice&) = delete;
	VlkDevice& operator=(const VlkDevice&) = delete;

	void WaitUntilIdle();
	VkQueue GetQueue(VlkQueueType p_Type, uint32_t p_uiIndex) const;

#ifdef HAWK_DEBUG
	void CmdPrintDevices();
	void CmdPrintQueueFamilies(uint32_t p_uiDeviceIndex);
	void CmdPrintLayers(uint32_t p_uiDeviceIndex, bool p_bKeepUnsupported);
	void CmdPrintExtensions(uint32_t p_uiDeviceIndex, bool p_bKeepUnsupported);
#endif

private:
	struct QueueCreateInfo
	{
		QueueCreateInfo(uint32_t p_uiFamilyIndex, uint32_t p_uiTypeIndex, uint32_t p_uiQueueIndex, uint32_t p_uiPrio)
		: m_uiFamilyIndex(p_uiFamilyIndex)
		, m_uiTypeIndex(p_uiTypeIndex)
		, m_uiQueueIndex(p_uiQueueIndex)
		, m_uiPrio(p_uiPrio) {}

		uint32_t m_uiFamilyIndex;
		uint32_t m_uiTypeIndex;
		uint32_t m_uiQueueIndex;
		uint32_t m_uiPrio;
	};
	using QueueCreateInfoMap_t = std::unordered_map<VlkQueueType, std::vector<QueueCreateInfo>>;
	using Queues_t = std::unordered_map<VlkQueueType, std::vector<VkQueue>>;

	using Devices_t = std::vector<VkPhysicalDevice>;
	using QueueFamilyProperties_t = std::vector<VkQueueFamilyProperties>;
	using QueueFamilyCreateInfos_t = std::vector<VkDeviceQueueCreateInfo>;
	using LayerProperties_t = std::vector<VkLayerProperties>;
	using ExtensionProperties_t = std::vector<VkExtensionProperties>;

	static bool IsLayerAvailable(VkPhysicalDevice p_Device, const std::string& p_Name);
	static bool IsExtensionAvailable(VkPhysicalDevice p_Device, const std::string& p_Name, const std::string& p_LayerName = std::string());
	static void GetAllLayers(VkPhysicalDevice p_Device, LayerProperties_t& p_Layers, bool p_bKeepUnsupported);
	static void GetAllExtensions(VkPhysicalDevice p_Device, ExtensionProperties_t& p_Extensions, const std::string& p_LayerName = std::string());

	static void GetLayersToCreate(VkPhysicalDevice p_Device, std::vector<const char*>& p_Layers);
	static void GetExtensionsToCreate(VkPhysicalDevice p_Device, std::vector<const char*>& p_Extensions);

	void CreateDevice(const QueueFamilyCreateInfos_t& p_QueueFamilyCreateInfos);
	void ExtractQueues(const QueueCreateInfoMap_t& p_Vec);
	void Initialize();
	void Validate();
	static void ValidateQueueFamilyCreateInfos(const QueueFamilyCreateInfos_t& p_QueueFamilyCreateInfos);

	void GetDevices(Devices_t& p_Devices) const;
	VkPhysicalDevice GetDeviceByIndex(uint32_t p_uiIndex) const;
	VkPhysicalDevice GetDeviceByID(uint32_t p_uiDeviceID) const;
	void OnDeviceLost();

	static void GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties);
	static void GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties_t& p_Properties);
	void GetQueueCreateData(QueueFamilyCreateInfos_t& p_FamilyCreateInfos, QueueCreateInfoMap_t& p_QueueCreateMap);
	static void GetFeatures(VkPhysicalDeviceFeatures& p_Features);

	static std::string PipelineCacheUUIDToString(const uint8_t* p_UUID);
	static std::string DeviceTypeToString(VkPhysicalDeviceType p_Type);
	static std::string QueueFlagsToString(VkQueueFlags p_Flags);
	static std::string TimestampValidBitsToString(uint32_t p_Bits);
	static VkQueueFlags QueueTypeToFlag(VlkQueueType p_Type);

	VkInstance m_Instance;
	VkDevice m_Device;
	VkPhysicalDevice m_PhysicalDevice;
	Queues_t m_Queues;
	VlkQueueRequestMap_t m_QueueRequestMap;
};

}
}