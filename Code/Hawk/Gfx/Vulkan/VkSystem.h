#pragma once
#include "VkUtil.h"
#include "System/Exception.h"
#include <vulkan/vulkan.h>

#define VK_THROW(p, msg) THROW(msg << " [VkResult: " << Hawk::Gfx::VkUtil::ResultToString(p) << "]")
#define VK_THROW_IF_ERR(p, msg)	if ((p < 0)) VK_THROW(p, msg)
#define VK_THROW_IF_NOT_SUCCESS(p, msg) if ((p != 0)) VK_THROW(p, msg)