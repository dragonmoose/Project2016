#pragma once
#include "System/Exception.h"
#include <vulkan/vulkan.h>

namespace Hawk {
namespace Gfx {
namespace VlkSystem
{
	void Initialize();
	uint32_t GetAPIVersion();
	const std::string& ResultToString(VkResult p_Result);
}
}
}

#define VK_THROW(p, msg) THROW(msg << " [VkResult: " << Hawk::Gfx::VlkSystem::ResultToString(p) << "]")
#define VK_THROW_IF_ERR(p, msg)	if ((p < 0)) VK_THROW(p, msg)
#define VK_THROW_IF_NOT_SUCCESS(p, msg) if ((p != 0)) VK_THROW(p, msg)