#pragma once
#include "VlkSystem.h"
#include "VlkDeviceCreateInfo.h"
#include "VlkQueue.h"
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
	std::shared_ptr<VlkQueue> GetQueue(VlkQueueType p_Type, uint32 p_uiIndex) const;
	std::shared_ptr<VlkQueue> GetPresentationQueue() const;
	std::shared_ptr<VlkPhysicalDevice> GetPhysicalDevice() const;
	VkDevice GetHandle() const;

private:
	using Queues = std::unordered_map<VlkQueueType, std::vector<std::shared_ptr<VlkQueue>>>;
	using QueueFamilyCreateInfos = std::vector<VkDeviceQueueCreateInfo>;

	void GetLayersToCreate(std::vector<const char*>& p_Layers);
	void GetExtensionsToCreate(std::vector<const char*>& p_Extensions);

	void CreateDevice(const QueueFamilyCreateInfos& p_QueueFamilyCreateInfoMap);
	void ExtractQueues(const VlkDeviceCreateInfo::QueueCreateInfoMap& p_QueueCreateInfoMap);
	void Initialize(const VlkDeviceCreateInfo& p_CreateInfo);
	void Validate();
	void ValidateQueueFamilyCreateInfos(const QueueFamilyCreateInfos& p_QueueFamilyCreateInfos);
	void OnDeviceLost();

	void GetQueueFamilyCreateInfos(const VlkDeviceCreateInfo::QueueCreateInfoMap& p_QueueCreateInfoMap, QueueFamilyCreateInfos& p_FamilyCreateInfos);
	void GetFeatures(VkPhysicalDeviceFeatures& p_Features);

	std::shared_ptr<VlkPhysicalDevice> m_PhysicalDevice;
	VkDevice m_Device;

	Queues m_Queues;
	VlkDeviceCreateInfo::QueueRequestMap m_QueueRequestMap;
};

}
}