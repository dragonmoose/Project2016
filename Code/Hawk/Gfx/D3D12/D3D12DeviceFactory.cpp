#include "pch.h"
#include "D3D12DeviceFactory.h"
#include "Debug/Assert.h"
#include "Util/StringUtil.h"
#include <boost/lexical_cast.hpp>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

namespace Hawk {
namespace Gfx {

namespace D3D12DeviceFactory
{
	using HWAdapters_t = std::vector<IDXGIAdapter*>;

	void GetHWAdapters(IDXGIFactory1* p_pFactory, HWAdapters_t& p_HWAdapters);
	IDXGIAdapter* GetPreferredHWAdapter(const HWAdapters_t& p_HWAdapters);
	IDXGIAdapter* GetSpecificDevice(const std::string& p_Luid, const HWAdapters_t& p_HWAdapters);
}

using Microsoft::WRL::ComPtr;

ID3D12Device* D3D12DeviceFactory::CreateDevice(const std::string& p_Luid)
{
	ComPtr<IDXGIFactory4> l_Factory;	//ComPtr destr. calls Release() on ptr, which should be done according to CreateDXGIFactory1 documentation: https://msdn.microsoft.com/en-us/library/windows/desktop/ff471318(v=vs.85).aspx
	THROW_IF_COMERR(CreateDXGIFactory1(IID_PPV_ARGS(&l_Factory)), "Failed to create DXGI Factory");

	HWAdapters_t l_Adapters;
	GetHWAdapters(l_Factory.Get(), l_Adapters);
	IDXGIAdapter* l_pAdapter = p_Luid.empty() ? GetPreferredHWAdapter(l_Adapters) : GetSpecificDevice(p_Luid, l_Adapters);

	ID3D12Device* l_pDevice = nullptr;
	THROW_IF_COMERR(D3D12CreateDevice(l_pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&l_pDevice)), "Failed to create HW device");

	return l_pDevice;
}

ID3D12Device* D3D12DeviceFactory::CreateWARPDevice()
{
	ComPtr<IDXGIFactory4> l_Factory;
	THROW_IF_COMERR(CreateDXGIFactory1(IID_PPV_ARGS(&l_Factory)), "Failed to create DXGI Factory");

	IDXGIAdapter* l_pWarpAdapter;
	THROW_IF_COMERR(l_Factory->EnumWarpAdapter(IID_PPV_ARGS(&l_pWarpAdapter)), "EnumWarpAdapter failed");

	ID3D12Device* l_pDevice = nullptr;
	THROW_IF_COMERR(D3D12CreateDevice(l_pWarpAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&l_pDevice)), "Failed to initialize WARP device");
	
	return l_pDevice;
}

void D3D12DeviceFactory::GetHWAdapters(IDXGIFactory1* p_pFactory, HWAdapters_t& p_HWAdapters)
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

IDXGIAdapter* D3D12DeviceFactory::GetPreferredHWAdapter(const HWAdapters_t& p_HWAdapters)
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
	LOG("Preferred adapter: " << l_PreferredAdapterDesc, "d3d12", Debug);
	return l_pPreferredAdapter;
}

IDXGIAdapter* D3D12DeviceFactory::GetSpecificDevice(const std::string& p_Luid, const HWAdapters_t& p_HWAdapters)
{
	ASSERT(!p_Luid.empty(), "Luid string should not be empty");

	LUID l_Luid;
	std::size_t l_Pos = p_Luid.find_first_of(":");
	THROW_IF(l_Pos == std::string::npos, "Invalid Luid format: " << p_Luid);

	try
	{
		l_Luid.HighPart = boost::lexical_cast<LONG>(p_Luid.substr(0, l_Pos));
		l_Luid.LowPart = boost::lexical_cast<DWORD>(p_Luid.substr(l_Pos + 1));
		LOG("Parsed Luid=" << l_Luid.HighPart << ":" << l_Luid.LowPart, "d3d", Debug);
	}
	catch (boost::bad_lexical_cast& e)
	{
		THROW("Failed to cast from Luid=" << p_Luid);
	}

	const auto& l_Itr = std::find_if(p_HWAdapters.cbegin(), p_HWAdapters.cend(), [l_Luid](IDXGIAdapter* p_pAdapter)
	{
		DXGI_ADAPTER_DESC l_Desc;
		THROW_IF_COMERR(p_pAdapter->GetDesc(&l_Desc), "Failed to get adapter desc");
		bool l_bMatch = l_Luid.HighPart == l_Desc.AdapterLuid.HighPart && l_Luid.LowPart == l_Desc.AdapterLuid.LowPart;
		LOG_IF(l_bMatch, "Specified Luid matches device: " << StringUtil::WCharToString(l_Desc.Description), "d3d", Debug);
		return l_bMatch;
	});
	THROW_IF(l_Itr == p_HWAdapters.cend(), "Failed to find adapter with Luid: " << p_Luid);
	return *l_Itr;
}

#ifdef HAWK_DEBUG
void D3D12DeviceFactory::CmdListAdapters()
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