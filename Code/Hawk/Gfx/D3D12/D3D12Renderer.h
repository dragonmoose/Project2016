#pragma once
#include "Gfx/IRenderingAPI.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <vector>

namespace Hawk {
namespace Gfx {

class D3D12Renderer final : public IRenderingAPI
{
public:
	void Initialize() override;

private:
	void CreateDebugInterface();
	void CreateDevice();

	using HWAdapters_t = std::vector<IDXGIAdapter*>;
	void GetHWAdapters(IDXGIFactory1* p_pFactory, HWAdapters_t& p_HWAdapters) const;
	IDXGIAdapter* GetPreferredHWAdapter(const HWAdapters_t& p_HWAdapters) const;

	Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
};

}
}