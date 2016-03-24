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
	std::string DepthStencilFormatToString(VkFormat p_Format);
	std::string ColorFormatToString(VkFormat p_Format);

	const std::vector<VkFormat>& GetDepthStencilFormats();
	bool IsDepthStencilFormat(VkFormat p_Format);

	const std::vector<VkFormat>& GetColorFormats();
	bool IsColorFormat(VkFormat p_Format);
}

}
}
}