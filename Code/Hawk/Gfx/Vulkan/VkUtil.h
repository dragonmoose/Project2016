#pragma once
#include <vulkan/vulkan.h>
#include <string>

namespace Hawk {
namespace Gfx {
namespace VkUtil
{
	void Initialize();
	const std::string& ResultToString(VkResult p_Result);
}

}
}