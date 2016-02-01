#include "pch.h"
#include "CommandList.h"
#include "RenderView.h"
#include "d3dx12.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

CommandList::CommandList(DeviceComPtr_t& p_Device, std::shared_ptr<RenderView>& p_RenderView, CommandAllocatorComPtr_t& p_Allocator)
: m_RenderView(p_RenderView)
, m_Allocator(p_Allocator)
{
	THROW_IF_COMERR(p_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, p_Allocator.Get(), m_State.Get(), IID_PPV_ARGS(&m_CommandList)), "Failed to create command list");
	THROW_IF_COMERR(m_CommandList->Close(), "Failed to close command list");	// Need to close since Reset() requires it to be closed
}

GfxCommandListComPtr_t& CommandList::GetD3DObject()
{
	return m_CommandList;
}

void CommandList::BeginRecord()
{
	THROW_IF_COMERR(m_CommandList->Reset(m_Allocator.Get(), m_State.Get()), "Failed to reset command list");
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderView->GetCurrBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
}

void CommandList::EndRecord()
{
	//m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderView->GetCurrBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	THROW_IF_COMERR(m_CommandList->Close(), "Failed to close command list");
}

}
}
}