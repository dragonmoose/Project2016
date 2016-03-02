#include "pch.h"
#include "VkInstance.h"

namespace Hawk {
namespace Gfx {

VkInstance::VkInstance()
{
	VkApplicationInfo l_AppInfo = {};
	l_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	l_AppInfo.pNext = nullptr; // must be null or pointer to an extension-specific structure
	l_AppInfo.pApplicationName = "";
	l_AppInfo.applicationVersion = 0;
	l_AppInfo.pEngineName = Constants::c_EngineInfo.GetName().c_str();
	l_AppInfo.engineVersion = Constants::c_EngineInfo.GetVersionID();
	l_AppInfo.apiVersion = VK_API_VERSION;
}

VkInstance::~VkInstance()
{
	ASSERT(m_Handle, "Handle NULL");
	//vkDestroyInstance(m_Handle, nullptr);
}

}
}