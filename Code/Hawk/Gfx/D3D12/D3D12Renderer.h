#pragma once
#include "Gfx/RenderingAPISubModule.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <vector>

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
	void CreateDebugInterface();
	void CreateDevice();

	using HWAdapters_t = std::vector<IDXGIAdapter*>;
	void GetHWAdapters(IDXGIFactory1* p_pFactory, HWAdapters_t& p_HWAdapters) const;
	IDXGIAdapter* GetPreferredHWAdapter(const HWAdapters_t& p_HWAdapters) const;

#ifdef HAWK_DEBUG
	void CmdListAdapters();
#endif

	Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
};

}
}