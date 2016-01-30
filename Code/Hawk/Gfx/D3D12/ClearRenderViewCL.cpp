#include "pch.h"
#include "ClearRenderViewCL.h"
#include "Constants.h"
#include "RenderView.h"

namespace Hawk {
namespace Gfx {
namespace D3D12 {

ClearRenderViewCL::ClearRenderViewCL(DeviceComPtr_t& p_Device, std::shared_ptr<RenderView>& p_RenderView, CommandAllocatorComPtr_t& p_Allocator)
: CommandList(p_Device, p_RenderView, p_Allocator)
{
}

void ClearRenderViewCL::Record()
{
	m_CommandList->ClearRenderTargetView(m_RenderView->GetHandle(), c_DefaultClearColor, 0, nullptr);
}

}
}
}