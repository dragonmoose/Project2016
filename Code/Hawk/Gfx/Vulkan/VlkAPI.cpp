#include "pch.h"
#include "VlkAPI.h"
#include "VlkSystem.h"
#include "VlkInstance.h"
#include "VlkInstanceUtil.h"
#include "Console/ScopedConsoleCommands.h"

namespace Hawk {
namespace Gfx {

void VlkAPI::Initialize()
{
	VlkSystem::Initialize();
	VlkInstance l_Instance;
	//SetFullscreenState(Config::Instance().Get("gfx.fullscreen", false));
	LOG("Vulkan API initialized", "vulkan", Info);
}

void VlkAPI::Render()
{
}

void VlkAPI::SetFullscreenState(bool /*p_bState*/)
{
}

void VlkAPI::OnWindowSizeChanged(UINT32 /*p_uiWidth*/, UINT32 /*p_uiHeight*/)
{
}

void VlkAPI::SetDebugText(const std::string& /*p_Text*/)
{
}

#ifdef HAWK_DEBUG
void VlkAPI::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("vk.printInstanceLayers", &VlkInstanceUtil::CmdPrintLayers, "Lists Vulkan instance layers", "KeepUnsupported [0|1]", false);
	p_Console->Register("vk.printInstanceExtensions", &VlkInstanceUtil::CmdPrintExtensions, "Lists Vulkan instance extensions", "KeepUnsupported [0|1]", false);
}

void VlkAPI::CmdListDevices()
{
}
#endif

const std::string& VlkAPI::GetLogDesc()
{
	static const std::string l_Desc("vulkan");
	return l_Desc;
}

}
}