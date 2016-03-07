#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Hawk {
namespace Gfx {
namespace VlkInstanceUtil
{
	bool IsLayerAvailable(const std::string& p_Name);
	bool IsExtensionAvailable(const std::string& p_Name, const std::string& p_LayerName = std::string());

	void CmdPrintLayers(bool p_bKeepUnsupported);
	void CmdPrintExtensions(bool p_bKeepUnsupported);
}

}
}