#include "pch.h"
#include "RenderView.h"
#include "Constants.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

RenderView::RenderView(SwapChainComPtr_t& p_SwapChain, DeviceComPtr_t& p_Device)
: m_SwapChain(p_SwapChain)
, m_uiDescriptorSize(0)
, m_Device(p_Device)
{
	Initialize();

}

void RenderView::BeginFrame()
{
	m_uiCurrBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void RenderView::EndFrame()
{
	THROW_IF_COMERR(m_SwapChain->Present(c_uiPresentSyncInterval, 0), "Present failed");
}

ID3D12Resource* RenderView::GetCurrBackBuffer()
{
	return m_Buffers[m_uiCurrBufferIndex].Get();
}

const RenderView::BackBufferArray_t& RenderView::GetBackBuffers() const
{
	return m_Buffers;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE RenderView::GetHandle() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_Heap->GetCPUDescriptorHandleForHeapStart(), m_uiCurrBufferIndex, m_uiDescriptorSize);
}

unsigned int RenderView::GetCurrBufferIndex() const
{
	return m_uiCurrBufferIndex;
}

void RenderView::SetFullscreenState(bool p_bState)
{
	THROW_IF_COMERR(m_SwapChain->SetFullscreenState(p_bState, nullptr), "Failed to set fullscreen state. State=" << p_bState);
}

void RenderView::OnWindowSizeChanged(unsigned int p_uiWidth, unsigned int p_uiHeight)
{
	DXGI_SWAP_CHAIN_DESC1 l_Desc;
	m_SwapChain->GetDesc1(&l_Desc);
	m_SwapChain->ResizeBuffers(c_uiNumBackBuffers, p_uiWidth, p_uiHeight, l_Desc.Format, l_Desc.Flags);
	m_uiCurrBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void RenderView::Initialize()
{
	CreateDescriptorHeap(m_Device);
	m_uiDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CreateRTV(m_Device);
}

void RenderView::CreateDescriptorHeap(DeviceComPtr_t& p_Device)
{
	D3D12_DESCRIPTOR_HEAP_DESC l_Desc = {};
	l_Desc.NumDescriptors = c_uiNumBackBuffers;
	l_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	l_Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	THROW_IF_COMERR(p_Device->CreateDescriptorHeap(&l_Desc, IID_PPV_ARGS(&m_Heap)), "Failed to create descriptor heap");
}

void RenderView::CreateRTV(DeviceComPtr_t& p_Device)
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