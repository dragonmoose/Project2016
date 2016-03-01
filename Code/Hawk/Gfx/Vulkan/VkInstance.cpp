#include "pch.h"
#include "VkInstance.h"

namespace Hawk {
namespace Gfx {

VkInstance::VkInstance()
{
	VkApplicationInfo l_AppInfo = {};
	l_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	l_AppInfo.pNext = nullptr; // must be null or pointer to an extension-specific structure
	l_AppInfo.pApplicationName = "HawkVulkanRenderer";
}

VkInstance::~VkInstance()
{
	ASSERT(m_Handle, "Handle NULL");
	//vkDestroyInstance(m_Handle, nullptr);
}

}
}