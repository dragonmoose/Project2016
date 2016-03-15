#include "pch.h"
#include "VlkInstance.h"
#include "VlkSystem.h"
#include "VlkUtil.h"
#include "Util/Algorithm.h"
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
: m_Instance(VK_NULL_HANDLE)
{
	std::vector<const char*> l_EnabledLayers = {};
	GetLayersToCreate(l_EnabledLayers);

	std::vector<const char*> l_EnabledExtensions = {};
	GetExtensionsToCreate(l_EnabledExtensions);

	VkApplicationInfo l_AppInfo = {};
	l_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	l_AppInfo.pApplicationName = CoreInfo::GetAppName();
	l_AppInfo.applicationVersion = CoreInfo::GetAppVersion().GetID();
	l_AppInfo.pEngineName = CoreInfo::GetEngineName();
	l_AppInfo.engineVersion = CoreInfo::GetEngineVersion().GetID();
	l_AppInfo.apiVersion = VlkSystem::GetAPIVersion();

	VkInstanceCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	l_Info.pApplicationInfo = &l_AppInfo;

	l_Info.enabledLayerCount = l_EnabledLayers.size();
	l_Info.ppEnabledLayerNames = l_EnabledLayers.data();

	l_Info.enabledExtensionCount = l_EnabledExtensions.size();
	l_Info.ppEnabledExtensionNames = l_EnabledExtensions.data();

	VK_THROW_IF_NOT_SUCCESS(vkCreateInstance(&l_Info, nullptr, &m_Instance), "Failed to create vulkan instance for API-version: " << VlkUtil::SpecVersionToString(l_Info.pApplicationInfo->apiVersion));
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

VkInstance VlkInstance::GetHandle() const
{
	return m_Instance;
}

void VlkInstance::CmdPrintLayers(bool p_bKeepUnsupported)
{
	LayerProperties_t l_Layers;
	GetAllLayers(l_Layers, p_bKeepUnsupported);

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	for (const auto& l_Layer : l_Layers)
	{
		if (l_Layer.specVersion > VlkSystem::GetAPIVersion())
		{
			std::cout << "!Requires API " << VlkUtil::SpecVersionToString(l_Layer.specVersion) << "!";
		}

		std::cout << l_Layer.layerName << " (" << l_Layer.description << ")" <<
			" SpecVersion: " << VlkUtil::SpecVersionToString(l_Layer.specVersion) <<
			" ImplementationVersion: " << l_Layer.implementationVersion << "\n";
	}
	std::cout << "\n";
}

void VlkInstance::CmdPrintExtensions(bool p_bKeepUnsupported)
{
	CONSOLE_WRITE_SCOPE();

	std::cout << "\n-Global extensions-----------------------\n";
	ExtensionProperties_t l_Extensions;
	GetAllExtensions(l_Extensions);
	for (const auto& l_Extension : l_Extensions)
	{
		std::cout << l_Extension.extensionName << " Version: " << l_Extension.specVersion << "\n";
	}
	std::cout << "\n-Layer extensions------------------------\n";

	LayerProperties_t l_Layers;
	GetAllLayers(l_Layers, p_bKeepUnsupported);

	for (const auto& l_Layer : l_Layers)
	{
		l_Extensions.clear();
		GetAllExtensions(l_Extensions, l_Layer.layerName);
		if (!l_Extensions.empty())
		{
			if (l_Layer.specVersion > VlkSystem::GetAPIVersion())
			{
				std::cout << "## N/A - requires API " << VlkUtil::SpecVersionToString(l_Layer.specVersion) << " ## ";
			}
			std::cout << l_Layer.layerName << ":\n";
			for (const auto& l_Extension : l_Extensions)
			{
				std::cout << "\t" << l_Extension.extensionName << " Version: " << l_Extension.specVersion << "\n";
			}
			std::cout << "\n";
		}
	}
	std::cout << "\n";
}

bool VlkInstance::IsLayerAvailable(const std::string& p_Name)
{
	LayerProperties_t l_Layers;
	GetAllLayers(l_Layers, false);
	return std::find_if(l_Layers.begin(), l_Layers.end(),
		[p_Name](const VkLayerProperties& p_Layer) { return p_Name == p_Layer.layerName && VlkSystem::GetAPIVersion() >= p_Layer.specVersion; }) != l_Layers.end();
}

bool VlkInstance::IsExtensionAvailable(const std::string& p_Name, const std::string& p_LayerName)
{
	if (!p_LayerName.empty() && !IsLayerAvailable(p_LayerName)) return false;

	ExtensionProperties_t l_Extensions;
	GetAllExtensions(l_Extensions, p_LayerName);
	return std::find_if(l_Extensions.begin(), l_Extensions.end(),
		[p_Name](const VkExtensionProperties& p_Extension) { return p_Name == p_Extension.extensionName; }) != l_Extensions.end();
}

void VlkInstance::GetAllLayers(LayerProperties_t& p_Layers, bool p_bKeepUnsupported)
{
	uint32_t l_uiCount = 0;
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceLayerProperties(&l_uiCount, nullptr), "Failed to get instance layer count");

	p_Layers.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceLayerProperties(&l_uiCount, p_Layers.data()), "Failed to get instance layers");

	if (!p_bKeepUnsupported)
	{
		hwk::erase_if(p_Layers, [](const VkLayerProperties& p_Layer) { return p_Layer.specVersion > VlkSystem::GetAPIVersion(); });
	}
}

void VlkInstance::GetAllExtensions(ExtensionProperties_t& p_Extensions, const std::string& p_LayerName)
{
	uint32_t l_uiCount = 0;
	const char* l_LayerName = !p_LayerName.empty() ? p_LayerName.c_str() : nullptr;

	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceExtensionProperties(l_LayerName, &l_uiCount, nullptr), "Failed to get instance extension count. Layer=" << p_LayerName);

	p_Extensions.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceExtensionProperties(l_LayerName, &l_uiCount, p_Extensions.data()), "Failed to get instance extensions. Layer=" << p_LayerName);
}

void VlkInstance::GetLayersToCreate(std::vector<const char*>& p_Layers)
{
	std::copy(n_EnabledLayers.begin(), n_EnabledLayers.end(), std::back_inserter(p_Layers));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugLayers.begin(), n_EnabledDebugLayers.end(), std::back_inserter(p_Layers));
#endif

	for (const auto& l_Layer : p_Layers)
	{
		THROW_IF_NOT(IsLayerAvailable(l_Layer), "Instance layer not available. Name=" << l_Layer);
	}
}

void VlkInstance::GetExtensionsToCreate(std::vector<const char*>& p_Extensions)
{
	std::copy(n_EnabledExtensions.begin(), n_EnabledExtensions.end(), std::back_inserter(p_Extensions));
#ifdef HAWK_DEBUG
	std::copy(n_EnabledDebugExtensions.begin(), n_EnabledDebugExtensions.end(), std::back_inserter(p_Extensions));
#endif

	for (const auto& l_Extension : p_Extensions)
	{
		THROW_IF_NOT(IsExtensionAvailable(l_Extension), "Global instance extension not available. Name=" << l_Extension);
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

VkBool32 VKAPI_CALL VlkInstance::OnDebugReport(VkDebugReportFlagsEXT p_Flags, VkDebugReportObjectTypeEXT p_ObjectType, uint64_t p_uiObject, size_t p_Location, int32_t p_iMessageCode, const char* p_pLayerPrefix, const char* p_pMessage, void* /*p_pUserData*/)
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