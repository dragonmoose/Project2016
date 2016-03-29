#pragma once

namespace Hawk {
namespace Gfx {
namespace Vulkan {
namespace VertexDeclarations
{
	struct PosColor
	{
		float m_Pos[3];
		float m_Color[3];
		static const uint32 Stride() { return sizeof(PosColor); }
	};
}
}
}
}