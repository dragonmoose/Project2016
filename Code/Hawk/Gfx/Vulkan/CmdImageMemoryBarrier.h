#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class CmdImageMemoryBarrier final
{
public:
	enum class TransferOp
	{
		Color_UndefinedToPresent,
		Color_WriteToPresent,
		Color_PresentToWrite,
		DepthStencil_UndefinedToWrite,
	};
	CmdImageMemoryBarrier(TransferOp p_TransferOp, VkImage p_Image = VK_NULL_HANDLE);

	void SetImage(VkImage p_Image);
	void Issue(VkCommandBuffer p_CommandBufferHandle) const;

private:
	void GetFlags(TransferOp p_TransferOp, VkImageLayout l_OldLayout, VkImageLayout l_NewLayout, VkAccessFlags l_SrcAccessMask, VkAccessFlags l_DestAccessMask, VkImageAspectFlags l_AspectMask) const;
	VkImageMemoryBarrier m_Barrier;
};

}
}
}