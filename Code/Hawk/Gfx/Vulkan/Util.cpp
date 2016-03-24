#include "pch.h"
#include "Util.h"
#include "System.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

namespace Util
{
	std::vector<VkFormat> n_DepthFormats = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};
}

std::string Util::SpecVersionToString(uint32 p_uiVersion)
{
	std::ostringstream l_Stream;
	l_Stream << VK_VERSION_MAJOR(p_uiVersion) << "."
		<< VK_VERSION_MINOR(p_uiVersion) << "."
		<< VK_VERSION_PATCH(p_uiVersion);
	return l_Stream.str();
}

std::string Util::PipelineCacheUUIDToString(const uint8* p_UUID)
{
	std::ostringstream l_Stream;
	for (int i = 0; i < VK_UUID_SIZE; i++)
	{
		l_Stream << static_cast<uint32>(p_UUID[i]);
		if (i < VK_UUID_SIZE - 1)
		{
			l_Stream << ":";
		}
	}
	return l_Stream.str();
}

std::string Util::DeviceTypeToString(VkPhysicalDeviceType p_Type)
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

std::string Util::QueueFlagsToString(VkQueueFlags p_Flags)
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

std::string Util::TimestampValidBitsToString(uint32 p_Bits)
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

VkQueueFlags Util::QueueTypeToFlag(QueueType p_Type)
{
	switch (p_Type)
	{
	case QueueType::Graphics:
	case QueueType::GraphicsPresentation:
		return VK_QUEUE_GRAPHICS_BIT;
	case QueueType::Compute:
		return VK_QUEUE_COMPUTE_BIT;
	case QueueType::Transfer:
		return VK_QUEUE_TRANSFER_BIT;
	case QueueType::SparseBinding:
		return VK_QUEUE_SPARSE_BINDING_BIT;
	default:
		THROW("Invalid type: " << (int)p_Type);
	}
}

std::string Util::DepthFormatToString(VkFormat p_Format)
{
	switch (p_Format)
	{
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return "D32_SFLOAT_S8_UINT";
		case VK_FORMAT_D32_SFLOAT:
			return "D32_SFLOAT";
		case VK_FORMAT_D24_UNORM_S8_UINT:
			return "D24_UNORM_S8_UINT";
		case VK_FORMAT_D16_UNORM_S8_UINT:
			return "D16_UNORM_S8_UINT";
		case VK_FORMAT_D16_UNORM:
			return "D16_UNORM";
		default:
			THROW("Invalid depth format type: " << (int)p_Format);
	}
}

const std::vector<VkFormat>& Util::GetDepthFormats()
{
	return n_DepthFormats;
}

bool Util::IsDepthFormat(VkFormat p_Format)
{
	return std::find(n_DepthFormats.cbegin(), n_DepthFormats.cend(), p_Format) != n_DepthFormats.end();
}

}
}
}