#pragma once
#include "System.h"
#include <vector>

namespace Hawk {
namespace Gfx {

class Color;

namespace Vulkan {

class RenderPass;
class FrameBuffer;

class CmdRenderPass final
{
public:
	CmdRenderPass(const RenderPass* p_RenderPass, const FrameBuffer* p_FrameBuffer, VkExtent2D p_Extent);
	CmdRenderPass(const RenderPass* p_RenderPass, const FrameBuffer* p_FrameBuffer, VkExtent2D p_Extent, const Color& p_ClearColor);

	void Begin(VkCommandBuffer p_CommandBufferHandle);
	void End(VkCommandBuffer p_CommandBufferHandle);

private:
	VkRenderPassBeginInfo m_Info;
	std::vector<VkClearValue> m_ClearValues;
};

	


}
}
}