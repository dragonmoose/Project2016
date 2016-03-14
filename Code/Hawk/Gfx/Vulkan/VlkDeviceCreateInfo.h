#pragma once
#include "VlkSystem.h"
#include "VlkTypes.h"
#include "VlkInstance.h"
#include "VlkSurface.h"
#include "VlkPhysicalDevice.h"
#include <vector>
#include <memory>

namespace Hawk {
namespace Gfx {

struct VlkQueueRequest
{
	VlkQueueRequest(uint32_t p_uiIndex, uint32_t p_uiPrio)
	: m_uiIndex(p_uiIndex)
	, m_uiPrio(p_uiPrio) {}

	uint32_t m_uiIndex;
	uint32_t m_uiPrio;
};
using VlkQueueRequests_t = std::vector<VlkQueueRequest>;
using VlkQueueRequestMap_t = std::unordered_map<VlkQueueType, VlkQueueRequests_t>;

class VlkDeviceCreateInfo final
{
public:
	VlkDeviceCreateInfo(std::shared_ptr<VlkInstance> p_Instance, std::shared_ptr<VlkPhysicalDevice> p_PhysicalDevice, std::shared_ptr<VlkSurface> p_Surface);

	void AddQueue(VlkQueueType p_Type, uint32_t p_uiIndex, uint32_t p_uiPrio);
	const VlkQueueRequestMap_t& GetQueueRequestMap() const;
	std::shared_ptr<VlkInstance> GetInstance() const;
	std::shared_ptr<VlkPhysicalDevice> GetPhysicalDevice() const;
	std::shared_ptr<VlkSurface> GetSurface() const;
	void Finalize();
	bool IsFinalized() const;

private:
	VlkQueueRequestMap_t m_QueueRequestMap;
	std::shared_ptr<VlkSurface> m_Surface;
	std::shared_ptr<VlkInstance> m_Instance;
	std::shared_ptr<VlkPhysicalDevice> m_PhysicalDevice;
	bool m_bFinalized;
};

}
}