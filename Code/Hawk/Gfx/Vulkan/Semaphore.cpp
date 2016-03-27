#include "pch.h"
#include "Semaphore.h"
#include "Device.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

Semaphore::Semaphore(std::shared_ptr<Device> p_Device)
: m_Device(p_Device)
{
	VkSemaphoreCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VK_THROW_IF_NOT_SUCCESS(vkCreateSemaphore(m_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create semaphore");
}

Semaphore::~Semaphore()
{
	vkDestroySemaphore(m_Device->GetHandle(), m_Handle, nullptr);
}

VkSemaphore Semaphore::GetHandle() const
{
	return m_Handle;
}

}
}
}