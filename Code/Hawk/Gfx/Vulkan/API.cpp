#include "pch.h"
#include "API.h"
#include "Device.h"
#include "PhysicalDevice.h"
#include "System.h"
#include "WindowSurface.h"
#include "Console/ScopedConsoleCommands.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

API::~API()
{
	LOG("API destroyed", "vulkan", Debug);
}

void API::Initialize()
{
	System::Initialize();
	CreateInstance();
	CreatePhysicalDevice();

	DeviceCreateInfo l_CreateInfo(m_PhysicalDevice);
	SetupQueues(l_CreateInfo);
	l_CreateInfo.Finalize();

	CreateDevice(l_CreateInfo);
	CreateSwapchain();

	m_CommandPool = std::make_shared<CommandPool>(m_Device, m_Device->GetPresentationQueue()->GetFamilyIndex());
	
	std::shared_ptr<CommandBuffer> l_Buffer = m_CommandPool->CreateBuffer();
	l_Buffer->Reset(false);

	//SetFullscreenState(Config::Instance().Get("gfx.fullscreen", false));
	LOG("Vulkan initialized", "vulkan", Info);
}

void API::Render()
{
	m_Device->WaitUntilIdle();
	m_Swapchain->Present();
}

void API::SetFullscreenState(bool /*p_bState*/)
{
}

void API::OnWindowSizeChanged(UINT32 /*p_uiWidth*/, UINT32 /*p_uiHeight*/)
{
}

void API::SetDebugText(const std::string& /*p_Text*/)
{
}

#ifdef HAWK_DEBUG
void API::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("vk.printInstanceLayers", &Instance::CmdPrintLayers, "Lists Vulkan instance layers", "KeepUnsupported [0|1]");
	p_Console->Register("vk.printInstanceExtensions", &Instance::CmdPrintExtensions, "Lists Vulkan instance extensions", "KeepUnsupported [0|1]");
	p_Console->Register("vk.printDevices", m_PhysicalDevice.get(), &PhysicalDevice::CmdPrintDevices, "Lists all available devices", "");
	p_Console->Register("vk.printQueueFamilies", m_PhysicalDevice.get(), &PhysicalDevice::CmdPrintQueueFamilies, "Lists queue families for the given device", "");
	p_Console->Register("vk.printDeviceLayers", m_PhysicalDevice.get(), &PhysicalDevice::CmdPrintLayers, "Lists Vulkan device layers for the given device", "[DeviceIndex] [KeepUnsupported 0|1]");
	p_Console->Register("vk.printDeviceExtensions", m_PhysicalDevice.get(), &PhysicalDevice::CmdPrintExtensions, "Lists Vulkan device extensions for the given device", "[DeviceIndex] [KeepUnsupported 0|1]");
}

#endif

void API::CreateInstance()
{
	m_Instance = std::make_shared<Instance>();
}

void API::CreatePhysicalDevice()
{
	uint32 l_uiDeviceID = 0;
	if (Config::Instance().TryGet<uint32>("vulkan.deviceID", 0, l_uiDeviceID))
	{
		m_PhysicalDevice = std::make_shared<PhysicalDevice>(m_Instance, l_uiDeviceID);
	}
	else
	{
		m_PhysicalDevice = std::make_shared<PhysicalDevice>(m_Instance);
	}
}

void API::SetupQueues(DeviceCreateInfo& p_CreateInfo)
{
	p_CreateInfo.AddQueue(QueueType::GraphicsPresentation, 0, 100);
	p_CreateInfo.AddQueue(QueueType::Graphics, 0, 100);
	p_CreateInfo.AddQueue(QueueType::Graphics, 1, 150);
	p_CreateInfo.AddQueue(QueueType::Graphics, 2, 50);
	p_CreateInfo.AddQueue(QueueType::Compute, 1, 100);
	p_CreateInfo.AddQueue(QueueType::Compute, 0, 25);
}

void API::CreateDevice(const DeviceCreateInfo& p_CreateInfo)
{
	ASSERT(m_Instance, "Instance null");
	ASSERT(m_PhysicalDevice, "PhysicalDevice null");
	m_Device = std::make_shared<Device>(p_CreateInfo);
}

void API::CreateSwapchain()
{
	m_Swapchain = std::make_shared<Swapchain>(m_Instance, m_Device);
}

const std::string& API::GetLogDesc()
{
	static const std::string l_Desc("vulkan");
	return l_Desc;
}

}
}
}