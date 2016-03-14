#pragma once
#include "VlkSystem.h"
#include "VlkTypes.h"
#include <vector>

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
	VlkDeviceCreateInfo(VkInstance p_Instance, VkSurfaceKHR p_Surface);

	void SetDeviceID(uint32_t p_uiDeviceID);
	void AddQueue(VlkQueueType p_Type, uint32_t p_uiIndex, uint32_t p_uiPrio);
	const VlkQueueRequestMap_t& GetQueueRequestMap() const;
	bool UseDeviceID() const;
	uint32_t GetDeviceID() const;
	VkInstance GetInstance() const;
	VkSurfaceKHR GetSurface() const;
	void Finalize();
	bool IsFinalized() const;

private:
	VkSurfaceKHR m_Surface;
	VlkQueueRequestMap_t m_QueueRequestMap;
	VkInstance m_Instance;
	uint32_t m_uiDeviceID;
	bool m_bUseDeviceID;
	bool m_bFinalized;
};

}
}