#pragma once
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class CommandBuffer final
{
public:
	CommandBuffer(VkCommandBuffer p_Handle);
	~CommandBuffer();
	CommandBuffer(const CommandBuffer&) = delete;
	CommandBuffer& operator=(const CommandBuffer&) = delete;

	std::shared_ptr<CommandBuffer> CreateSecondary();
	void Reset(bool p_bReleaseResources) const; // May not be called if not created from CommandPool with p_bIndividualBufferReset=true

	VkCommandBuffer GetHandle() const;

private:
	VkCommandBuffer m_Handle;
};

}
}
}