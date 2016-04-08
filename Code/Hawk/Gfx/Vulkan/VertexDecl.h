#pragma once
#include "System.h"
#include <vector>

namespace Hawk {
namespace Gfx {
namespace Vulkan {
namespace VertexDecl
{
	struct PosColor
	{
		float m_Pos[3];
		float m_Color[3];
		static const VkDeviceSize Size() { return sizeof(PosColor); }
	};
	using PosColorVertices = std::vector<PosColor>;
}
}
}
}