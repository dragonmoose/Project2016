#include "pch.h"
#include "CmdScissor.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan
{

CmdScissor::CmdScissor(const VkExtent2D& p_Extent)
: m_Scissor {}
{
	m_Scissor.offset.x = 0;
	m_Scissor.offset.y = 0;
	m_Scissor.extent.width = p_Extent.width;
	m_Scissor.extent.height = p_Extent.height;
}

void CmdScissor::Issue(VkCommandBuffer p_CommandBufferHandle) const
{
	vkCmdSetScissor(p_CommandBufferHandle, 0, 1, &m_Scissor);
}



}
}
}