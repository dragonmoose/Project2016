#include "pch.h"
#include "VlkSystem.h"
#include <unordered_map>

namespace Hawk {
namespace Gfx {
namespace VlkSystem
{
	using ResultMap_t = std::unordered_map<VkResult, std::string>;
	ResultMap_t n_ResultMap;
	const std::string n_DefaultResult("N/A");
	bool n_bInitialized = false;
	uint32_t n_uiAPIVersion = VK_MAKE_VERSION(1, 0, 3);

	void PopulateResultMap();
}

#define ADD_VK_RESULT(r) n_ResultMap[VK_##r] = #r 

void VlkSystem::Initialize()
{
	ASSERT(!n_bInitialized, "VlkSystem already initialized");
	PopulateResultMap();
	n_bInitialized = true;
}

uint32_t VlkSystem::GetAPIVersion()
{
	return n_uiAPIVersion;
}


const std::string& VlkSystem::ResultToString(VkResult p_Result)
{
	ASSERT(n_bInitialized, "VlkSystem not initialized");
	auto l_Itr = n_ResultMap.find(p_Result);
	if (l_Itr != n_ResultMap.end())
	{
		return l_Itr->second;
	}
	return n_DefaultResult;
}

void VlkSystem::PopulateResultMap()
{
	ADD_VK_RESULT(SUCCESS);
	ADD_VK_RESULT(NOT_READY);
	ADD_VK_RESULT(TIMEOUT);
	ADD_VK_RESULT(EVENT_SET);
	ADD_VK_RESULT(EVENT_RESET);
	ADD_VK_RESULT(INCOMPLETE);
	ADD_VK_RESULT(ERROR_OUT_OF_HOST_MEMORY);
	ADD_VK_RESULT(ERROR_OUT_OF_DEVICE_MEMORY);
	ADD_VK_RESULT(ERROR_INITIALIZATION_FAILED);
	ADD_VK_RESULT(ERROR_DEVICE_LOST);
	ADD_VK_RESULT(ERROR_MEMORY_MAP_FAILED);
	ADD_VK_RESULT(ERROR_LAYER_NOT_PRESENT);
	ADD_VK_RESULT(ERROR_EXTENSION_NOT_PRESENT);
	ADD_VK_RESULT(ERROR_FEATURE_NOT_PRESENT);
	ADD_VK_RESULT(ERROR_INCOMPATIBLE_DRIVER);
	ADD_VK_RESULT(ERROR_TOO_MANY_OBJECTS);
	ADD_VK_RESULT(ERROR_FORMAT_NOT_SUPPORTED);
	ADD_VK_RESULT(ERROR_SURFACE_LOST_KHR);
	ADD_VK_RESULT(ERROR_NATIVE_WINDOW_IN_USE_KHR);
	ADD_VK_RESULT(SUBOPTIMAL_KHR);
	ADD_VK_RESULT(ERROR_OUT_OF_DATE_KHR);
	ADD_VK_RESULT(ERROR_INCOMPATIBLE_DISPLAY_KHR);
	ADD_VK_RESULT(ERROR_VALIDATION_FAILED_EXT);
}

}
}

std::ostream& operator<<(std::ostream& os, VkExtent3D p_Extent)
{
	os << "(w:" << p_Extent.width << " h:" << p_Extent.height << " d:" << p_Extent.depth << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, Hawk::Gfx::VlkQueueType p_QueueType)
{
	switch (p_QueueType)
	{
		case Hawk::Gfx::VlkQueueType::Graphics:
			os << "'Graphics'";
			break;
		case Hawk::Gfx::VlkQueueType::Compute:
			os << "'Compute'";
			break;
		case Hawk::Gfx::VlkQueueType::Transfer:
			os << "'Transfer'";
			break;
		case Hawk::Gfx::VlkQueueType::SparseBinding:
			os << "'SparseBinding'";
			break;
		default:
			os << "[Invalid]";
	}
	return os;
}