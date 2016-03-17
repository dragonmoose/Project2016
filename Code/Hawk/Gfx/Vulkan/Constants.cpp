#include "pch.h"
#include "Constants.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

const uint32 Constants::c_uiNumBackBuffers = 2;
const VkFormat Constants::c_BackBufferFormat = VK_FORMAT_B8G8R8A8_UNORM;
const VkPresentModeKHR Constants::c_PresentationMode = VK_PRESENT_MODE_FIFO_KHR;
const VkColorSpaceKHR Constants::c_BackBufferColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

}
}
}