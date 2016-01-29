#pragma once
#include "Constants.h"
#include "Common.h" 
#include <array>

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class BackBuffer final
{
public:
	BackBuffer(SwapChainComPtr_t& p_SwapChain, DeviceComPtr_t& p_Device);
	void BeginFrame();
	void EndFrame();

private:
	void CreateDescriptorHeap(DeviceComPtr_t& p_Device);
	void CreateRTV(DeviceComPtr_t& p_Device);

	SwapChainComPtr_t m_SwapChain;
	RTVHeapComPtr_t m_Heap;
	unsigned int m_uiCurrFrameIndex;
	unsigned int m_uiDescriptorSize;
	std::array<ResourceComPtr_t, c_uiNumBackBuffers> m_Buffers;
};
}
}
}