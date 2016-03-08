#include "pch.h"
#include "VlkInstance.h"
#include "VlkInstanceUtil.h"
#include "VlkSystem.h"
#include <array>
#include <functional>

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

	VkDebugReportFlagsEXT c_ReportFlags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT |
		VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
#endif
}

VlkInstance::VlkInstance()
: m_Instance(nullptr)
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
	l_AppInfo.apiVersion = VlkSystem::GetAPIVersion();

	VkInstanceCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	l_Info.pNext = nullptr; // must be null or pointer to an extension-specific structure
	l_Info.flags = 0;		// Reserved for future use
	l_Info.pApplicationInfo = &l_AppInfo;

	l_Info.enabledLayerCount = l_EnabledLayers.size();
	l_Info.ppEnabledLayerNames = l_EnabledLayers.data();

	l_Info.enabledExtensionCount = l_EnabledExtensions.size();
	l_Info.ppEnabledExtensionNames = l_EnabledExtensions.data();

	VK_THROW_IF_NOT_SUCCESS(vkCreateInstance(&l_Info, nullptr, &m_Instance), "Failed to create vulkan instance");
	LOG("VlkInstance created", "vulkan", Debug);

#ifdef HAWK_DEBUG
	RetrieveDebugCallbacks();
	CreateDebugReportCallback();
#endif
}

VlkInstance::~VlkInstance()
{
	ASSERT(m_Instance, "Handle NULL");
#ifdef HAWK_DEBUG
	DestroyDebugReportCallback();
#endif
	vkDestroyInstance(m_Instance, nullptr);
	LOG("VlkInstance destroyed", "vulkan", Debug);
}

VkInstance VlkInstance::GetRawInstance() const
{
	return m_Instance;
}

void VlkInstance::GetLayers(std::vector<const char*>& p_Layers) const
{
	std::copy(n_EnabledLayers.begin(), n_EnabledLayers.end(), std::back_inserter(p_Layers));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugLayers.begin(), n_EnabledDebugLayers.end(), std::back_inserter(p_Layers));
#endif

	for (const auto& l_Layer : p_Layers)
	{
		THROW_IF_NOT(VlkInstanceUtil::IsLayerAvailable(l_Layer), "Instance layer not available. Name=" << l_Layer);
	}
}

void VlkInstance::GetExtensions(std::vector<const char*>& p_Extensions) const
{
	std::copy(n_EnabledExtensions.begin(), n_EnabledExtensions.end(), std::back_inserter(p_Extensions));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugExtensions.begin(), n_EnabledDebugExtensions.end(), std::back_inserter(p_Extensions));
#endif

	for (const auto& l_Extension : p_Extensions)
	{
		THROW_IF_NOT(VlkInstanceUtil::IsExtensionAvailable(l_Extension), "Global instance extension not available. Name=" << l_Extension);
	}
}

#ifdef HAWK_DEBUG
void VlkInstance::RetrieveDebugCallbacks()
{
	m_CreateDebugReport = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugReportCallbackEXT");
	THROW_IF_NOT(m_CreateDebugReport, "Failed to create CreateDebugReport vulkan extension");

	m_DestroyDebugReport = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugReportCallbackEXT");
	THROW_IF_NOT(m_DestroyDebugReport, "Failed to create DestroyDebugReport vulkan extension");
}

namespace
{
	VkBool32 VKAPI_CALL OnDebugReport(VkDebugReportFlagsEXT p_Flags, VkDebugReportObjectTypeEXT p_ObjectType, uint64_t p_uiObject, size_t p_Location, int32_t p_iMessageCode, const char* p_pLayerPrefix, const char* p_pMessage, void* /*p_pUserData*/)
	{
		std::ostringstream l_Msg;
		l_Msg << p_pLayerPrefix << ": '" << p_pMessage << "' [ObjType=" << p_ObjectType << " Obj=" << p_uiObject << " Loc=" << p_Location << " Code=" << p_iMessageCode << "]";
		if (p_Flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
		{
			LOG(l_Msg.str(), "vkdbg", Info);
		}
		else if (p_Flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
		{
			LOG(l_Msg.str(), "vkdbg", Warning);
		}
		else if (p_Flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
		{
			LOG("#PEFORMANCE WARNING# " << l_Msg.str(), "vkdbg", Warning);
		}
		else if (p_Flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
		{
			LOG(l_Msg.str(), "vkdbg", Debug);
		}
		else if (p_Flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
		{
			LOG(l_Msg.str(), "vkdbg", Error);
		}
		return false; // Returning false here will yield the same behavior as without validation layers
	}
}

void VlkInstance::CreateDebugReportCallback()
{
	VkDebugReportCallbackCreateInfoEXT l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	l_Info.pNext = nullptr;
	l_Info.flags = c_ReportFlags;
	l_Info.pfnCallback = OnDebugReport;
	l_Info.pUserData = nullptr;
	m_CreateDebugReport(m_Instance, &l_Info, nullptr, &m_DebugReportHandle);
}

void VlkInstance::DestroyDebugReportCallback()
{
	m_DestroyDebugReport(m_Instance, m_DebugReportHandle, nullptr);
}
#endif

}
}