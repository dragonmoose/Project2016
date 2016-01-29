#include "pch.h"

#ifdef HAWK_RENDERER_D3D12
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#endif

#include "Renderer.h"
#include "BaseFactory.h"
#include "DeviceFactory.h"
#include "Util.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

Renderer::Renderer()
{
}


std::string Renderer::GetName() const
{
	return "d3d";
}

void Renderer::Initialize()
{
	Util::EnableDebugLayer();

	DXGIFacoryComPtr_t l_Factory;
#ifdef HAWK_DEBUG
	THROW_IF_COMERR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&l_Factory)), "Failed to create DXGIFactory4");
#else
	THROW_IF_COMERR(CreateDXGIFactory1(IID_PPV_ARGS(&l_Factory)), "Failed to create DXGIFactory4");
#endif

	CreateDevice(l_Factory.Get());
	m_CommandQueue = std::make_unique<CommandQueue>(m_Device);
	BaseFactory::CreateSwapChain(l_Factory.Get(), m_CommandQueue->GetD3DObject().Get(), m_SwapChain);
	BaseFactory::CreateCommandAllocator(m_Device.Get(), m_CommandAllocator);
	m_BackBuffer = std::make_unique<BackBuffer>(m_SwapChain, m_Device);
	SetFullscreen(Config::Instance().Get("gfx.fullscreen", false));
}

#ifdef HAWK_DEBUG
void Renderer::InitializeConsole()
{
	RegisterConsole("d3d.listAdapters", &DeviceFactory::CmdListAdapters, "Lists the available hardware adapters", "");
}
#endif

void Renderer::Update(const Duration& p_Duration)
{
	m_BackBuffer->BeginFrame();
	m_BackBuffer->EndFrame();
	m_CommandQueue->WaitForGPU();
}

void Renderer::CreateDevice(IDXGIFactory4* p_Factory)
{
	if (Config::Instance().Get("gfx.preferSWRendering", false))
	{
		DeviceFactory::CreateWARPDevice(p_Factory, m_Device);
		LOGM("Created WARP device", Info);
	}
	else
	{
		std::string l_DeviceLuid = Config::Instance().Get<std::string>("gfx.deviceLuid", "");
		DeviceFactory::CreateDevice(p_Factory, l_DeviceLuid, m_Device);
		LOGM_IF(!l_DeviceLuid.empty(), "Created device from Luid: " << l_DeviceLuid, Info);
		LOGM_IF(l_DeviceLuid.empty(), "Created auto-selected device", Info);
	}
}

void Renderer::SetFullscreen(bool p_bValue)
{
	THROW_IF_COMERR(m_SwapChain->SetFullscreenState(p_bValue, nullptr), "Failed to set fullscreen state. Value=" << p_bValue);
}

}
}
}