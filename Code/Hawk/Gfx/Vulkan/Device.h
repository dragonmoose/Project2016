#pragma once
#include "System.h"
#include "DeviceCreateInfo.h"
#include "Queue.h"
#include <vector>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device final
{
public:
	Device(const DeviceCreateInfo& p_CreateInfo);
	~Device();
	Device(const Device&) = delete;
	Device& operator=(const Device&) = delete;

	void WaitUntilIdle();
	std::shared_ptr<Queue> GetQueue(QueueType p_Type, uint32 p_uiIndex) const;
	std::shared_ptr<Queue> GetPresentationQueue() const;
	std::shared_ptr<PhysicalDevice> GetPhysicalDevice() const;
	VkDevice GetHandle() const;

private:
	using Queues = std::unordered_map<QueueType, std::vector<std::shared_ptr<Queue>>>;
	using QueueFamilyCreateInfos = std::vector<VkDeviceQueueCreateInfo>;

	void GetLayersToCreate(std::vector<const char*>& p_Layers);
	void GetExtensionsToCreate(std::vector<const char*>& p_Extensions);

	void CreateDevice(const QueueFamilyCreateInfos& p_QueueFamilyCreateInfoMap);
	void ExtractQueues(const DeviceCreateInfo::QueueCreateInfoMap& p_QueueCreateInfoMap);
	void Initialize(const DeviceCreateInfo& p_CreateInfo);
	void Validate();
	void ValidateQueueFamilyCreateInfos(const QueueFamilyCreateInfos& p_QueueFamilyCreateInfos);
	void OnDeviceLost();

	void GetQueueFamilyCreateInfos(const DeviceCreateInfo::QueueCreateInfoMap& p_QueueCreateInfoMap, QueueFamilyCreateInfos& p_FamilyCreateInfos);
	void GetFeatures(VkPhysicalDeviceFeatures& p_Features);

	std::shared_ptr<PhysicalDevice> m_PhysicalDevice;
	VkDevice m_Handle;

	Queues m_Queues;
	DeviceCreateInfo::QueueRequestMap m_QueueRequestMap;
};

}
}
}