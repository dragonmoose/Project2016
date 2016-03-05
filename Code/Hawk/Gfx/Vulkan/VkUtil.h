#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Hawk {
namespace Gfx {
namespace VkUtil
{
	void Initialize();
	bool IsInstanceLayerAvailable(const std::string& p_Name);
	bool IsInstanceExtensionAvailable(const std::string& p_Name);

	void PrintAvailableInstanceLayers();
}

}
}