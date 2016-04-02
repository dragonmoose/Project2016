#include "pch.h"
#include "ShaderModule.h"
#include "Device.h"
#include "Util/FileUtil.h"
#include <fstream>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

ShaderModule::ShaderModule(std::shared_ptr<Device> p_Device, const std::string& p_Path)
: m_Device(p_Device)
, m_Handle(VK_NULL_HANDLE)
{
	try
	{
		THROW_IF_NOT(FileUtil::Exists(p_Path), "File does not exist on disk=" << p_Path);
		std::size_t l_FileSize = static_cast<std::size_t>(FileUtil::FileSize(p_Path));
		THROW_IF_NOT(l_FileSize > 0, "File is empty=" << p_Path);

		bool l_bIsSPIRV = FileUtil::HasExtension(p_Path, ".spv");
		THROW_IF(l_bIsSPIRV && l_FileSize % 4 != 0, "Filesize for SPIR-V file not multiple of 4. Size=" << l_FileSize);
		
		std::vector<char> l_Buffer;
		l_Buffer.resize(l_FileSize);

		int32 l_iOpenFlags = std::ios_base::in;
		l_iOpenFlags |= (l_bIsSPIRV ? std::ios_base::binary : 0);

		std::ifstream l_File(p_Path, l_iOpenFlags);
		THROW_IF_NOT(l_File, "Failed to open file=" << p_Path);

		l_File.read(l_Buffer.data(), l_FileSize);

		VkShaderModuleCreateInfo l_Info = {};
		l_Info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		l_Info.pCode = reinterpret_cast<uint32*>(l_Buffer.data());
		l_Info.codeSize = l_FileSize;

		VK_THROW_IF_NOT_SUCCESS(vkCreateShaderModule(p_Device->GetHandle(), &l_Info, nullptr, &m_Handle), "Failed to create shader module. Path=" << p_Path);
		LOG_IF(l_bIsSPIRV, "Loaded SPIR-V module: " << p_Path, "vulkan", Debug);
		LOG_IF(!l_bIsSPIRV, "Loaded glsl module: " << p_Path, "vulkan", Debug);

		LOG("shader-Contents=" << (char*)(l_Buffer.data()), "vulkan", Debug);
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION(e, "vulkan", Error);
	}
	catch (std::exception& e)
	{
		LOG_STD_EXCEPTION(e, "vulkan", Error);
	}
}

ShaderModule::~ShaderModule()
{
	if (m_Handle)
	{
		vkDestroyShaderModule(m_Device->GetHandle(), m_Handle, nullptr);
	}
}

VkShaderModule ShaderModule::GetHandle() const
{
	return m_Handle;
}

}
}
}