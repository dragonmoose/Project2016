#include "pch.h"
#include "D3D12BaseFactory.h"
#include "Base/WindowManager.h"
#include <d3d12.h>
#include <dxgi1_4.h>

namespace Hawk {
namespace Gfx {

void D3D12BaseFactory::CreateCommandQueue(ID3D12Device* p_pDevice, CommandQueueComPtr_t& p_CommandQueue)
{
	CommandQueueComPtr_t l_CommandQueue;

	D3D12_COMMAND_QUEUE_DESC l_Desc = {};
	l_Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	l_Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	l_Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	l_Desc.NodeMask = 0;

	THROW_IF_COMERR(p_pDevice->CreateCommandQueue(&l_Desc, IID_PPV_ARGS(&l_CommandQueue)), "Failed to create CommandQueue");
	p_CommandQueue = std::move(l_CommandQueue);
}

void D3D12BaseFactory::CreateSwapChain(IDXGIFactory4* p_Factory, ID3D12CommandQueue* p_CommandQueue, SwapChainComPtr_t& p_SwapChain)
{
	DXGI_SWAP_CHAIN_DESC1 l_Desc = {};
	l_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	l_Desc.SampleDesc.Count = 1;
	l_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	l_Desc.BufferCount = Config::Instance().Get("gfx.backBufferCount", 2);
	l_Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> l_SwapChain;
	THROW_IF_COMERR(p_Factory->CreateSwapChainForHwnd(p_CommandQueue, WindowManager::GetHandle(), &l_Desc, nullptr, nullptr, &l_SwapChain), "Failed to create swap chain");
	THROW_IF_COMERR(l_SwapChain.CopyTo<IDXGISwapChain3>(&p_SwapChain), "Failed to cast and copy to IDXGISwapChain3");
}

void D3D12BaseFactory::CreateCommandAllocator(ID3D12Device* p_pDevice, CommandAllocatorComPtr_t& p_Allocator)
{
	CommandAllocatorComPtr_t l_Allocator;
	THROW_IF_COMERR(p_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&l_Allocator)), "Failed to create comand allocator");
	p_Allocator = std::move(l_Allocator);
}


}
}