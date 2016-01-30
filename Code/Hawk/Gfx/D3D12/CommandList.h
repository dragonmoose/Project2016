#pragma once

#include "Common.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class RenderView;

class CommandList
{
public:
	CommandList(DeviceComPtr_t& p_Device, std::shared_ptr<RenderView>& p_RenderView, CommandAllocatorComPtr_t& p_Allocator);
	virtual ~CommandList() = default;

	CommandList(const CommandList&) = delete;
	CommandList& operator=(const CommandList&) = delete;

	ID3D12CommandList* GetD3DObject();
	void BeginRecord();
	void EndRecord();
	virtual void Record() = 0;

protected:
	GfxCommandListComPtr_t m_CommandList;
	std::shared_ptr<RenderView> m_RenderView;
	PipelineStateComPtr_t m_State;

private:
	CommandAllocatorComPtr_t m_Allocator;
};

}
}
}