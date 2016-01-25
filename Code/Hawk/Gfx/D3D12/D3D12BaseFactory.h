#pragma once
#include "D3D12Types.h"
#include <windef.h>
#include <string>

struct ID3D12CommandQueue;

namespace Hawk {
namespace Gfx {

namespace D3D12BaseFactory
{
	void CreateCommandQueue(ID3D12Device* p_pDevice, CommandQueueComPtr_t& p_CommandQueue);
	void CreateSwapChain(IDXGIFactory4* p_Factory, ID3D12CommandQueue* p_CommandQueue, SwapChainComPtr_t& p_SwapChain);
	void CreateCommandAllocator(ID3D12Device* p_pDevice, CommandAllocatorComPtr_t& p_Allocator);
}
}
}