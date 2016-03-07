#pragma once
#include "VlkSystem.h"

namespace Hawk {
namespace Gfx {

class VlkInstance final
{
public:
	VlkInstance();
	~VlkInstance();
	VlkInstance(const VlkInstance&) = delete;
	VlkInstance& operator=(const VlkInstance&) = delete;

	VkInstance GetHandle() const;

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