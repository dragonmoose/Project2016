#include "pch.h"
#include "Util.h"
#include <wrl/client.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

namespace Hawk {
namespace Gfx {
namespace D3D12 {

void Util::EnableDebugLayer()
{
#ifdef HAWK_DEBUG
	ComPtr<ID3D12Debug> l_DebugInterface;
	THROW_IF_COMERR(D3D12GetDebugInterface(IID_PPV_ARGS(&l_DebugInterface)), "Failed to get d3d debug interface");
	l_DebugInterface->EnableDebugLayer();
#endif
}

}
}
}