#pragma once
#include "System.h"
#include "Types.h"
#include "PhysicalDevice.h"
#include <vector>
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class DeviceCreateInfo final
{
public:
	DeviceCreateInfo(std::shared_ptr<PhysicalDevice> p_PhysicalDevice);
	DeviceCreateInfo(const DeviceCreateInfo&) = delete;
	DeviceCreateInfo& operator=(const DeviceCreateInfo&) = delete;

	struct QueueRequest
	{
		QueueRequest(uint32 p_uiIndex, uint32 p_uiPrio)
		: m_uiIndex(p_uiIndex)
		, m_uiPrio(p_uiPrio) {}

		uint32 m_uiIndex;
		uint32 m_uiPrio;
	};
	using QueueRequests = std::vector<QueueRequest>;
	using QueueRequestMap = std::unordered_map<QueueType, QueueRequests>;

	struct QueueCreateInfo
	{
		QueueCreateInfo(uint32 p_uiFamilyIndex, uint32 p_uiTypeIndex, uint32 p_uiQueueIndex, uint32 p_uiPrio)
			: m_uiFamilyIndex(p_uiFamilyIndex)
			, m_uiTypeIndex(p_uiTypeIndex)
			, m_uiQueueIndex(p_uiQueueIndex)
			, m_uiPrio(p_uiPrio) {}

		uint32 m_uiFamilyIndex;
		uint32 m_uiTypeIndex;
		uint32 m_uiQueueIndex;
		uint32 m_uiPrio;
	};
	using QueueCreateInfoMap = std::unordered_map<QueueType, std::vector<QueueCreateInfo>>;

	void AddQueue(QueueType p_Type, uint32 p_uiIndex, uint32 p_uiPrio);
	const DeviceCreateInfo::QueueCreateInfoMap& GetQueueCreateInfoMap() const;
	std::shared_ptr<PhysicalDevice> GetPhysicalDevice() const;
	void Finalize();
	bool IsFinalized() const;
	void SortAndValidateQueueRequests();
	void SetupQueueCreateInfoMap();

private:
	QueueRequestMap m_QueueRequestMap;
	QueueCreateInfoMap m_QueueCreateInfoMap;
	std::shared_ptr<PhysicalDevice> m_PhysicalDevice;
	bool m_bFinalized;
};

}
}
}