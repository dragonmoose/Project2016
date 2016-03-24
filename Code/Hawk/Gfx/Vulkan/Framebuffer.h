#pragma once
#include "System.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class RenderPass;
class ImageView;

class FrameBuffer final
{
public:
	FrameBuffer(std::shared_ptr<Device> p_Device, const RenderPass* p_RenderPass, const ImageView* p_ColorView, const ImageView* p_DepthStencilView, VkExtent2D p_Extent);
	~FrameBuffer();
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	VkFramebuffer GetHandle() const;

private:
	std::shared_ptr<Device> m_Device;
	VkFramebuffer m_Handle;
};

}
}
}