#pragma once
#include "VkSystem.h"

namespace Hawk {
namespace Gfx {

class VkInstanceWrapper final
{
public:
	VkInstanceWrapper();
	~VkInstanceWrapper();
	VkInstanceWrapper(const VkInstance&) = delete;
	VkInstanceWrapper& operator=(const VkInstance&) = delete;

private:
	void GetLayers(std::vector<const char*>& p_Layers) const;
	void GetExtensions(std::vector<const char*>& p_Extensions) const;

#ifdef HAWK_DEBUG
	void RetrieveDebugCallbacks();
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