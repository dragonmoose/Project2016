#include "pch.h"
#include "VkUtil.h"

#ifdef HAWK_RENDERER_VULKAN
#pragma comment(lib, "d3d12")
#endif

#include "VkAPI.h"

namespace Hawk {
namespace Gfx {

void VkAPI::Initialize()
{
	VkUtil::Initialize();
	//SetFullscreenState(Config::Instance().Get("gfx.fullscreen", false));
	LOG("Vulkan API initialized", "vulkan", Info);
}

void VkAPI::Render()
{
}

void VkAPI::SetFullscreenState(bool p_bState)
{
}

void VkAPI::OnWindowSizeChanged(UINT32 p_uiWidth, UINT32 p_uiHeight)
{
}

void VkAPI::SetDebugText(const std::string& p_Text)
{
}

#ifdef HAWK_DEBUG
void VkAPI::CmdListDevices()
{
}
#endif

const std::string& VkAPI::GetLogDesc()
{
	static const std::string l_Desc("vulkan");
	return l_Desc;
}

}
}