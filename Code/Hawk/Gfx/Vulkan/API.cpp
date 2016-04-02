#include "pch.h"
#include "API.h"
#include "Device.h"
#include "PhysicalDevice.h"
#include "System.h"
#include "WindowSurface.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "ShaderModule.h"
#include "CmdImageMemoryBarrier.h"
#include "GraphicsPipeline.h"
#include "Console/ScopedConsoleCommands.h"
#include "Constants.h"
#include "CmdViewport.h"
#include "CmdScissor.h"
#include "CmdRenderPass.h"
#include "Gfx/Color.h"
#include "Util/Random.h"			// test

namespace Hawk {
namespace Gfx {
namespace Vulkan {

API::~API()
{
	m_Device->WaitUntilIdle();
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
	m_ShaderManager = std::make_shared<ShaderManager>(m_Device);
	CreateRenderPasses();
	CreateDepthStencilBuffer();
	CreateFrameBuffers();
	CreateGPUWorkManager();

	GraphicsPipeline tst_Pipeline(m_Device, m_DefaultRenderPass.get(), m_ShaderManager.get());

	CreateCommandBuffers();
	PrepareRendering();

	//SetFullscreenState(Config::Instance().Get("gfx.fullscreen", false));
	LOG("Vulkan initialized", "vulkan", Info);
}

void API::Render()
{
	m_Swapchain->AcquireNextImage(m_NextImageSemaphore.get());

	m_GPUWorkManager->SubmitQueued(m_Swapchain->GetCurrImageIndex());
	m_Swapchain->Present();
	m_GPUWorkManager->WaitUntilIdle();		// TODO: Is there a better way to do this?
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
	m_GPUWorkManager->CreateBatch("SetupBatch", false);
	std::vector<CommandBufferBatch*> l_Batches;
	l_Batches.resize(Constants::c_uiNumBackBuffers);
	for (uint32 i = 0; i < Constants::c_uiNumBackBuffers; i++)
	{
		CommandBufferBatch* l_Batch = m_GPUWorkManager->GetBatch("SetupBatch", i);
		l_Batches[i] = l_Batch;

		CommandBuffer* l_Buffer = l_Batch->CreateBuffer("SetupBuffer", true);
		l_Buffer->Begin();
		l_Buffer->Issue(CmdImageMemoryBarrier(CmdImageMemoryBarrier::TransferOp::Color_UndefinedToPresent, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_Swapchain->GetImage(i)));
		if (i == 0)
		{
			l_Buffer->Issue(CmdImageMemoryBarrier(CmdImageMemoryBarrier::TransferOp::DepthStencil_UndefinedToWrite, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_DepthStencilBuffer->GetImage()));
		}
		l_Buffer->End();
	}
	m_GPUWorkManager->Submit(l_Batches);
}

void API::CreateGPUWorkManager()
{
	GPUWorkManagerInitInfo l_Info;
	l_Info.m_Device = m_Device;
	l_Info.m_Queue = m_Device->GetQueue(QueueType::GraphicsPresentation, 0);
	l_Info.m_bAllowIndividualBufferReset = true;
	l_Info.m_uiNumCopies = Constants::c_uiNumBackBuffers;
	m_GPUWorkManager = std::make_unique<GPUWorkManager>(l_Info);
}

void API::CreateCommandBuffers()
{
	m_GPUWorkManager->CreateBatch("PreRenderBatch", true);
	m_GPUWorkManager->CreateBatch("ClearBatch", true);
	m_GPUWorkManager->CreateBatch("PostRenderBatch", true);

	for (uint32 i = 0; i < Constants::c_uiNumBackBuffers; i++)
	{
		{
			CommandBufferBatch* l_Batch = m_GPUWorkManager->GetBatch("PreRenderBatch", i);
			l_Batch->AddWaitSemaphore(m_NextImageSemaphore.get(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

			CommandBuffer* l_Buffer = l_Batch->CreateBuffer("PreRenderBuffer", false);
			l_Buffer->Begin();
			l_Buffer->Issue(CmdImageMemoryBarrier(CmdImageMemoryBarrier::TransferOp::Color_PresentToWrite, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, m_Swapchain->GetImage(i)));
			l_Buffer->End();
		}
		{
			CommandBufferBatch* l_Batch = m_GPUWorkManager->GetBatch("ClearBatch", i);
			CommandBuffer* l_Buffer = l_Batch->CreateBuffer("ClearBuffer", false);
			l_Buffer->Begin();

			VkExtent2D l_Extent = m_Swapchain->GetExtent();

			CmdRenderPass l_CmdRenderPass(m_DefaultRenderPass.get(), m_FrameBuffers[i].get(), l_Extent, Color::Magenta);
			l_CmdRenderPass.Begin(l_Buffer->GetHandle());

			l_Buffer->Issue(CmdViewport(l_Extent));
			l_Buffer->Issue(CmdScissor(l_Extent));

			l_CmdRenderPass.End(l_Buffer->GetHandle());

			// end pass

			l_Buffer->End();
		}
		{
			CommandBufferBatch* l_Batch = m_GPUWorkManager->GetBatch("PostRenderBatch", i);
			CommandBuffer* l_Buffer = l_Batch->CreateBuffer("PostRender", false);
			l_Buffer->Begin();
			l_Buffer->Issue(CmdImageMemoryBarrier(CmdImageMemoryBarrier::TransferOp::Color_WriteToPresent, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, m_Swapchain->GetImage(i)));
			l_Buffer->End();
		}
	}
}

const std::string& API::GetLogDesc()
{
	static const std::string l_Desc("vulkan");
	return l_Desc;
}

}
}
}