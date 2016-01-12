#include "pch.h"
#include "RenderingModule.h"
#include <client.h>
#include <d3d12.h>

namespace Hawk {
namespace Gfx {

void RenderingModule::Initialize()
{
#ifdef HAWK_DEBUG
	ComPtr<ID3D12Debug> l_DebugInterface;
	THROW_IF_COMERR(D3D12GetDebugInterface(IID_PPV_ARGS(&l_DebugInterface)), "Failed to get d3d debug interface");
	l_DebugInterface->EnableDebugLayer();
#endif
}

void RenderingModule::RegisterEvents(EventManager& p_EventManager)
{
}

}
}