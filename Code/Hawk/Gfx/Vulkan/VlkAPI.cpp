#include "pch.h"
#include "VlkAPI.h"
#include "VlkDevice.h"
#include "VlkPhysicalDevice.h"
#include "VlkSystem.h"
#include "VlkSurface.h"
#include "Base/WindowManager.h"
#include "Console/ScopedConsoleCommands.h"

namespace Hawk {
namespace Gfx {

VlkAPI::~VlkAPI()
{
	m_Surface.reset();
	m_Device.reset();
	m_Instance.reset();
}

void VlkAPI::Initialize()
{
	VlkSystem::Initialize();
	CreateInstance();
	CreatePhysicalDevice();
	CreateWindowSurface();
	CreateDevice();
	//SetFullscreenState(Config::Instance().Get("gfx.fullscreen", false));
	LOG("Vulkan API initialized", "vulkan", Info);
}

void VlkAPI::Render()
{
	m_Device->WaitUntilIdle();
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
	p_Console->Register("vk.printInstanceLayers", &VlkInstance::CmdPrintLayers, "Lists Vulkan instance layers", "KeepUnsupported [0|1]");
	p_Console->Register("vk.printInstanceExtensions", &VlkInstance::CmdPrintExtensions, "Lists Vulkan instance extensions", "KeepUnsupported [0|1]");
	p_Console->Register("vk.printDevices", m_PhysicalDevice.get(), &VlkPhysicalDevice::CmdPrintDevices, "Lists all available devices", "");
	p_Console->Register("vk.printQueueFamilies", m_PhysicalDevice.get(), &VlkPhysicalDevice::CmdPrintQueueFamilies, "Lists queue families for the given device", "");
	p_Console->Register("vk.printDeviceLayers", m_PhysicalDevice.get(), &VlkPhysicalDevice::CmdPrintLayers, "Lists Vulkan device layers for the given device", "[DeviceIndex] [KeepUnsupported 0|1]");
	p_Console->Register("vk.printDeviceExtensions", m_PhysicalDevice.get(), &VlkPhysicalDevice::CmdPrintExtensions, "Lists Vulkan device extensions for the given device", "[DeviceIndex] [KeepUnsupported 0|1]");
}

void VlkAPI::CmdListDevices()
{
}
#endif

void VlkAPI::CreateInstance()
{
	m_Instance = std::make_shared<VlkInstance>();
}

void VlkAPI::CreatePhysicalDevice()
{
	uint32_t l_uiDeviceID = 0;
	if (Config::Instance().TryGet<uint32_t>("vulkan.deviceID", 0, l_uiDeviceID))
	{
		m_PhysicalDevice = std::make_shared<VlkPhysicalDevice>(m_Instance, l_uiDeviceID);
	}
	else
	{
		m_PhysicalDevice = std::make_shared<VlkPhysicalDevice>(m_Instance);
	}
}

void VlkAPI::CreateDevice()
{
	ASSERT(m_Instance, "Instance null");
	ASSERT(m_PhysicalDevice, "PhysicalDevice null");
	ASSERT(m_Surface, "Surface null");

	VlkDeviceCreateInfo l_Info(m_Instance, m_PhysicalDevice, m_Surface);
	l_Info.AddQueue(VlkQueueType::Graphics, 0, 100);
	l_Info.AddQueue(VlkQueueType::Graphics, 1, 100);
	l_Info.AddQueue(VlkQueueType::Graphics, 2, 150);
	l_Info.AddQueue(VlkQueueType::Graphics, 3, 50);
	l_Info.AddQueue(VlkQueueType::Compute, 1, 100);
	l_Info.AddQueue(VlkQueueType::Compute, 0, 25);
	l_Info.Finalize();
	m_Device = std::make_shared<VlkDevice>(l_Info);
}

void VlkAPI::CreateWindowSurface()
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
	m_Surface = std::make_shared<VlkSurface>(m_Instance->GetHandle(), WindowManager::GetHInstance(), WindowManager::GetHWND());
#endif
}

const std::string& VlkAPI::GetLogDesc()
{
	static const std::string l_Desc("vulkan");
	return l_Desc;
}

}
}