#pragma once
#include "System.h"
#include "System/Types.h"
#include <string>

namespace Hawk {
namespace Gfx {
namespace Vulkan {
namespace Util
{
	std::string SpecVersionToString(uint32 p_uiVersion);
	std::string PipelineCacheUUIDToString(const uint8* p_UUID);
	std::string DeviceTypeToString(VkPhysicalDeviceType p_Type);
	std::string QueueFlagsToString(VkQueueFlags p_Flags);
	std::string TimestampValidBitsToString(uint32 p_Bits);
	VkQueueFlags QueueTypeToFlag(QueueType p_Type);
}

}
}
}