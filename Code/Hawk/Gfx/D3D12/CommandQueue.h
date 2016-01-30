#pragma once

#include "Common.h"
#include <vector>
#include <memory>

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class CommandList;

class CommandQueue final
{
public:
	CommandQueue(DeviceComPtr_t& p_Device);
	~CommandQueue();

	void AddCommandList(std::unique_ptr<CommandList>& p_CommandList);

	ID3D12CommandQueue* GetD3DObject();
	void WaitForGPU();
	void Execute();

private:
	FenceComPtr_t m_Fence;
	UINT64 m_uiNextValue;
	HANDLE m_Event;
	CommandQueueComPtr_t m_Object;
	std::vector<ID3D12CommandList*> m_CommandLists;
};

}
}
}