#pragma once
#include <windef.h>
#include <string>

struct ID3D12CommandQueue;
struct ID3D12Device;
struct IDXGISwapChain3;
struct IDXGIFactory4;
struct ID3D12CommandAllocator;

namespace Hawk {
namespace Gfx {

namespace D3D12BaseFactory
{
	ID3D12CommandQueue* CreateCommandQueue(ID3D12Device* p_pDevice);
	IDXGISwapChain3* CreateSwapChain(IDXGIFactory4* p_Factory, ID3D12CommandQueue* p_CommandQueue);
	ID3D12CommandAllocator* CreateCommandAllocator(ID3D12Device* p_pDevice);
}
}
}