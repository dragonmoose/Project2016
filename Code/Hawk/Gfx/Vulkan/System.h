#pragma once
#include "Types.h"
#include "System/Exception.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Queue;
class CommandBuffer;
class CommandBufferPool;

namespace System
{
	void Initialize();
	uint32 GetAPIVersion();
	const std::string& ResultToString(VkResult p_Result);
}
}
}
}

#define VK_THROW(p, msg) THROW(msg << " [VkResult: " << Hawk::Gfx::Vulkan::System::ResultToString(p) << "]")
#define VK_THROW_IF_ERR(p, msg)	if ((p < 0)) VK_THROW(p, msg)
#define VK_THROW_IF_NOT_SUCCESS(p, msg) if ((p != 0)) VK_THROW(p, msg)

std::ostream& operator<<(std::ostream& os, VkExtent3D p_Extent);
std::ostream& operator<<(std::ostream& os, Hawk::Gfx::Vulkan::QueueType p_QueueType);

#define HAWK_VULKAN_ENABLE_VALIDATION