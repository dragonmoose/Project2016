#pragma once
#include "System.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan
{
	class ShaderModule;
	class Device;

	class ShaderManager final
	{
	public:
		ShaderManager(std::shared_ptr<Device> p_Device);
		~ShaderManager();
		ShaderManager(const ShaderManager&) = delete;
		ShaderManager& operator=(const ShaderManager&) = delete;

		VkPipelineShaderStageCreateInfo GetShader(const std::string& p_Filename, const std::string& p_EntryPoint, VkShaderStageFlagBits p_Stage);

	private:
		std::string GetPath(const std::string& p_Filename);
		VkPipelineShaderStageCreateInfo GetCreateInfo(const ShaderModule* p_Module, const std::string& p_EntryPoint, VkShaderStageFlagBits p_Stage);

		using Modules = std::unordered_map<std::string, std::unique_ptr<ShaderModule>>;
		Modules m_Modules;
		std::shared_ptr<Device> m_Device;
	};

}
}
}