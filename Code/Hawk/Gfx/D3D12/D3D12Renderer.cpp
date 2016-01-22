#include "pch.h"

#ifdef HAWK_RENDERER_D3D12
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#endif

#include "D3D12Renderer.h"
#include "D3D12BaseFactory.h"
#include "D3D12DeviceFactory.h"
#include "D3D12Util.h"

namespace Hawk {
namespace Gfx {

std::string D3D12Renderer::GetName() const
{
	return "d3d";
}

void D3D12Renderer::Initialize()
{
	D3D12Util::EnableDebugLayer();

	ComPtr<IDXGIFactory4> l_Factory;
	THROW_IF_COMERR(CreateDXGIFactory1(IID_PPV_ARGS(&l_Factory)), "Failed to create DXGIFactory4");

	CreateDevice(l_Factory.Get());
	m_CommandQueue = D3D12BaseFactory::CreateCommandQueue(m_Device.Get());
	m_SwapChain = D3D12BaseFactory::CreateSwapChain(l_Factory.Get(), m_CommandQueue.Get());
}

#ifdef HAWK_DEBUG
void D3D12Renderer::InitializeConsole()
{
	RegisterConsole("d3d.listAdapters", &D3D12DeviceFactory::CmdListAdapters, "Lists the available hardware adapters", "");
}
#endif

void D3D12Renderer::CreateDevice(IDXGIFactory4* p_Factory)
{
	if (Config::Instance().Get("gfx.preferSWRendering", false))
	{
		m_Device = D3D12DeviceFactory::CreateWARPDevice(p_Factory);
		LOGM("Created WARP device", Info);
	}
	else
	{
		std::string l_DeviceLuid = Config::Instance().Get<std::string>("gfx.deviceLuid", "");
		m_Device = D3D12DeviceFactory::CreateDevice(p_Factory, l_DeviceLuid);
		LOGM_IF(!l_DeviceLuid.empty(), "Created device from Luid: " << l_DeviceLuid, Info);
		LOGM_IF(l_DeviceLuid.empty(), "Created auto-selected device", Info);
	}
}

}
}