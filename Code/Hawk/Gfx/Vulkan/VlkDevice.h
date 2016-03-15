#pragma once
#include "VlkSystem.h"
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
	std::shared_ptr<VlkPhysicalDevice> GetPhysicalDevice() const;
	VkDevice GetHandle() const;

private:
	using Queues_t = std::unordered_map<VlkQueueType, std::vector<VkQueue>>;
	using QueueFamilyCreateInfos_t = std::vector<VkDeviceQueueCreateInfo>;

	void GetLayersToCreate(std::vector<const char*>& p_Layers);
	void GetExtensionsToCreate(std::vector<const char*>& p_Extensions);

	void CreateDevice(const QueueFamilyCreateInfos_t& p_QueueFamilyCreateInfoMap);
	void ExtractQueues(const VlkDeviceCreateInfo::QueueCreateInfoMap_t& p_QueueCreateInfoMap);
	void Initialize(const VlkDeviceCreateInfo& p_CreateInfo);
	void Validate();
	void ValidateQueueFamilyCreateInfos(const QueueFamilyCreateInfos_t& p_QueueFamilyCreateInfos);
	void OnDeviceLost();

	void GetQueueFamilyCreateInfos(const VlkDeviceCreateInfo::QueueCreateInfoMap_t& p_QueueCreateInfoMap, QueueFamilyCreateInfos_t& p_FamilyCreateInfos);
	void GetFeatures(VkPhysicalDeviceFeatures& p_Features);

	std::shared_ptr<VlkPhysicalDevice> m_PhysicalDevice;
	VkDevice m_Device;

	Queues_t m_Queues;
	VlkDeviceCreateInfo::QueueRequestMap_t m_QueueRequestMap;
};

}
}