#pragma once
#include "VkSystem.h"

namespace Hawk {
namespace Gfx {

class VkInstance final
{
public:
	VkInstance();
	~VkInstance();
	VkInstance(const VkInstance&) = delete;
	VkInstance& operator=(const VkInstance&) = delete;

private:
	void GetLayers(std::vector<const char*>& p_Layers) const;
	void GetExtensions(std::vector<const char*>& p_Extensions) const;

	::VkInstance m_Handle;
};

}
}