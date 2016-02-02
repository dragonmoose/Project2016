#pragma once
#include "Constants.h"
#include <d3d11on12.h>
#include <d2d1_3.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <string>

struct ID3D12Device;
struct IUnknown;

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class RenderView;

class TextRenderer final
{
public:
	TextRenderer(ID3D12Device* p_Device, IUnknown** p_CommandQueues, std::shared_ptr<RenderView> p_RenderView);
	~TextRenderer();
	TextRenderer(const TextRenderer&) = delete;
	TextRenderer& operator=(const TextRenderer&) = delete;

	void SetText(const std::string& p_Text);
	void Render();

private:
	Microsoft::WRL::ComPtr<ID3D11On12Device> m_D3D11On12Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_D3D11Context;
	Microsoft::WRL::ComPtr<ID2D1Device2> m_D2DDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext2> m_D2DDeviceContext;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_WrappedBackBuffers[c_uiNumBackBuffers];
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_D2DRenderTargets[c_uiNumBackBuffers];
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_Brush;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_TextFormat;
	std::shared_ptr<RenderView> m_RenderView;

	std::wstring m_Text;
};

}
}
}