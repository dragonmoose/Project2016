#include "pch.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "BaseFactory.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

CommandQueue::CommandQueue(DeviceComPtr_t& p_Device)
{
	BaseFactory::CreateCommandQueue(p_Device.Get(), m_Object);

	THROW_IF_COMERR(p_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)), "Failed to create fence");
	m_uiNextValue = 1;

	m_Event = CreateEvent(nullptr, false, false, nullptr);
	if (!m_Event)
	{
		THROW_COMERR(HRESULT_FROM_WIN32(GetLastError()), "Failed to create fence event");
	}
}

CommandQueue::~CommandQueue()
{
	WaitForGPU();
	CloseHandle(m_Event);

	for (auto l_List : m_CommandLists)
	{
		l_List->Release();
	}
}

void CommandQueue::AddCommandList(std::unique_ptr<CommandList>& p_CommandList)
{
	ID3D12CommandList* l_CmdList;
	p_CommandList->GetD3DObject().CopyTo<ID3D12CommandList>(&l_CmdList);
	m_CommandLists.push_back(l_CmdList);
}

CommandQueueComPtr_t& CommandQueue::GetD3DObject()
{
	return m_Object;
}

void CommandQueue::WaitForGPU()
{
	const UINT64 l_uiValue = m_uiNextValue;
	THROW_IF_COMERR(m_Object->Signal(m_Fence.Get(), l_uiValue), "Failed to update fence. Value=" << l_uiValue);
	m_uiNextValue++;

	if (m_Fence->GetCompletedValue() < l_uiValue)
	{
		THROW_IF_COMERR(m_Fence->SetEventOnCompletion(l_uiValue, m_Event), "Failed to set completed event for fence");
		WaitForSingleObject(m_Event, INFINITE);
	}
}

void CommandQueue::Execute()
{
	m_Object->ExecuteCommandLists(m_CommandLists.size(), m_CommandLists.data());
}

}
}
}