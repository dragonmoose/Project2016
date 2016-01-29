#pragma once
#include "Common.h"
#include "BackBuffer.h"
#include "CommandQueue.h"
#include "Gfx/RenderingAPISubModule.h"
#include <vector>
#include <memory>

struct IDXGIFactory4;

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class CommandQueue;
class BackBuffer;

class Renderer final : public RenderingAPISubModule
{
public:
	Renderer();

	std::string GetName() const override;
	void Initialize() override;

#ifdef HAWK_DEBUG
	void InitializeConsole() override;
#endif

	void Update(const Duration& p_Duration) override;

private:
	void CreateDevice(IDXGIFactory4* p_Factory);
	void SetFullscreen(bool p_bValue);

	DeviceComPtr_t m_Device;
	std::unique_ptr<CommandQueue> m_CommandQueue;
	SwapChainComPtr_t m_SwapChain;
	CommandAllocatorComPtr_t m_CommandAllocator;
	std::unique_ptr<BackBuffer> m_BackBuffer;
};

}
}
}