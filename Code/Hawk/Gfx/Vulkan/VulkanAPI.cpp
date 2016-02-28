#include "pch.h"

#ifdef HAWK_RENDERER_VULKAN
//#pragma comment(lib, "d3d12")
#endif

#include "VulkanAPI.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

void VulkanAPI::Initialize()
{
	//SetFullscreenState(Config::Instance().Get("gfx.fullscreen", false));
}

void VulkanAPI::Render()
{
}

void VulkanAPI::SetFullscreenState(bool p_bState)
{
}

void VulkanAPI::OnWindowSizeChanged(UINT32 p_uiWidth, UINT32 p_uiHeight)
{
}

void VulkanAPI::SetDebugText(const std::string& p_Text)
{
}

#ifdef HAWK_DEBUG
void VulkanAPI::CmdListDevices()
{
}
#endif

const std::string& VulkanAPI::GetLogDesc()
{
	static const std::string l_Desc("vulkan");
	return l_Desc;
}

}
}
}