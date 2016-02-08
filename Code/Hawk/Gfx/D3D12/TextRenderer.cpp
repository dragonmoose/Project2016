#include "pch.h"
#include "TextRenderer.h"
#include "RenderView.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

TextRenderer::TextRenderer(ID3D12Device* p_Device, IUnknown** p_CommandQueues, std::shared_ptr<RenderView> p_RenderView)
: m_RenderView(p_RenderView)
{
	UINT l_uiDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Documentation states this is required for interop with D3D resources
#ifdef HAWK_DEBUG
	l_uiDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	Microsoft::WRL::ComPtr<ID3D11Device> l_D3D11Device;
	THROW_IF_COMERR(D3D11On12CreateDevice(p_Device, l_uiDeviceFlags, nullptr, 0, p_CommandQueues, 1, 0, &l_D3D11Device, &m_D3D11Context, nullptr), "Failed to create D3D11on12 Device");
	THROW_IF_COMERR(l_D3D11Device.As(&m_D3D11On12Device), "Failed to retrieve D3D11on12Device interface");

	D2D1_FACTORY_OPTIONS l_D2DFactoryOptions = {};
#ifdef HAWK_DEBUG
	l_D2DFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	THROW_IF_COMERR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &l_D2DFactoryOptions, &m_D2DFactory), "Failed to create D2D factory");
	
	Microsoft::WRL::ComPtr<IDXGIDevice> l_DXGIDevice;
	THROW_IF_COMERR(m_D3D11On12Device.As(&l_DXGIDevice), "Failed to retrieve dxgi device");
	THROW_IF_COMERR(m_D2DFactory->CreateDevice(l_DXGIDevice.Get(), &m_D2DDevice), "Failed to create d2d device");
	THROW_IF_COMERR(m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_D2DDeviceContext), "Failed to create d2d device context");

	Microsoft::WRL::ComPtr<IDWriteFactory> l_WriteFactory;
	THROW_IF_COMERR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &l_WriteFactory), "Failed to create write factory");

	THROW_IF_COMERR(m_D2DDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_Brush), "Failed to create brush");
	THROW_IF_COMERR(l_WriteFactory->CreateTextFormat(L"Verdana", nullptr, DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 21, L"en-us", &m_TextFormat), "Failed to create text format");

	THROW_IF_COMERR(m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING), "Failed to set text alignment");
	THROW_IF_COMERR(m_TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR), "Failed to set paragraph alignment");

	CreateBackBufferResources();

	LOG("TextRenderer initialized", "d3d", Info);
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::SetText(const std::string& p_Text)
{
	m_Text = std::wstring(p_Text.cbegin(), p_Text.cend());
}

void TextRenderer::Render()
{
	const unsigned int l_uiBackBufferIndex = m_RenderView->GetCurrBufferIndex();

	D2D1_SIZE_F l_Size = m_D2DRenderTargets[l_uiBackBufferIndex]->GetSize();
	D2D1_RECT_F l_Rect = D2D1::RectF(0, 0, l_Size.width, l_Size.height);

	m_D3D11On12Device->AcquireWrappedResources(m_WrappedBackBuffers[l_uiBackBufferIndex].GetAddressOf(), 1);
	m_D2DDeviceContext->SetTarget(m_D2DRenderTargets[l_uiBackBufferIndex].Get());
	m_D2DDeviceContext->BeginDraw();
	m_D2DDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	m_D2DDeviceContext->DrawText(m_Text.c_str(), m_Text.size(), m_TextFormat.Get(), l_Rect, m_Brush.Get());
	THROW_IF_COMERR(m_D2DDeviceContext->EndDraw(), "EndDraw failed");

	m_D3D11On12Device->ReleaseWrappedResources(m_WrappedBackBuffers[l_uiBackBufferIndex].GetAddressOf(), 1);
	m_D3D11Context->Flush();	// Submit to shared command queue
}

void TextRenderer::CreateBackBufferResources()
{
	float l_fDpiX;
	float l_fDpiY;
	m_D2DFactory->GetDesktopDpi(&l_fDpiX, &l_fDpiY);

	D2D1_BITMAP_PROPERTIES1 l_BitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), l_fDpiX, l_fDpiY);

	D3D11_RESOURCE_FLAGS l_ResourceFlags = { D3D11_BIND_RENDER_TARGET };

	for (int i = 0; i < c_uiNumBackBuffers; i++)
	{
		THROW_IF_COMERR(m_D3D11On12Device->CreateWrappedResource(m_RenderView->GetBackBuffers()[i].Get(), &l_ResourceFlags, D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&m_WrappedBackBuffers[i])), "Failed to create wrapped back buffer");

		Microsoft::WRL::ComPtr<IDXGISurface> l_Surface;
		THROW_IF_COMERR(m_WrappedBackBuffers[i].As(&l_Surface), "Failed to retrieve surface from backbuffer");

		THROW_IF_COMERR(m_D2DDeviceContext->CreateBitmapFromDxgiSurface(l_Surface.Get(),
			&l_BitmapProperties, &m_D2DRenderTargets[i]), "Failed to create rendertarget from back buffer surface");
	}
}

}
}
}