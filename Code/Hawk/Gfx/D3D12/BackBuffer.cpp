#include "pch.h"
#include "BackBuffer.h"
#include "Constants.h"
#include "d3dx12.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

BackBuffer::BackBuffer(SwapChainComPtr_t& p_SwapChain, DeviceComPtr_t& p_Device)
: m_SwapChain(p_SwapChain)
, m_uiDescriptorSize(0)
{
	CreateDescriptorHeap(p_Device);
	m_uiDescriptorSize = p_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CreateRTV(p_Device);
}

void BackBuffer::BeginFrame()
{
	m_uiCurrFrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void BackBuffer::EndFrame()
{
	THROW_IF_COMERR(m_SwapChain->Present(c_uiPresentSyncInterval, 0), "Present failed");
}

void BackBuffer::CreateDescriptorHeap(DeviceComPtr_t& p_Device)
{
	D3D12_DESCRIPTOR_HEAP_DESC l_Desc = {};
	l_Desc.NumDescriptors = c_uiNumBackBuffers;
	l_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	l_Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	THROW_IF_COMERR(p_Device->CreateDescriptorHeap(&l_Desc, IID_PPV_ARGS(&m_Heap)), "Failed to create descriptor heap");
}

void BackBuffer::CreateRTV(DeviceComPtr_t& p_Device)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE l_Handle(m_Heap->GetCPUDescriptorHandleForHeapStart());
	for (unsigned int i = 0; i < c_uiNumBackBuffers; i++)
	{
		THROW_IF_COMERR(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_Buffers[i])), "Failed to get back buffer from swap chain");
		p_Device->CreateRenderTargetView(m_Buffers[i].Get(), nullptr, l_Handle);
		l_Handle.Offset(1, m_uiDescriptorSize);
	}
}

}
}
}