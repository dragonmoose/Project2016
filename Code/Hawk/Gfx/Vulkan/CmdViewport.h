#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class CmdViewport
{
public:
	CmdViewport(const VkExtent2D& p_Extent);
	void Issue(VkCommandBuffer p_CommandBufferHandle) const;

private:
	VkViewport m_Viewport;
};

	


}
}
}