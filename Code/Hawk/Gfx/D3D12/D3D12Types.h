#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_4.h>

namespace Hawk {
namespace Gfx {

using DeviceComPtr_t = Microsoft::WRL::ComPtr<ID3D12Device>;
using CommandQueueComPtr_t = Microsoft::WRL::ComPtr<ID3D12CommandQueue>;
using SwapChainComPtr_t = Microsoft::WRL::ComPtr<IDXGISwapChain3>;
using DXGIFacoryComPtr_t = Microsoft::WRL::ComPtr<IDXGIFactory4>;
using CommandAllocatorComPtr_t = Microsoft::WRL::ComPtr<ID3D12CommandAllocator>;
using DXGIAdapterComPtr_t = Microsoft::WRL::ComPtr<IDXGIAdapter>;


}
}
