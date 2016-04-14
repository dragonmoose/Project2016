#include "pch.h"
#include "Constants.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

const uint32 Constants::c_uiNumBackBuffers = 2;
const VkFormat Constants::c_PreferredBackBufferColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
const VkFormat Constants::c_PreferredDepthBufferFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
const VkPresentModeKHR Constants::c_PresentationMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
const VkColorSpaceKHR Constants::c_BackBufferColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

}
}
}