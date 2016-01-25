#pragma once
#include "D3D12Types.h"
#include "Gfx/RenderingAPISubModule.h"
#include <vector>
#include <d3d12.h>
#include <dxgi1_4.h>

struct IDXGIFactory4;

namespace Hawk {
namespace Gfx {

class D3D12Renderer final : public RenderingAPISubModule
{
public:
	std::string GetName() const override;
	void Initialize() override;

#ifdef HAWK_DEBUG
	void InitializeConsole() override;
#endif

private:
	void CreateDevice(IDXGIFactory4* p_Factory);
	void SetFullscreen(bool p_bValue);

	DeviceComPtr_t m_Device;
	CommandQueueComPtr_t m_CommandQueue;
	SwapChainComPtr_t m_SwapChain;
	CommandAllocatorComPtr_t m_CommandAllocator;
};

}
}