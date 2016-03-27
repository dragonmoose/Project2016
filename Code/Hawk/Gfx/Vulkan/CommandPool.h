#pragma once
#include "System.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;

class CommandPool final
{
public:
	CommandPool(std::shared_ptr<Device> p_Device, uint32 p_uiQueueFamilyIndex, bool p_bAllowIndividualBufferReset = false, bool p_bShortLifetimeObjects = false);
	~CommandPool();
	CommandPool(const CommandPool&) = delete;
	CommandPool& operator=(const CommandPool&) = delete;
	bool AllowsIndividualBufferReset() const;

	VkCommandPool GetHandle() const;
	void Reset(bool p_bReleaseResources) const;

private:
	VkCommandPool m_Handle;
	std::shared_ptr<Device> m_Device;
	bool m_bAllowIndividualBufferReset;
};

}
}
}