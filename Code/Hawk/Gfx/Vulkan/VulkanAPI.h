#pragma once
#include "Gfx/IRenderingAPI.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class VulkanAPI final : public IRenderingAPI
{
public:
	VulkanAPI() = default;

	void Initialize() override;
	void Render() override;
	void SetFullscreenState(bool p_bState) override;
	void OnWindowSizeChanged(UINT32 p_uiWidth, UINT32 p_uiHeight);
	void SetDebugText(const std::string& p_Text) override;

#ifdef HAWK_DEBUG
	void CmdListDevices() override;
#endif

private:
	static const std::string& GetLogDesc();
};

}
}
}