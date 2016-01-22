#pragma once
#include <string>

struct ID3D12Device;
struct IDXGIFactory4;

namespace Hawk {
namespace Gfx {

namespace D3D12DeviceFactory
{
	ID3D12Device* CreateDevice(IDXGIFactory4* p_Factory, const std::string& p_Luid = std::string());
	ID3D12Device* CreateWARPDevice(IDXGIFactory4* p_Factory);

#ifdef HAWK_DEBUG
	void CmdListAdapters();
#endif
}
}
}