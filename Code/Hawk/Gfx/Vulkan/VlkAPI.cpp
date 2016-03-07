#include "pch.h"
#include "VlkAPI.h"
#include "VlkSystem.h"
#include "VlkInstanceUtil.h"
#include "Console/ScopedConsoleCommands.h"

namespace Hawk {
namespace Gfx {

void VlkAPI::Initialize()
{
	VlkSystem::Initialize();
	m_Instance = std::make_unique<VlkInstance>();
	CreateDevice();
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
	//p_Console->Register("vk.printDevices", &VlkDeviceUtil::CmdPrintDevices, "Lists Vulkan instance extensions", "");
}

void VlkAPI::CmdListDevices()
{
}
#endif

void VlkAPI::CreateDevice()
{
	uint32_t l_uiDeviceID;
	if (Config::Instance().TryGet<uint32_t>("vulkan.deviceID", 0, l_uiDeviceID))
	{
		m_Device = std::make_unique<VlkDevice>(m_Instance->GetHandle(), l_uiDeviceID);
	}
	else
	{
		m_Device = std::make_unique<VlkDevice>(m_Instance->GetHandle());
	}
}

const std::string& VlkAPI::GetLogDesc()
{
	static const std::string l_Desc("vulkan");
	return l_Desc;
}

}
}