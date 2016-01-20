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

std::string D3D12Renderer::GetName() const
{
	return "d3d";
}

void D3D12Renderer::Initialize()
{
	CreateDebugInterface();
	CreateDevice();
	LOG("Init done (D3D12)", "d3d", Info);
}

#ifdef HAWK_DEBUG
void D3D12Renderer::InitializeConsole()
{
	RegisterConsole("d3d.listAdapters", this, &D3D12Renderer::CmdListAdapters, "Lists the available hardware adapters", "");
}
#endif

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
			}
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
		LOG("Evaluating hardware adapter: " << l_DescStr << " Dedicated video mem: " << l_Desc.DedicatedVideoMemory / (1024 * 1024) << " MB", "d3d12", Debug);
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

#ifdef HAWK_DEBUG
void D3D12Renderer::CmdListAdapters()
{
	ComPtr<IDXGIFactory4> l_Factory;
	THROW_IF_COMERR(CreateDXGIFactory1(IID_PPV_ARGS(&l_Factory)), "Failed to create DXGI Factory");

	HWAdapters_t l_Adapters;
	GetHWAdapters(l_Factory.Get(), l_Adapters);

	CONSOLE_WRITE_SCOPE();
	for (const auto& l_pAdapter : l_Adapters)
	{
		DXGI_ADAPTER_DESC l_Desc;
		THROW_IF_COMERR(l_pAdapter->GetDesc(&l_Desc), "Failed to get adapter desc");

		std::cout << StringUtil::WCharToString(l_Desc.Description) << "\n";
		std::cout << "\tVendorId: " << l_Desc.VendorId << "\n";
		std::cout << "\tDeviceId: " << l_Desc.DeviceId << "\n";
		std::cout << "\tSubSysId: " << l_Desc.SubSysId << "\n";
		std::cout << "\tRevision: " << l_Desc.Revision << "\n";
		std::cout << "\tDedicatedVideoMemory: " << (l_Desc.DedicatedVideoMemory / (1024 * 1024)) << " MB\n";
		std::cout << "\tDedicatedSystemMemory: " << (l_Desc.DedicatedSystemMemory / (1024 * 1024)) << " MB\n";
		std::cout << "\tSharedSystemMemory: " << (l_Desc.SharedSystemMemory / (1024 * 1024)) << " MB\n";
		std::cout << "\tLuid: " << l_Desc.AdapterLuid.HighPart << ":" << l_Desc.AdapterLuid.LowPart << "\n\n";
	}
}
#endif

}
}
#endif