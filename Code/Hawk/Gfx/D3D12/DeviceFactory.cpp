#include "pch.h"
#include "DeviceFactory.h"
#include "Debug/Assert.h"
#include "Util/StringUtil.h"
#include <boost/lexical_cast.hpp>

namespace Hawk {
namespace Gfx {
namespace D3D12 {

namespace DeviceFactory
{
	using HWAdapters_t = std::vector<DXGIAdapterComPtr_t>;

	void GetPrimaryAdapter(IDXGIFactory4* p_pFactory, DXGIAdapterComPtr_t& p_Adapter);
	void GetHWAdapters(IDXGIFactory4* p_pFactory, HWAdapters_t& p_HWAdapters);
	void GetPreferredHWAdapter(const HWAdapters_t& p_HWAdapters, DXGIAdapterComPtr_t& p_Adapter);
	void GetSpecificAdapter(const std::string& p_Luid, const HWAdapters_t& p_HWAdapters, DXGIAdapterComPtr_t& p_Adapter);
}

void DeviceFactory::CreateDevice(IDXGIFactory4* p_Factory, const std::string& p_Luid, DeviceComPtr_t& p_Device)
{
	DXGIAdapterComPtr_t l_Adapter;
	if (p_Luid.empty())
	{
		GetPrimaryAdapter(p_Factory, l_Adapter);
	}
	else
	{
		HWAdapters_t l_Adapters;
		GetHWAdapters(p_Factory, l_Adapters);
		GetSpecificAdapter(p_Luid, l_Adapters, l_Adapter);
	}

	DeviceComPtr_t l_Device;
	THROW_IF_COMERR(D3D12CreateDevice(l_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&l_Device)), "Failed to create HW device");
	p_Device = std::move(l_Device);
}

void DeviceFactory::CreateWARPDevice(IDXGIFactory4* p_Factory, DeviceComPtr_t& p_Device)
{
	DXGIAdapterComPtr_t l_WarpAdapter;
	THROW_IF_COMERR(p_Factory->EnumWarpAdapter(IID_PPV_ARGS(&l_WarpAdapter)), "EnumWarpAdapter failed");

	DeviceComPtr_t l_Device;
	THROW_IF_COMERR(D3D12CreateDevice(l_WarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&l_Device)), "Failed to initialize WARP device");
	p_Device = std::move(l_Device);
}

void DeviceFactory::GetPrimaryAdapter(IDXGIFactory4* p_pFactory, DXGIAdapterComPtr_t& p_Adapter)
{
	int l_iAdapterNo = 0;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> l_Adapter;
	while (p_pFactory->EnumAdapters1(l_iAdapterNo, &l_Adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 l_Desc;
		THROW_IF_COMERR(l_Adapter->GetDesc1(&l_Desc), "Failed to get adapter desc. AdapterNo: " << l_iAdapterNo);
		if (!(l_Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
		{
			if (D3D12CreateDevice(l_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))
			{
				LOG("Primary adapter: " << StringUtil::WCharToString(l_Desc.Description), "d3d12", Debug);
				p_Adapter = std::move(l_Adapter);
				return;
			}
		}
		l_iAdapterNo++;
	}
	THROW("No d3d12 compatible hardware adapter found");
}

void DeviceFactory::GetHWAdapters(IDXGIFactory4* p_pFactory, HWAdapters_t& p_HWAdapters)
{
	int l_iAdapterNo = 0;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> l_Adapter;
	while (p_pFactory->EnumAdapters1(l_iAdapterNo, &l_Adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 l_Desc;
		THROW_IF_COMERR(l_Adapter->GetDesc1(&l_Desc), "Failed to get adapter desc. AdapterNo: " << l_iAdapterNo);
		if (!(l_Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
		{
			if (D3D12CreateDevice(l_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))
			{
				p_HWAdapters.push_back(l_Adapter.Get());
			}
		}
		l_iAdapterNo++;
	}
	THROW_IF(p_HWAdapters.empty(), "No d3d12 compatible hardware adapters found");
}

void DeviceFactory::GetPreferredHWAdapter(const HWAdapters_t& p_HWAdapters, DXGIAdapterComPtr_t& p_Adapter)
{
	ASSERT(!p_HWAdapters.empty(), "Vector is empty");
	IDXGIAdapter* l_pPreferredAdapter = nullptr;
	SIZE_T l_DedicatedVidMemMax = 0;
	std::string l_PreferredAdapterDesc;
	for (const auto& l_Adapter : p_HWAdapters)
	{
		DXGI_ADAPTER_DESC l_Desc;
		THROW_IF_COMERR(l_Adapter->GetDesc(&l_Desc), "Failed to get adapter desc");
		std::string l_DescStr = StringUtil::WCharToString(l_Desc.Description);
		LOG("Evaluating hardware adapter: " << l_DescStr << " Dedicated video mem: " << l_Desc.DedicatedVideoMemory / (1024 * 1024) << " MB", "d3d12", Debug);
		if (l_Desc.DedicatedVideoMemory > l_DedicatedVidMemMax)
		{
			l_pPreferredAdapter = l_Adapter.Get();
			l_PreferredAdapterDesc = l_DescStr;
		}
	}
	THROW_IF_NOT(l_pPreferredAdapter, "Failed to select preferred adapter");
	LOG("Preferred adapter: " << l_PreferredAdapterDesc, "d3d12", Debug);
	p_Adapter = l_pPreferredAdapter;
}

void DeviceFactory::GetSpecificAdapter(const std::string& p_Luid, const HWAdapters_t& p_HWAdapters, DXGIAdapterComPtr_t& p_Adapter)
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

	const auto& l_Itr = std::find_if(p_HWAdapters.cbegin(), p_HWAdapters.cend(), [l_Luid](const DXGIAdapterComPtr_t& p_Adapter)
	{
		DXGI_ADAPTER_DESC l_Desc;
		THROW_IF_COMERR(p_Adapter->GetDesc(&l_Desc), "Failed to get adapter desc");
		bool l_bMatch = l_Luid.HighPart == l_Desc.AdapterLuid.HighPart && l_Luid.LowPart == l_Desc.AdapterLuid.LowPart;
		LOG_IF(l_bMatch, "Specified Luid matches device: " << StringUtil::WCharToString(l_Desc.Description), "d3d", Debug);
		return l_bMatch;
	});

	THROW_IF(l_Itr == p_HWAdapters.cend(), "Failed to find adapter with Luid: " << p_Luid);
	p_Adapter = *l_Itr;
}

#ifdef HAWK_DEBUG
void DeviceFactory::CmdListAdapters()
{
	Microsoft::WRL::ComPtr<IDXGIFactory4> l_Factory;
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
}