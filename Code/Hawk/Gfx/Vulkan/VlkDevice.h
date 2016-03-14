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
	using QueueFamilyCreateInfos_t = std::vector<VkDeviceQueueCreateInfo>;

	void GetLayersToCreate(std::vector<const char*>& p_Layers);
	void GetExtensionsToCreate(std::vector<const char*>& p_Extensions);

	void CreateDevice(const QueueFamilyCreateInfos_t& p_QueueFamilyCreateInfos);
	void ExtractQueues(const QueueCreateInfoMap_t& p_Vec);
	void Initialize();
	void Validate();
	void ValidateQueueFamilyCreateInfos(const QueueFamilyCreateInfos_t& p_QueueFamilyCreateInfos);
	void OnDeviceLost();

	void GetQueueFamilyCreateInfos(const QueueCreateInfoMap_t& p_QueueCreateMap, QueueFamilyCreateInfos_t& p_FamilyCreateInfos);
	void GetQueueCreateInfoMap(QueueCreateInfoMap_t& p_QueueCreateMap);
	void GetFeatures(VkPhysicalDeviceFeatures& p_Features);
	void CheckWSISupport(const QueueCreateInfoMap_t& p_QueueCreateMap);

	std::shared_ptr<VlkInstance> m_Instance;
	std::shared_ptr<VlkPhysicalDevice> m_PhysicalDevice;
	std::shared_ptr<VlkSurface> m_Surface;
	VkDevice m_Device;

	Queues_t m_Queues;
	VlkQueueRequestMap_t m_QueueRequestMap;
};

}
}