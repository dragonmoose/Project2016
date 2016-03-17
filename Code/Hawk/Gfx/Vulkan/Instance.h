#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Instance final
{
public:
	Instance();
	~Instance();
	Instance(const Instance&) = delete;
	Instance& operator=(const Instance&) = delete;

	VkInstance GetHandle() const;

	static void CmdPrintLayers(bool p_bKeepUnsupported);
	static void CmdPrintExtensions(bool p_bKeepUnsupported);

private:
	using LayerProperties = std::vector<VkLayerProperties>;
	using ExtensionProperties = std::vector<VkExtensionProperties>;

	static bool IsLayerAvailable(const std::string& p_Name);
	static bool IsExtensionAvailable(const std::string& p_Name, const std::string& p_LayerName = std::string());
	static void GetAllLayers(LayerProperties& p_Layers, bool p_bKeepUnsupported);
	static void GetAllExtensions(ExtensionProperties& p_Extensions, const std::string& p_LayerName = std::string());

	static void GetLayersToCreate(std::vector<const char*>& p_Layers);
	static void GetExtensionsToCreate(std::vector<const char*>& p_Extensions);

#ifdef HAWK_DEBUG
	void RetrieveDebugCallbacks();
	static VkBool32 VKAPI_CALL OnDebugReport(VkDebugReportFlagsEXT p_Flags, VkDebugReportObjectTypeEXT p_ObjectType, uint64 p_uiObject, std::size_t p_Location, int32 p_iMessageCode, const char* p_pLayerPrefix, const char* p_pMessage, void* /*p_pUserData*/);
	void CreateDebugReportCallback();
	void DestroyDebugReportCallback();

	PFN_vkCreateDebugReportCallbackEXT m_CreateDebugReport;
	PFN_vkDestroyDebugReportCallbackEXT m_DestroyDebugReport;
	VkDebugReportCallbackEXT m_DebugReportHandle;
#endif
	VkInstance m_Handle;
};

}
}
}