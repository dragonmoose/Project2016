#include "pch.h"
#include "VlkConstants.h"

namespace Hawk {
namespace Gfx {

const uint32_t VlkConstants::c_uiNumBackBuffers = 2;
const VkFormat VlkConstants::c_BackBufferFormat = VK_FORMAT_B8G8R8A8_UNORM;
const VkPresentModeKHR VlkConstants::c_PresentationMode = VK_PRESENT_MODE_FIFO_KHR;
const VkColorSpaceKHR VlkConstants::c_BackBufferColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

}
}