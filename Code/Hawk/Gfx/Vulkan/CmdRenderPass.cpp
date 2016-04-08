#include "pch.h"
#include "CmdRenderPass.h"
#include "RenderPass.h"
#include "FrameBuffer.h"
#include "Gfx/Color.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

CmdRenderPass::CmdRenderPass(const RenderPass* p_RenderPass, const FrameBuffer* p_FrameBuffer, VkExtent2D p_Extent)
: m_Info {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO}
{
	m_Info.renderPass = p_RenderPass->GetHandle();
	m_Info.framebuffer = p_FrameBuffer->GetHandle();
	m_Info.renderArea.extent = p_Extent;
}

CmdRenderPass::CmdRenderPass(const RenderPass* p_RenderPass, const FrameBuffer* p_FrameBuffer, VkExtent2D p_Extent, const Color& p_ClearColor)
: CmdRenderPass(p_RenderPass, p_FrameBuffer, p_Extent)
{
	// TODO: Adapt for different render pass attachment configurations
	m_ClearValues.resize(2);
	m_ClearValues[0].depthStencil.depth = 1.0f;
	m_ClearValues[0].depthStencil.stencil = 0;
	m_ClearValues[1].color.float32[0] = p_ClearColor.R();
	m_ClearValues[1].color.float32[1] = p_ClearColor.G();
	m_ClearValues[1].color.float32[2] = p_ClearColor.B();
	m_ClearValues[1].color.float32[3] = p_ClearColor.A();
	m_Info.pClearValues = m_ClearValues.data();
	m_Info.clearValueCount = 2;
}

void CmdRenderPass::Begin(VkCommandBuffer p_CommandBufferHandle)
{
	// TODO: Support secondary buffers
	vkCmdBeginRenderPass(p_CommandBufferHandle, &m_Info, VK_SUBPASS_CONTENTS_INLINE);
}

void CmdRenderPass::End(VkCommandBuffer p_CommandBufferHandle)
{
	vkCmdEndRenderPass(p_CommandBufferHandle);
}


}
}
}