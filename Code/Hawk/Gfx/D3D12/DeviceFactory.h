#pragma once
#include "Common.h"
#include <string>

struct ID3D12Device;
struct IDXGIFactory4;

namespace Hawk {
namespace Gfx {
namespace D3D12 {

namespace DeviceFactory
{
	void CreateDevice(IDXGIFactory4* p_Factory, const std::string& p_Luid, DeviceComPtr_t& p_Device);
	void CreateWARPDevice(IDXGIFactory4* p_Factory, DeviceComPtr_t& p_Device);

#ifdef HAWK_DEBUG
	void CmdListAdapters();
#endif
}
}
}
}