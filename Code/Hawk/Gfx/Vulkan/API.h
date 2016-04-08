#pragma once
#include "Gfx/IRenderingAPI.h"
#include "Instance.h"
#include "Device.h"
#include "System.h"
#include "WindowSurface.h"
#include "Swapchain.h"
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "DepthStencilBuffer.h"
#include "GPUWorkManager.h"	// TODO: Fix so that these can be forward declared (some parse problem with RenderModule)
#include "Semaphore.h"
#include "ShaderManager.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GraphicsPipeline.h"
#include "DescriptorSet.h"
#include "UniformBuffer.h"
#include "UniformBufferDecl.h"
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
	void Update(Duration p_Duration);

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
	void PrepareRendering();
	void CreateGPUWorkManager();
	void CreateCommandBuffers();
	static const std::string& GetLogDesc();

	std::shared_ptr<Instance> m_Instance;
	std::shared_ptr<PhysicalDevice> m_PhysicalDevice;
	std::shared_ptr<Device> m_Device;
	std::shared_ptr<WindowSurface> m_WindowSurface;
	std::shared_ptr<Swapchain> m_Swapchain;
	std::vector<std::shared_ptr<FrameBuffer>> m_FrameBuffers;
	std::shared_ptr<DepthStencilBuffer> m_DepthStencilBuffer;
	std::shared_ptr<RenderPass> m_DefaultRenderPass;
	std::unique_ptr<GPUWorkManager> m_GPUWorkManager;
	std::unique_ptr<Semaphore> m_NextImageSemaphore;
	std::shared_ptr<ShaderManager> m_ShaderManager;
	std::shared_ptr<VertexBuffer> m_VertexBuffer;
	std::shared_ptr<IndexBuffer> m_IndexBuffer;
	std::shared_ptr<GraphicsPipeline> m_Pipeline;
	std::shared_ptr<DescriptorSet> m_DescriptorSet;
	std::shared_ptr<PipelineLayout> m_PipelineLayout;
	std::unique_ptr<UniformBuffer<UniformBufferDecl::WVP>> m_UniformBuffer;
};

}
}
}