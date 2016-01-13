#include "pch.h"

#ifdef HAWK_RENDERER_D3D12

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")

#include "D3D12Renderer.h"
#include <dxgi1_4.h>

namespace Hawk {
namespace Gfx {

using Microsoft::WRL::ComPtr;

void D3D12Renderer::Initialize()
{
	CreateDebugInterface();
	CreateDevice();
	LOG("Init done (D3D12)", "d3d", Info);
}

void D3D12Renderer::CreateDebugInterface()
{
#ifdef HAWK_DEBUG
	ComPtr<ID3D12Debug> l_DebugInterface;
	THROW_IF_COMERR(D3D12GetDebugInterface(IID_PPV_ARGS(&l_DebugInterface)), "Failed to get d3d debug interface");
	l_DebugInterface->EnableDebugLayer();
#endif
}

void D3D12Renderer::CreateDevice()
{
	ComPtr<IDXGIFactory4> l_Factory;
	THROW_IF_COMERR(CreateDXGIFactory1(IID_PPV_ARGS(&l_Factory)), "Failed to create DXGI Factory");

	if (Config::Instance().Get("gfx.preferSWRendering", false))
	{
		ComPtr<IDXGIAdapter> l_WarpAdapter;
		THROW_IF_COMERR(l_Factory->EnumWarpAdapter(IID_PPV_ARGS(&l_WarpAdapter)), "EnumWarpAdapter failed");
		THROW_IF_COMERR(D3D12CreateDevice(l_WarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)), "Failed to initialize WARP device");
	}
	else
	{
		ComPtr<IDXGIAdapter1> l_Adapter;
		//GetHardwareAdapter(l_Factory.Get(), &l_Adapter);

		THROW_IF_COMERR(D3D12CreateDevice(l_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)), "Failed to create HW device");
	}
}

}
}
#endif