#include "pch.h"
#include "CommandQueue.h"
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
		THROW_IF_COMERR(HRESULT_FROM_WIN32(GetLastError()), "Failed to create fence event");
		THROW("Failed to create fence event (prev statement should have thrown");
	}
}

CommandQueue::~CommandQueue()
{
	CloseHandle(m_Event);
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

}
}
}