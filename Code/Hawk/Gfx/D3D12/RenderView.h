#pragma once
#include "Constants.h"
#include "Common.h"
#include "d3dx12.h"
#include <array>

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class RenderView final
{
public:
	using BackBufferArray_t = std::array<ResourceComPtr_t, c_uiNumBackBuffers>;

	RenderView(SwapChainComPtr_t& p_SwapChain, DeviceComPtr_t& p_Device);
	void BeginFrame();
	void EndFrame();

	ID3D12Resource* GetCurrBackBuffer();
	const BackBufferArray_t& GetBackBuffers() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetHandle() const;
	UINT32 GetCurrBufferIndex() const;
	void SetFullscreenState(bool p_bState);
	void OnWindowSizeChanged(UINT32 p_uiWidth, UINT32 p_uiHeight);

private:
	void Initialize();
	void CreateDescriptorHeap(DeviceComPtr_t& p_Device);
	void CreateRTV(DeviceComPtr_t& p_Device);

	SwapChainComPtr_t m_SwapChain;
	RTVHeapComPtr_t m_Heap;
	UINT32 m_uiCurrBufferIndex;
	UINT32 m_uiDescriptorSize;
	BackBufferArray_t m_Buffers;
	DeviceComPtr_t m_Device;
};
}
}
}