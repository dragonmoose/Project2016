#pragma once
#include "Common.h"
#include "RenderView.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "TextRenderer.h"
#include "Gfx/RenderingAPISubModule.h"
#include <vector>
#include <memory>

struct IDXGIFactory4;

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class CommandQueue;
class RenderView;

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

	void BeginFrame();
	void EndFrame();
	void RecordCommands();

	DeviceComPtr_t m_Device;
	std::unique_ptr<CommandQueue> m_CommandQueue;
	SwapChainComPtr_t m_SwapChain;
	CommandAllocatorComPtr_t m_CommandAllocator;
	std::shared_ptr<RenderView> m_RenderView;
	std::vector<std::unique_ptr<CommandList>> m_CommandLists;
	std::unique_ptr<TextRenderer> m_TextRenderer;
};

}
}
}