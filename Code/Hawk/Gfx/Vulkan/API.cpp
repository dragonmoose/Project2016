#include "pch.h"
#include "API.h"
#include "Device.h"
#include "PhysicalDevice.h"
#include "System.h"
#include "WindowSurface.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "CmdImageMemoryBarrier.h"
#include "Console/ScopedConsoleCommands.h"
#include "Constants.h"
#include "Util/Random.h"			// test

namespace Hawk {
namespace Gfx {
namespace Vulkan {

API::~API()
{
	LOG("API destroyed", "vulkan", Debug);
}

void API::Initialize()
{
	System::Initialize();
	CreateInstance();
	CreatePhysicalDevice();

	DeviceCreateInfo l_CreateInfo(m_PhysicalDevice);
	SetupQueues(l_CreateInfo);
	l_CreateInfo.Finalize();

	CreateDevice(l_CreateInfo);
	CreateSwapchain();
	CreateRenderPasses();
	CreateDepthStencilBuffer();
	CreateFrameBuffers();
	CreateGPUWorkManager();

	CreateCommandBuffers();
	PrepareRendering();

	//SetFullscreenState(Config::Instance().Get("gfx.fullscreen", false));
	LOG("Vulkan initialized", "vulkan", Info);
}

void API::Render()
{
	m_Swapchain->AcquireNextImage(m_NextImageSemaphore.get());

	CommandBuffer* l_PrepareBuffer = m_GPUWorkManager->GetBatch("PreRenderBatch")->GetBuffer("PreRenderBuffer");
	l_PrepareBuffer->Begin();
	l_PrepareBuffer->Issue(CmdImageMemoryBarrier(CmdImageMemoryBarrier::TransferOp::Color_PresentToWrite, m_Swapchain->GetCurrImage()));
	l_PrepareBuffer->End();

	CommandBuffer* l_ClearBuffer = m_GPUWorkManager->GetBatch("ClearBatch")->GetBuffer("ClearBuffer");
	l_ClearBuffer->Begin();

	VkRenderPassBeginInfo l_RenderPassBeginInfo = {};
	l_RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	l_RenderPassBeginInfo.renderPass = m_DefaultRenderPass->GetHandle();
	l_RenderPassBeginInfo.framebuffer = m_FrameBuffers[m_Swapchain->GetCurrImageIndex()]->GetHandle();
	l_RenderPassBeginInfo.renderArea.extent = m_Swapchain->GetExtent();
	l_RenderPassBeginInfo.clearValueCount = 2;

	std::array<VkClearValue, 2> l_ClearValues;
	l_ClearValues[0].depthStencil.depth = 0.0f;
	l_ClearValues[0].depthStencil.stencil = 0;
	l_ClearValues[1].color.float32[0] = 0.0f;
	l_ClearValues[1].color.float32[1] = 0.0f;
	l_ClearValues[1].color.float32[2] = Random::GetFloat(0.0f, 0.5f);
	l_ClearValues[1].color.float32[3] = 1.0f;
	l_RenderPassBeginInfo.pClearValues = l_ClearValues.data();

	vkCmdBeginRenderPass(l_ClearBuffer->GetHandle(), &l_RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdEndRenderPass(l_ClearBuffer->GetHandle());
	l_ClearBuffer->End();

	CommandBuffer* l_PostBuffer = m_GPUWorkManager->GetBatch("PostRenderBatch")->GetBuffer("PostRenderBuffer");
	l_PostBuffer->Begin();
	l_PostBuffer->Issue(CmdImageMemoryBarrier(CmdImageMemoryBarrier::TransferOp::Color_WriteToPresent, m_Swapchain->GetCurrImage()));
	l_PostBuffer->End();

	m_GPUWorkManager->SubmitQueued();
	m_Swapchain->Present();
}

void API::SetFullscreenState(bool /*p_bState*/)
{
}

void API::OnWindowSizeChanged(UINT32 /*p_uiWidth*/, UINT32 /*p_uiHeight*/)
{
}

void API::SetDebugText(const std::string& /*p_Text*/)
{
}

#ifdef HAWK_DEBUG
void API::InitializeConsole(ScopedConsoleCommands* p_Console)
{
	p_Console->Register("vk.printInstanceLayers", &Instance::CmdPrintLayers, "Lists Vulkan instance layers", "KeepUnsupported [0|1]");
	p_Console->Register("vk.printInstanceExtensions", &Instance::CmdPrintExtensions, "Lists Vulkan instance extensions", "KeepUnsupported [0|1]");
	p_Console->Register("vk.printDevices", m_PhysicalDevice.get(), &PhysicalDevice::CmdPrintDevices, "Lists all available devices", "");
	p_Console->Register("vk.printQueueFamilies", m_PhysicalDevice.get(), &PhysicalDevice::CmdPrintQueueFamilies, "Lists queue families for the given device", "");
	p_Console->Register("vk.printDeviceLayers", m_PhysicalDevice.get(), &PhysicalDevice::CmdPrintLayers, "Lists Vulkan device layers for the given device", "[DeviceIndex] [KeepUnsupported 0|1]");
	p_Console->Register("vk.printDeviceExtensions", m_PhysicalDevice.get(), &PhysicalDevice::CmdPrintExtensions, "Lists Vulkan device extensions for the given device", "[DeviceIndex] [KeepUnsupported 0|1]");
	p_Console->Register("vk.printSupportedDepthStencilFormats", m_PhysicalDevice.get(), &PhysicalDevice::CmdPrintDepthStencilFormats, "Lists the supported depth/stencil formats for the given device", "[DeviceIndex] [KeepUnsupported 0|1]");
}

#endif

void API::CreateInstance()
{
	m_Instance = std::make_shared<Instance>();
}

void API::CreatePhysicalDevice()
{
	uint32 l_uiDeviceID = 0;
	if (Config::Instance().TryGet<uint32>("vulkan.deviceID", 0, l_uiDeviceID))
	{
		m_PhysicalDevice = std::make_shared<PhysicalDevice>(m_Instance, l_uiDeviceID);
	}
	else
	{
		m_PhysicalDevice = std::make_shared<PhysicalDevice>(m_Instance);
	}
}

void API::SetupQueues(DeviceCreateInfo& p_CreateInfo)
{
	p_CreateInfo.AddQueue(QueueType::GraphicsPresentation, 0, 100);
	p_CreateInfo.AddQueue(QueueType::Graphics, 0, 100);
	p_CreateInfo.AddQueue(QueueType::Graphics, 1, 150);
	p_CreateInfo.AddQueue(QueueType::Graphics, 2, 50);
	p_CreateInfo.AddQueue(QueueType::Compute, 1, 100);
	p_CreateInfo.AddQueue(QueueType::Compute, 0, 25);
}

void API::CreateDevice(const DeviceCreateInfo& p_CreateInfo)
{
	ASSERT(m_Instance, "Instance null");
	ASSERT(m_PhysicalDevice, "PhysicalDevice null");
	m_Device = std::make_shared<Device>(p_CreateInfo);
}

void API::CreateSwapchain()
{
	m_Swapchain = std::make_shared<Swapchain>(m_Instance, m_Device);
	m_NextImageSemaphore = std::make_unique<Semaphore>(m_Device);
}

void API::CreateRenderPasses()
{
	m_DefaultRenderPass = std::make_shared<RenderPass>(m_Device);
}

void API::CreateFrameBuffers()
{
	m_FrameBuffers.resize(Constants::c_uiNumBackBuffers);
	for (uint32 i = 0; i < Constants::c_uiNumBackBuffers; i++)
	{
		m_FrameBuffers[i] = std::make_shared<FrameBuffer>(m_Device, m_DefaultRenderPass.get(), m_Swapchain->GetImageView(i).get(), m_DepthStencilBuffer->GetImageView().get(), m_Swapchain->GetExtent());
	}
}

void API::CreateDepthStencilBuffer()
{
	m_DepthStencilBuffer = std::make_shared<DepthStencilBuffer>(m_Device, m_Swapchain->GetExtent());
}

void API::PrepareRendering()
{
	CommandBufferBatch* l_Batch = m_GPUWorkManager->CreateBatch("SetupBatch", false);
	CommandBuffer* l_Buffer = l_Batch->CreateBuffer("SetupBuffer", true);

	l_Buffer->Begin();
	for (uint32 i = 0; i < Constants::c_uiNumBackBuffers; i++)
	{
		l_Buffer->Issue(CmdImageMemoryBarrier(CmdImageMemoryBarrier::TransferOp::Color_UndefinedToPresent, m_Swapchain->GetImage(i)));
	}
	l_Buffer->Issue(CmdImageMemoryBarrier(CmdImageMemoryBarrier::TransferOp::DepthStencil_UndefinedToWrite, m_DepthStencilBuffer->GetImage()));
	l_Buffer->End();
	m_GPUWorkManager->Submit(l_Batch);
}

void API::CreateGPUWorkManager()
{
	GPUWorkManagerInitInfo l_Info;
	l_Info.m_Device = m_Device;
	l_Info.m_Queue = m_Device->GetQueue(QueueType::GraphicsPresentation, 0);
	l_Info.m_bAllowIndividualBufferReset = true;
	m_GPUWorkManager = std::make_unique<GPUWorkManager>(l_Info);
}

void API::CreateCommandBuffers()
{
	CommandBufferBatch* l_PreRenderBatch = m_GPUWorkManager->CreateBatch("PreRenderBatch", true);
	l_PreRenderBatch->AddWaitSemaphore(m_NextImageSemaphore.get(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
	l_PreRenderBatch->CreateBuffer("PreRenderBuffer", true);

	m_GPUWorkManager->CreateBatch("ClearBatch", true)->CreateBuffer("ClearBuffer", true);
	m_GPUWorkManager->CreateBatch("PostRenderBatch", true)->CreateBuffer("PostRenderBuffer", true);
}

const std::string& API::GetLogDesc()
{
	static const std::string l_Desc("vulkan");
	return l_Desc;
}

}
}
}