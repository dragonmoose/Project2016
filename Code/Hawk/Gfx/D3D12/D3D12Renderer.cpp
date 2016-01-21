#include "pch.h"

#ifdef HAWK_RENDERER_D3D12
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#endif

#include "D3D12Renderer.h"
#include "D3D12DeviceFactory.h"

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

	if (Config::Instance().Get("gfx.preferSWRendering", false))
	{
		m_Device = D3D12DeviceFactory::CreateWARPDevice();
		LOGM("Created WARP device", Info);
	}
	else
	{
		std::string l_DeviceLuid = Config::Instance().Get<std::string>("gfx.deviceLuid", "");
		m_Device = D3D12DeviceFactory::CreateDevice(l_DeviceLuid);
		LOGM_IF(!l_DeviceLuid.empty(), "Created device from Luid: " << l_DeviceLuid, Info);
		LOGM_IF(l_DeviceLuid.empty(), "Created auto-selected device", Info);
	}
}

#ifdef HAWK_DEBUG
void D3D12Renderer::InitializeConsole()
{
	RegisterConsole("d3d.listAdapters", &D3D12DeviceFactory::CmdListAdapters, "Lists the available hardware adapters", "");
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

}
}