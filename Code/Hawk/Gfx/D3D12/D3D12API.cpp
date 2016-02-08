#include "pch.h"

#ifdef HAWK_RENDERER_D3D12
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#endif

#include "D3D12API.h"
#include "BaseFactory.h"
#include "DeviceFactory.h"
#include "ClearRenderViewCL.h"
#include "Util.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

void D3D12API::Initialize()
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
	m_RenderView = std::make_unique<RenderView>(m_SwapChain, m_Device);

	std::unique_ptr<CommandList> l_ClearRenderViewCL = std::make_unique<ClearRenderViewCL>(m_Device, m_RenderView, m_CommandAllocator);
	m_CommandQueue->AddCommandList(l_ClearRenderViewCL);
	m_CommandLists.push_back(std::move(l_ClearRenderViewCL));
	//m_TextRenderer = std::make_unique<TextRenderer>(m_Device.Get(), reinterpret_cast<IUnknown**>(m_CommandQueue->GetD3DObject().GetAddressOf()), m_RenderView);

	SetFullscreenState(Config::Instance().Get("gfx.fullscreen", false));
}

void D3D12API::Render()
{
	BeginFrame();
	RecordCommands();
	m_CommandQueue->Execute();
	//m_TextRenderer->Render();
	EndFrame();
}

void D3D12API::SetFullscreenState(bool p_bState)
{
	m_RenderView->SetFullscreenState(p_bState); // Triggers WM_SIZE
}

void D3D12API::OnWindowSizeChanged(unsigned int p_uiWidth, unsigned int p_uiHeight)
{
	m_CommandQueue->WaitForGPU();
	//m_TextRenderer->ReleaseBackBufferResources();
	m_RenderView->OnWindowSizeChanged(p_uiWidth, p_uiHeight);
	//m_TextRenderer->CreateBackBufferResources();
}

void D3D12API::SetDebugText(const std::string& p_Text)
{
	//m_TextRenderer->SetText(p_Text);
}

#ifdef HAWK_DEBUG
void D3D12API::CmdListDevices()
{
	DeviceFactory::CmdListAdapters();
}
#endif

void D3D12API::CreateDevice(IDXGIFactory4* p_Factory)
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

void D3D12API::BeginFrame()
{
	m_CommandQueue->WaitForGPU();
	THROW_IF_COMERR(m_CommandAllocator->Reset(), "Failed to reset command allocator (GPU not finished with associated command lists?)");
	m_RenderView->BeginFrame();
}
void D3D12API::EndFrame()
{
	m_RenderView->EndFrame();
}

void D3D12API::RecordCommands()
{
	for (auto& l_CommandList : m_CommandLists)
	{
		l_CommandList->BeginRecord();
		l_CommandList->Record();
		l_CommandList->EndRecord();
	}
}

const std::string& D3D12API::GetLogDesc()
{
	static const std::string l_Desc("d3d");
	return l_Desc;
}

}
}
}