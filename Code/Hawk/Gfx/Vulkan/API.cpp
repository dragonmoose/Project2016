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
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "PipelineLayout.h"
#include "System/Duration.h"
#include "Gfx/Color.h"
#include "Util/Random.h"			// test
#include "Util/Math.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

API::~API()
{
	if (m_Device)
	{
		m_Device->WaitUntilIdle();
	}
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

	std::vector<VkDescriptorPoolSize> l_Sizes(1);
	l_Sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	l_Sizes[0].descriptorCount = 1;
	std::shared_ptr<DescriptorPool> l_Pool = std::make_shared<DescriptorPool>(m_Device, true, 5, l_Sizes);

	std::vector<VkDescriptorSetLayoutBinding> l_Bindings(1);
	l_Bindings[0] = {};
	l_Bindings[0].binding = 0;
	l_Bindings[0].descriptorCount = 1;
	l_Bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	l_Bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	DescriptorSetLayout l_SetLayout(m_Device, l_Bindings);

	std::vector<VkDescriptorSetLayout> l_Layouts(1);
	l_Layouts[0] = l_SetLayout.GetHandle();

	m_PipelineLayout = std::make_shared<PipelineLayout>(m_Device, l_Layouts);
	m_DescriptorSet = std::make_shared<DescriptorSet>(m_Device, l_Pool, m_PipelineLayout, &l_SetLayout);

	m_Pipeline = std::make_shared<GraphicsPipeline>(m_Device, m_DefaultRenderPass.get(), m_ShaderManager.get(), m_PipelineLayout.get());

	VertexDecl::PosColorVertices l_Vertices =
	{
		{{-1.0f, -1.0f, 1.0f},	{1.0f, 0.0f, 0.0f}}, // Back
		{{1.0f, -1.0f, 1.0f},	{1.0f, 0.0f, 0.0f}},
		{{1.0f, 1.0f, 1.0f},	{1.0f, 0.0f, 0.0f}},
		{{-1.0f, 1.0f, 1.0f},	{1.0f, 0.0f, 0.0f}},

		{{-1.0f, -1.0f, -1.0f},	{0.0f, 0.0f, 1.0f}}, // Front
		{{1.0f, -1.0f, -1.0f},	{0.0f, 0.0f, 1.0f}},
		{{1.0f, 1.0f, -1.0f},	{0.0f, 0.0f, 1.0f}},
		{{-1.0f, 1.0f, -1.0f},	{0.0f, 0.0f, 1.0f}}
	};
	m_VertexBuffer = std::make_shared<VertexBuffer>(m_Device, l_Vertices);

	std::vector<uint16> l_Indices = {
		0, 3, 1, 1, 3, 2, // Back
		7, 4, 6, 6, 4, 5, // Front
		3, 0, 7, 7, 0, 4, // Left
		6, 5, 2, 2, 5, 1, // Right
		3, 7, 2, 2, 7, 6, // Top
		4, 0, 5, 5, 0, 1  // Bottom
	};
	m_IndexBuffer = std::make_shared<IndexBuffer>(m_Device, l_Indices);

	m_UniformBuffer = std::make_unique<UniformBuffer<UniformBufferDecl::WVP>>(m_Device);

	UniformBufferDecl::WVP& l_WVP = m_UniformBuffer->GetData();
	l_WVP.m_Proj = glm::perspective(Math::Deg2Rad(60.0f), m_Swapchain->GetAspectRatio(), 0.1f, 256.0f);
	l_WVP.m_View = glm::translate(glm::mat4(), glm::vec3(0.0f, 1.0f, -10.0f));

	// define your up vector
	glm::vec3 upVector = glm::vec3(0, 1, 0);
	// rotate around to a given bearing: yaw
	glm::mat4 camera = glm::rotate(glm::mat4(), 20.0f, upVector);
	// Define the 'look up' axis, should be orthogonal to the up axis
	glm::vec3 pitchVector = glm::vec3(1, 0, 0);
	// rotate around to the required head tilt: pitch
	camera = glm::rotate(camera, Math::Deg2Rad(20.0f), pitchVector);
	camera *= glm::translate(glm::mat4(), glm::vec3(0.0f, 1.0f, 5.0f));

	// now get the view matrix by taking the camera inverse
	l_WVP.m_View = glm::inverse(camera);

	l_WVP.m_World = glm::mat4();

	m_UniformBuffer->Update();
	m_DescriptorSet->Bind(m_UniformBuffer->GetDescriptorBufferInfo(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0); // TODO: Improve connections between these classes

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

void API::Update(Duration p_Duration)
{
	static float l_fAngle = 0.0f;
	UniformBufferDecl::WVP& l_WVP = m_UniformBuffer->GetData();
	l_fAngle += p_Duration.GetSecondsFloat();
	l_WVP.m_World = glm::rotate(glm::mat4(), l_fAngle, glm::vec3(0, 1, 1));
	m_UniformBuffer->Update();
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

void API::DrawAABB(const Vec3& p_Min, const Vec3& p_Max, const Color& p_Color, uint32 p_uiLineWidth, bool p_bDepthTest)
{
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
	/*p_CreateInfo.AddQueue(QueueType::Graphics, 0, 100);
	p_CreateInfo.AddQueue(QueueType::Graphics, 1, 150);
	p_CreateInfo.AddQueue(QueueType::Graphics, 2, 50);
	p_CreateInfo.AddQueue(QueueType::Compute, 1, 100);
	p_CreateInfo.AddQueue(QueueType::Compute, 0, 25);*/
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

			CmdRenderPass l_CmdRenderPass(m_DefaultRenderPass.get(), m_FrameBuffers[i].get(), l_Extent, Color::Black);
			l_CmdRenderPass.Begin(l_Buffer->GetHandle());

			l_Buffer->Issue(CmdViewport(l_Extent));
			l_Buffer->Issue(CmdScissor(l_Extent));

			vkCmdBindPipeline(l_Buffer->GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetHandle());

			l_Buffer->Issue(*m_DescriptorSet.get());
			l_Buffer->Issue(*m_VertexBuffer.get());
			l_Buffer->Issue(*m_IndexBuffer.get());
			vkCmdDrawIndexed(l_Buffer->GetHandle(), m_IndexBuffer->GetCount(), 1, 0, 0, 0);
			
			l_CmdRenderPass.End(l_Buffer->GetHandle());

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