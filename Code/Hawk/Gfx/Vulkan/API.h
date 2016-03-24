#pragma once
#include "Gfx/IRenderingAPI.h"
#include "Instance.h"
#include "Device.h"
#include "System.h"
#include "WindowSurface.h"
#include "Swapchain.h"
#include "CommandPool.h"
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "DepthStencilBuffer.h"
#include "Queue.h"
#include <memory>
#include <array>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class API final : public IRenderingAPI
{
public:
	API() = default;
	~API();

	void Initialize() override;
	void Render() override;
	void SetFullscreenState(bool p_bState) override;
	void OnWindowSizeChanged(UINT32 p_uiWidth, UINT32 p_uiHeight);
	void SetDebugText(const std::string& p_Text) override;

#ifdef HAWK_DEBUG
	void InitializeConsole(ScopedConsoleCommands* p_Console) override;
#endif

private:
	void CreateInstance();
	void CreatePhysicalDevice();
	void SetupQueues(DeviceCreateInfo& p_CreateInfo);
	void CreateDevice(const DeviceCreateInfo& p_CreateInfo);
	void CreateSwapchain();
	void CreateRenderPasses();
	void CreateFrameBuffers();
	void CreateDepthStencilBuffer();
	void SetupCommandBuffers();
	static const std::string& GetLogDesc();

	std::shared_ptr<Instance> m_Instance;
	std::shared_ptr<PhysicalDevice> m_PhysicalDevice;
	std::shared_ptr<Device> m_Device;
	std::shared_ptr<WindowSurface> m_WindowSurface;
	std::shared_ptr<Swapchain> m_Swapchain;
	std::shared_ptr<CommandPool> m_CommandPool;
	std::vector<std::shared_ptr<FrameBuffer>> m_FrameBuffers;
	std::shared_ptr<DepthStencilBuffer> m_DepthStencilBuffer;
	std::shared_ptr<RenderPass> m_DefaultRenderPass;
	std::shared_ptr<CommandBuffer> m_InitCommandBuffer;
	std::shared_ptr<CommandBuffer> m_PreRenderCommandBuffer;
	std::shared_ptr<CommandBuffer> m_PostRenderCommandBuffer;
	std::shared_ptr<CommandBuffer> m_ClearCommandBuffer;
	std::shared_ptr<Queue> m_Queue;
};

}
}
}