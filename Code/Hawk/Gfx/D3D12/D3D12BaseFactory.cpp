#include "pch.h"
#include "D3D12BaseFactory.h"
#include "Base/WindowManager.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace Hawk {
namespace Gfx {

ID3D12CommandQueue* D3D12BaseFactory::CreateCommandQueue(ID3D12Device* p_pDevice)
{
	ID3D12CommandQueue* l_pQueue = nullptr;

	D3D12_COMMAND_QUEUE_DESC l_Desc = {};
	l_Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	l_Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	l_Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	l_Desc.NodeMask = 0;

	THROW_IF_COMERR(p_pDevice->CreateCommandQueue(&l_Desc, IID_PPV_ARGS(&l_pQueue)), "Failed to create CommandQueue");
	return l_pQueue;
}

IDXGISwapChain3* D3D12BaseFactory::CreateSwapChain(IDXGIFactory4* p_Factory, ID3D12CommandQueue* p_CommandQueue)
{
	DXGI_SWAP_CHAIN_DESC1 l_Desc = {};
	l_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	l_Desc.SampleDesc.Count = 1;
	l_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	l_Desc.BufferCount = Config::Instance().Get("gfx.backBufferCount", 2);
	l_Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ComPtr<IDXGISwapChain1> l_SwapChain;

	THROW_IF_COMERR(p_Factory->CreateSwapChainForHwnd(p_CommandQueue, WindowManager::GetHandle(), &l_Desc, nullptr, nullptr, &l_SwapChain), "Failed to create swap chain");

	IDXGISwapChain3* l_pSwapChain3;
	THROW_IF_COMERR(l_SwapChain.CopyTo<IDXGISwapChain3>(&l_pSwapChain3), "Failed to cast to IDXGISwapChain3");
	return l_pSwapChain3;
}

ID3D12CommandAllocator* D3D12BaseFactory::CreateCommandAllocator(ID3D12Device* p_pDevice)
{
	ID3D12CommandAllocator* l_pAllocator;
	p_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&l_pAllocator));
	return l_pAllocator;
}


}
}