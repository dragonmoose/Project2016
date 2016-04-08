#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {
namespace UniformBufferDecl
{
	struct WVP
	{
		glm::mat4 m_World;
		glm::mat4 m_View;
		glm::mat4 m_Proj;
		static const VkDeviceSize Size() { return sizeof(WVP); }
	};
}
}
}
}