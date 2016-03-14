#include "pch.h"
#include "VlkUtil.h"
#include "VlkSystem.h"

namespace Hawk {
namespace Gfx {

std::string VlkUtil::SpecVersionToString(uint32_t p_uiVersion)
{
	std::ostringstream l_Stream;
	l_Stream << VK_VERSION_MAJOR(p_uiVersion) << "."
		<< VK_VERSION_MINOR(p_uiVersion) << "."
		<< VK_VERSION_PATCH(p_uiVersion);
	return l_Stream.str();
}

std::string VlkUtil::PipelineCacheUUIDToString(const uint8_t* p_UUID)
{
	std::ostringstream l_Stream;
	for (int i = 0; i < VK_UUID_SIZE; i++)
	{
		l_Stream << static_cast<uint32_t>(p_UUID[i]);
		if (i < VK_UUID_SIZE - 1)
		{
			l_Stream << ":";
		}
	}
	return l_Stream.str();
}

std::string VlkUtil::DeviceTypeToString(VkPhysicalDeviceType p_Type)
{
	switch (p_Type)
	{
	case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		return "Other";
	case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
		return "IntegratedGPU";
	case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
		return "DiscreteGPU";
	case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
		return "VirtualGPU";
	case VK_PHYSICAL_DEVICE_TYPE_CPU:
		return "CPU";
	default:
		THROW("DeviceType out of bounds: " << p_Type);
	}
}

std::string VlkUtil::QueueFlagsToString(VkQueueFlags p_Flags)
{
	std::ostringstream l_Stream;
	if (p_Flags & VK_QUEUE_GRAPHICS_BIT)
	{
		l_Stream << "Graphics;";
	}
	if (p_Flags & VK_QUEUE_COMPUTE_BIT)
	{
		l_Stream << "Compute;";
	}
	if (p_Flags & VK_QUEUE_TRANSFER_BIT)
	{
		l_Stream << "Transfer;";
	}
	if (p_Flags & VK_QUEUE_SPARSE_BINDING_BIT)
	{
		l_Stream << "SparseBinding";
	}
	return l_Stream.str();
}

std::string VlkUtil::TimestampValidBitsToString(uint32_t p_Bits)
{
	if (p_Bits == 0)
	{
		return "No timestamp support";
	}

	std::ostringstream l_Stream;
	l_Stream << p_Bits;
	if (p_Bits < 32 || p_Bits > 64)
	{
		l_Stream << " [Invalid bit count!]";
	}
	return l_Stream.str();
}

VkQueueFlags VlkUtil::QueueTypeToFlag(VlkQueueType p_Type)
{
	switch (p_Type)
	{
	case VlkQueueType::Graphics:
		return VK_QUEUE_GRAPHICS_BIT;
	case VlkQueueType::Compute:
		return VK_QUEUE_COMPUTE_BIT;
	case VlkQueueType::Transfer:
		return VK_QUEUE_TRANSFER_BIT;
	case VlkQueueType::SparseBinding:
		return VK_QUEUE_SPARSE_BINDING_BIT;
	default:
		THROW("Invalid type: " << (int)p_Type);
	}
}

}
}