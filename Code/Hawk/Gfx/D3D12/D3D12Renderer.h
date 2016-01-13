#pragma once
#include "Gfx/IRenderingAPI.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace Hawk {
namespace Gfx {

class D3D12Renderer final : public IRenderingAPI
{
public:
	void Initialize() override;

private:
	void CreateDebugInterface();
	void CreateDevice();

	Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
};

}
}