#pragma once
#include "VlkSystem.h"
#include "VlkTypes.h"
#include "VlkPhysicalDevice.h"
#include <vector>
#include <memory>

namespace Hawk {
namespace Gfx {

class VlkDeviceCreateInfo final
{
public:
	VlkDeviceCreateInfo(std::shared_ptr<VlkPhysicalDevice> p_PhysicalDevice);
	VlkDeviceCreateInfo(const VlkDeviceCreateInfo&) = delete;
	VlkDeviceCreateInfo& operator=(const VlkDeviceCreateInfo&) = delete;

	struct QueueRequest
	{
		QueueRequest(uint32_t p_uiIndex, uint32_t p_uiPrio)
		: m_uiIndex(p_uiIndex)
		, m_uiPrio(p_uiPrio) {}

		uint32_t m_uiIndex;
		uint32_t m_uiPrio;
	};
	using QueueRequests_t = std::vector<QueueRequest>;
	using QueueRequestMap_t = std::unordered_map<VlkQueueType, QueueRequests_t>;

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

	void AddQueue(VlkQueueType p_Type, uint32_t p_uiIndex, uint32_t p_uiPrio);
	const VlkDeviceCreateInfo::QueueCreateInfoMap_t& GetQueueCreateInfoMap() const;
	std::shared_ptr<VlkPhysicalDevice> GetPhysicalDevice() const;
	void Finalize();
	bool IsFinalized() const;
	void SortAndValidateQueueRequests();
	void SetupQueueCreateInfoMap();

private:
	QueueRequestMap_t m_QueueRequestMap;
	QueueCreateInfoMap_t m_QueueCreateInfoMap;
	std::shared_ptr<VlkPhysicalDevice> m_PhysicalDevice;
	bool m_bFinalized;
};

}
}