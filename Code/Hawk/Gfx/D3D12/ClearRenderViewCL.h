#pragma once
#include "CommandList.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class ClearRenderViewCL : public CommandList
{
public:
	ClearRenderViewCL(DeviceComPtr_t& p_Device, std::shared_ptr<RenderView>& p_RenderView, CommandAllocatorComPtr_t& p_Allocator);
	void Record() override;
};

}
}
}