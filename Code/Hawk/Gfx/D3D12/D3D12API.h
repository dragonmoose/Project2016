#pragma once
#include "Common.h"
#include "RenderView.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "TextRenderer.h"
#include "Gfx/IRenderingAPI.h"
#include <vector>
#include <memory>

struct IDXGIFactory4;

namespace Hawk {
namespace Gfx {
namespace D3D12 {

class CommandQueue;
class RenderView;

class D3D12API final : public IRenderingAPI
{
public:
	D3D12API() = default;
	void Initialize() override;
	void Render() override;
	void SetFullscreenState(bool p_bState) override;

	void CmdListDevices() override;

private:
	void CreateDevice(IDXGIFactory4* p_Factory);

	void BeginFrame();
	void EndFrame();
	void RecordCommands();

	static const std::string& GetLogDesc();

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