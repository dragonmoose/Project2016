#include "pch.h"

#ifdef HAWK_RENDERER_D3D12
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#endif

#include "Renderer.h"
#include "BaseFactory.h"
#include "DeviceFactory.h"
#include "ClearRenderViewCL.h"
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
	BaseFactory::CreateSwapChain(l_Factory.Get(), m_CommandQueue->GetD3DObject(), m_SwapChain);
	BaseFactory::CreateCommandAllocator(m_Device.Get(), m_CommandAllocator);
	m_RenderView = std::make_unique<RenderView>(m_SwapChain, m_Device);
	SetFullscreen(Config::Instance().Get("gfx.fullscreen", false));

	std::unique_ptr<CommandList> l_ClearRenderViewCL = std::make_unique<ClearRenderViewCL>(m_Device, m_RenderView, m_CommandAllocator);
	m_CommandQueue->AddCommandList(l_ClearRenderViewCL);
	m_CommandLists.push_back(std::move(l_ClearRenderViewCL));

}

#ifdef HAWK_DEBUG
void Renderer::InitializeConsole()
{
	RegisterConsole("d3d.listAdapters", &DeviceFactory::CmdListAdapters, "Lists the available hardware adapters", "");
}
#endif

void Renderer::Update(const Duration& p_Duration)
{
	BeginFrame();
	RecordCommands();
	m_CommandQueue->Execute();
	EndFrame();
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

void Renderer::BeginFrame()
{
	m_CommandQueue->WaitForGPU();
	THROW_IF_COMERR(m_CommandAllocator->Reset(), "Failed to reset command allocator (GPU not finished with associated command lists?)");
	m_RenderView->BeginFrame();
}
void Renderer::EndFrame()
{
	m_RenderView->EndFrame();
}

void Renderer::RecordCommands()
{
	for (auto& l_CommandList : m_CommandLists)
	{
		l_CommandList->BeginRecord();
		l_CommandList->Record();
		l_CommandList->EndRecord();
	}
}

}
}
}