#pragma once
#include "VlkSystem.h"
#include <stdint.h>
#include <string>

namespace Hawk {
namespace Gfx {
namespace VlkUtil
{
	std::string SpecVersionToString(uint32_t p_uiVersion);
	std::string PipelineCacheUUIDToString(const uint8_t* p_UUID);
	std::string DeviceTypeToString(VkPhysicalDeviceType p_Type);
	std::string QueueFlagsToString(VkQueueFlags p_Flags);
	std::string TimestampValidBitsToString(uint32_t p_Bits);
	VkQueueFlags QueueTypeToFlag(VlkQueueType p_Type);
}

}
}
