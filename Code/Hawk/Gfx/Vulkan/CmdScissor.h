#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class CmdScissor
{
public:
	CmdScissor(const VkExtent2D& p_Extent);
	void Issue(VkCommandBuffer p_CommandBufferHandle) const;

private:
	VkRect2D m_Scissor;
};

	


}
}
}