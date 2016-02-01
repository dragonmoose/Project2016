#pragma once
#include <d3d11on12.h>
#include <wrl/client.h>
#include <string>

struct ID3D12Device;
struct IUnknown;

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class TextRenderer final
{
public:
	TextRenderer(ID3D12Device* p_Device, IUnknown** p_CommandQueues);
	TextRenderer(const TextRenderer&) = delete;
	TextRenderer& operator=(const TextRenderer&) = delete;

	void Initialize();
//	void DrawText(const std::string& p_Text, unsigned int p_uiXPos, unsigned int p_uiYPos, unsigned int p_uiSize);
	void Update();

private:
	Microsoft::WRL::ComPtr<ID3D11On12Device> m_D3D11On12Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_D3D11Context;
};

}
}
}