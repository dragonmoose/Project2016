#pragma once
#include "System.h"
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;

class Semaphore final
{
public:
	Semaphore(std::shared_ptr<Device> p_Device);
	~Semaphore();
	Semaphore(const Semaphore&) = delete;
	Semaphore& operator=(const Semaphore&) = delete;

	VkSemaphore GetHandle() const;

private:
	VkSemaphore m_Handle;
	std::shared_ptr<Device> m_Device;
};

}
}
}