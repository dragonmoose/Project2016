#include "pch.h"
#include "VkUtil.h"
#include <vulkan/vulkan.h>

namespace Hawk {
namespace Gfx {

std::string VkUtil::SpecVersionToString(uint32_t p_uiVersion)
{
	std::ostringstream l_Stream;
	l_Stream << VK_VERSION_MAJOR(p_uiVersion) << "."
		<< VK_VERSION_MINOR(p_uiVersion) << "."
		<< VK_VERSION_PATCH(p_uiVersion);
	return l_Stream.str();
}

}
}