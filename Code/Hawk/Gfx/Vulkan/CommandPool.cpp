#include "pch.h"
#include "CommandPool.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

CommandPool::CommandPool()
: m_Handle(VK_NULL_HANDLE)
{

}

CommandPool::~CommandPool()
{
	LOG("CommandPool destroyed", "vulkan", Debug);
}

VkCommandPool CommandPool::GetHandle() const
{
	return m_Handle;
}

}
}
}