#include "pch.h"

#ifdef HAWK_RENDERER_D3D12

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")

#include "D3D12Renderer.h"
#include "Debug/Assert.h"
#include "Util/StringUtil.h"

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
	ComPtr<IDXGIFactory4> l_Factory;	//ComPtr destr. calls Release() on ptr, which should be done according to CreateDXGIFactory1 documentation: https://msdn.microsoft.com/en-us/library/windows/desktop/ff471318(v=vs.85).aspx
	THROW_IF_COMERR(CreateDXGIFactory1(IID_PPV_ARGS(&l_Factory)), "Failed to create DXGI Factory");

	if (Config::Instance().Get("gfx.preferSWRendering", false))
	{
		IDXGIAdapter* l_pWarpAdapter;
		THROW_IF_COMERR(l_Factory->EnumWarpAdapter(IID_PPV_ARGS(&l_pWarpAdapter)), "EnumWarpAdapter failed");
		THROW_IF_COMERR(D3D12CreateDevice(l_pWarpAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)), "Failed to initialize WARP device");
		LOG("D3D12 WARP device successfully created", "d3d12", Info);
	}
	else
	{
		HWAdapters_t l_Adapters;
		GetHWAdapters(l_Factory.Get(), l_Adapters);
		IDXGIAdapter* l_pAdapter = GetPreferredHWAdapter(l_Adapters);
		THROW_IF_COMERR(D3D12CreateDevice(l_pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)), "Failed to create HW device");
	}
}

void D3D12Renderer::GetHWAdapters(IDXGIFactory1* p_pFactory, HWAdapters_t& p_HWAdapters) const
{
	int l_iAdapterNo = 0;
	IDXGIAdapter1* l_pAdapter;
	while (p_pFactory->EnumAdapters1(l_iAdapterNo, &l_pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 l_Desc;
		THROW_IF_COMERR(l_pAdapter->GetDesc1(&l_Desc), "Failed to get adapter desc. AdapterNo: " << l_iAdapterNo);
		if (!(l_Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
		{
			if (D3D12CreateDevice(l_pAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))
			{
				p_HWAdapters.push_back(l_pAdapter);
				LOG("Adding d3d12 compatible adapter: " << StringUtil::WCharToString(l_Desc.Description), "d3d12", Debug);
			}
			else
			{
				LOG("Adapter " << StringUtil::WCharToString(l_Desc.Description) << " did not support D3D12 - skipped. AdapterNo: " << l_iAdapterNo, "d3d12", Debug);
			}
		}
		else
		{
			LOG("Skipped software adapter: " << StringUtil::WCharToString(l_Desc.Description), "d3d12", Debug);
		}
		l_iAdapterNo++;
	}
	THROW_IF(p_HWAdapters.empty(), "No d3d12 compatible hardware adapters found");
}

IDXGIAdapter* D3D12Renderer::GetPreferredHWAdapter(const HWAdapters_t& p_HWAdapters) const
{
	ASSERT(!p_HWAdapters.empty(), "Vector is empty");
	IDXGIAdapter* l_pPreferredAdapter = nullptr;
	SIZE_T l_DedicatedVidMemMax = 0;
	std::string l_PreferredAdapterDesc;
	for (const auto& l_pAdapter : p_HWAdapters)
	{
		DXGI_ADAPTER_DESC l_Desc;
		THROW_IF_COMERR(l_pAdapter->GetDesc(&l_Desc), "Failed to get adapter desc");
		std::string l_DescStr = StringUtil::WCharToString(l_Desc.Description);
		LOG("Adapter: " << l_DescStr << " Dedicated video mem: " << l_Desc.DedicatedVideoMemory / (1024 * 1024) << " MB", "d3d12", Debug);
		if (l_Desc.DedicatedVideoMemory > l_DedicatedVidMemMax)
		{
			l_pPreferredAdapter = l_pAdapter;
			l_PreferredAdapterDesc = l_DescStr;
		}
	}
	THROW_IF_NOT(l_pPreferredAdapter, "Failed to select preferred adapter");
	LOG("Preferred adapter: " << l_PreferredAdapterDesc, "d3d12", Info);
	return l_pPreferredAdapter;
}

}
}
#endif