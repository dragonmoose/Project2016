#include "pch.h"
#include "VkSystem.h"
#include <unordered_map>

namespace Hawk {
namespace Gfx {
namespace VkSystem
{
	using ResultMap_t = std::unordered_map<VkResult, std::string>;
	ResultMap_t n_ResultMap;
	const std::string n_DefaultResult("N/A");
	bool n_bInitialized = false;

	void PopulateResultMap();
}

#define ADD_VK_RESULT(r) n_ResultMap[VK_##r] = #r 

void VkSystem::Initialize()
{
	ASSERT(!n_bInitialized, "VkSystem already initialized");
	PopulateResultMap();
	n_bInitialized = true;
}

const std::string& VkSystem::ResultToString(VkResult p_Result)
{
	ASSERT(n_bInitialized, "VkSystem not initialized");
	auto l_Itr = n_ResultMap.find(p_Result);
	if (l_Itr != n_ResultMap.end())
	{
		return l_Itr->second;
	}
	return n_DefaultResult;
}

void VkSystem::PopulateResultMap()
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