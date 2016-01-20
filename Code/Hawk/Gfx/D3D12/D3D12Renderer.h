#pragma once
#include "Gfx/RenderingAPISubModule.h"
#include <wrl/client.h>
#include <vector>
#include <d3d12.h>

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

	Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
};

}
}