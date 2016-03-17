#pragma once

namespace Hawk {
namespace Gfx {
namespace Vulkan {

enum class QueueType
{
	Graphics,
	GraphicsPresentation,
	Compute,
	Transfer,
	SparseBinding
};

}
}
}