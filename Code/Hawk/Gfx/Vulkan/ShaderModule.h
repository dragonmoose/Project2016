#pragma once
#include "System.h"
#include <memory>
#include <string>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class Device;

class ShaderModule final
{
public:
	ShaderModule(std::shared_ptr<Device> p_Device, const std::string& p_Path);
	~ShaderModule();
	ShaderModule(const ShaderModule&) = delete;
	ShaderModule& operator=(const ShaderModule&) = delete;

	VkShaderModule GetHandle() const;

private:
	std::shared_ptr<Device> m_Device;
	VkShaderModule m_Handle;
};

}
}
}