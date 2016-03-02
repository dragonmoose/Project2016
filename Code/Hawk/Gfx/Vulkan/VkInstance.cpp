#include "pch.h"
#include "VkInstance.h"
#include "VkUtil.h"
#include "VkSystem.h"
#include <array>

namespace Hawk {
namespace Gfx {

namespace
{
	std::vector<const char*> n_EnabledLayers = {};
	std::vector<const char*> n_EnabledExtensions = {};

#ifdef HAWK_DEBUG
	std::vector<const char*> n_EnabledDebugLayers = {
		"VK_LAYER_LUNARG_api_dump",
		"VK_LAYER_LUNARG_device_limits",
	};

	std::vector<const char*> n_EnabledDebugExtensions = {
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
	};
#endif
}

VkInstance::VkInstance()
: m_Handle(nullptr)
{
	std::vector<const char*> l_EnabledLayers = {};
	GetLayers(l_EnabledLayers);

	std::vector<const char*> l_EnabledExtensions = {};
	GetExtensions(l_EnabledExtensions);

	VkApplicationInfo l_AppInfo = {};
	l_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	l_AppInfo.pNext = nullptr; // must be null or pointer to an extension-specific structure
	l_AppInfo.pApplicationName = CoreInfo::GetAppName();
	l_AppInfo.applicationVersion = CoreInfo::GetAppVersion().GetID();
	l_AppInfo.pEngineName = CoreInfo::GetEngineName();
	l_AppInfo.engineVersion = CoreInfo::GetEngineVersion().GetID();
	l_AppInfo.apiVersion = VK_API_VERSION;

	VkInstanceCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	l_Info.pNext = nullptr; // must be null or pointer to an extension-specific structure
	l_Info.flags = 0;		// Reserved for future use
	l_Info.pApplicationInfo = &l_AppInfo;

	l_Info.enabledLayerCount = l_EnabledLayers.size();
	l_Info.ppEnabledLayerNames = l_EnabledLayers.data();

	l_Info.enabledExtensionCount = l_EnabledExtensions.size();
	l_Info.ppEnabledExtensionNames = l_EnabledExtensions.data();

	VK_THROW_IF_NOT_SUCCESS(vkCreateInstance(&l_Info, nullptr, &m_Handle), "Failed to create vulkan instance");
	LOG("VkInstance created", "vulkan", Debug);
}

VkInstance::~VkInstance()
{
	ASSERT(m_Handle, "Handle NULL");
	vkDestroyInstance(m_Handle, nullptr);
	LOG("VkInstance destroyed", "vulkan", Debug);
}

void VkInstance::GetLayers(std::vector<const char*>& p_Layers) const
{
	std::copy(n_EnabledLayers.begin(), n_EnabledLayers.end(), std::back_inserter(p_Layers));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugLayers.begin(), n_EnabledDebugLayers.end(), std::back_inserter(p_Layers));
#endif

	for (const auto& l_Layer : p_Layers)
	{
		THROW_IF_NOT(VkUtil::IsInstanceLayerAvailable(l_Layer), "Instance layer not available. Name=" << l_Layer);
	}
}

void VkInstance::GetExtensions(std::vector<const char*>& p_Extensions) const
{
	std::copy(n_EnabledExtensions.begin(), n_EnabledExtensions.end(), std::back_inserter(p_Extensions));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugExtensions.begin(), n_EnabledDebugExtensions.end(), std::back_inserter(p_Extensions));
#endif

	for (const auto& l_Layer : p_Extensions)
	{
		//THROW_IF_NOT(VkUtil::IsInstanceLayerAvailable(l_Layer), "Instance layer not available. Name=" << l_Layer);
	}
}

}
}