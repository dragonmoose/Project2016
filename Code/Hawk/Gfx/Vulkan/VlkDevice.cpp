#include "pch.h"
#include "VlkDevice.h"

namespace Hawk {
namespace Gfx {

VlkDevice::VlkDevice(VkInstance p_Instance)
{
	LOG("Created auto-selected vulkan device", "vulkan", Info);
}

VlkDevice::VlkDevice(VkInstance p_Instance, uint32_t p_uiDeviceID)
{
	LOG("Created vulkan device with ID=" << p_uiDeviceID, "vulkan", Info);
}

VlkDevice::~VlkDevice()
{
	LOG("Vulkan device destroyed", "vulkan", Debug);
}

}
}