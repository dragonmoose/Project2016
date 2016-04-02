#include "pch.h"
#include "CmdViewport.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan
{

CmdViewport::CmdViewport(const VkExtent2D& p_Extent)
: m_Viewport {}
{
	m_Viewport.width = static_cast<float>(p_Extent.width);
	m_Viewport.height = static_cast<float>(p_Extent.height);
	m_Viewport.minDepth = 0.0f;
	m_Viewport.maxDepth = 1.0f;
}

void CmdViewport::Issue(VkCommandBuffer p_CommandBufferHandle) const
{
	vkCmdSetViewport(p_CommandBufferHandle, 0, 1, &m_Viewport);
}



}
}
}