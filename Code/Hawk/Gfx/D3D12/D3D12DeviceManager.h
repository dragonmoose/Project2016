#pragma once
#include <string>

struct ID3D12Device;

namespace Hawk {
namespace Gfx {

namespace D3D12DeviceManager
{
	ID3D12Device* CreateDevice(const std::string& p_Luid = std::string());
	ID3D12Device* CreateWARPDevice();

#ifdef HAWK_DEBUG
	void CmdListAdapters();
#endif
}
}
}