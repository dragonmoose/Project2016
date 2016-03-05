#include "pch.h"
#include "VkAPI.h"
#include "VkSystem.h"
#include "VkInstance.h"
#include "VkUtil.h"
#include "Console/ScopedConsoleCommands.h"

namespace Hawk {
namespace Gfx {

void VkAPI::Initialize()
{
	VkSystem::Initialize();
	VkUtil::Initialize();
	VkInstance l_Instance;
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
void VkAPI::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("vk.availableInstanceLayers", &VkUtil::PrintAvailableInstanceLayers, "Lists the available Vulkan instance layers", "");
}

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