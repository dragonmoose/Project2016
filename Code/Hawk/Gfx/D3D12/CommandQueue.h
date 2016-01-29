#pragma once

#include "Common.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class CommandQueue final
{
public:
	CommandQueue(DeviceComPtr_t& p_Device);
	~CommandQueue();

	CommandQueueComPtr_t& GetD3DObject();
	void WaitForGPU();

private:
	FenceComPtr_t m_Fence;
	UINT64 m_uiNextValue;
	HANDLE m_Event;
	CommandQueueComPtr_t m_Object;
};

}
}
}