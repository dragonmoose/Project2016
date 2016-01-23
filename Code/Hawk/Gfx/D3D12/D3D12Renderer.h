#pragma once
#include "Gfx/RenderingAPISubModule.h"
#include <wrl/client.h>
#include <vector>
#include <d3d12.h>
#include <dxgi1_4.h>

struct IDXGIFactory4;

using Microsoft::WRL::ComPtr;

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

	ComPtr<ID3D12Device> m_Device;
	ComPtr<ID3D12CommandQueue> m_CommandQueue;
	ComPtr<IDXGISwapChain3> m_SwapChain;
};

}
}