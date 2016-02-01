#include "pch.h"
#include "TextRenderer.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

TextRenderer::TextRenderer(ID3D12Device* p_Device, IUnknown** p_CommandQueues)
{
	Microsoft::WRL::ComPtr<ID3D11Device> l_D3D11Device;
	THROW_IF_COMERR(D3D11On12CreateDevice(p_Device, 0, nullptr, 0, p_CommandQueues, 1, 0, l_D3D11Device.GetAddressOf(), m_D3D11Context.GetAddressOf(), nullptr), "Failed to create D3D11on12 Device");
	THROW_IF_COMERR(l_D3D11Device.As(&m_D3D11On12Device), "Failed to retrieve D3D11on12Device interface");

	LOG("TextRenderer initialized", "d3d", Info);
}

}
}
}