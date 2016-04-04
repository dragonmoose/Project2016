#pragma once

namespace Hawk {
namespace Gfx {
namespace Vulkan {
namespace VertexDeclarations
{
	struct PosColor
	{
		float m_Pos[3];
		float m_Color[4];
		static const uint32 Size() { return sizeof(PosColor); }
	};
	using PosColorVertices = std::vector<PosColor>;
}
}
}
}