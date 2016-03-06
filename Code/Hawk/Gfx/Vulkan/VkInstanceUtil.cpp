#include "pch.h"
#include "VkInstanceUtil.h"
#include "VkSystem.h"
#include "VkUtil.h"
#include <unordered_map>
#include <algorithm>
#include <iomanip>

namespace Hawk {
namespace Gfx {
namespace VkInstanceUtil
{
	using LayerProperties_t = std::vector<VkLayerProperties>;
	using ExtensionProperties_t = std::vector<VkExtensionProperties>;

	void GetLayers(LayerProperties_t& p_Layers);
	void GetExtensions(ExtensionProperties_t& p_Extensions, const std::string& p_LayerName = std::string());
}

bool VkInstanceUtil::IsLayerAvailable(const std::string& p_Name)
{
	LayerProperties_t l_Layers;
	GetLayers(l_Layers);
	return std::find_if(l_Layers.begin(), l_Layers.end(),
		[p_Name](const VkLayerProperties& p_Layer) { return p_Name == p_Layer.layerName; }) != l_Layers.end();
}

bool VkInstanceUtil::IsExtensionAvailable(const std::string& p_Name, const std::string& p_LayerName)
{
	ExtensionProperties_t l_Extensions;
	GetExtensions(l_Extensions, p_LayerName);
	return std::find_if(l_Extensions.begin(), l_Extensions.end(),
		[p_Name](const VkExtensionProperties& p_Extension) { return p_Name == p_Extension.extensionName; }) != l_Extensions.end();
}


void VkInstanceUtil::CmdPrintAvailableLayers()
{
	LayerProperties_t l_Layers;
	GetLayers(l_Layers);

	CONSOLE_WRITE_SCOPE();
	std::cout << "\n";
	for (const auto& l_Layer : l_Layers)
	{
		std::cout << l_Layer.layerName << " (" << l_Layer.description << ")" <<
			" SpecVersion: " << VkUtil::SpecVersionToString(l_Layer.specVersion) <<
			" ImplementationVersion: " << l_Layer.implementationVersion << "\n";
	}
	std::cout << "\n";
}

void VkInstanceUtil::CmdPrintAvailableExtensions()
{
	CONSOLE_WRITE_SCOPE();

	std::cout << "\n-Global extensions-----------------------\n";
	ExtensionProperties_t l_Extensions;
	GetExtensions(l_Extensions);
	for (const auto& l_Extension : l_Extensions)
	{
		std::cout << l_Extension.extensionName << " Version: " << l_Extension.specVersion << "\n";
	}
	std::cout << "\n-Layer extensions------------------------\n";

	LayerProperties_t l_Layers;
	GetLayers(l_Layers);

	for (const auto& l_Layer : l_Layers)
	{
		l_Extensions.clear();
		GetExtensions(l_Extensions, l_Layer.layerName);
		if (!l_Extensions.empty())
		{
			std::cout << l_Layer.layerName << ":\n";
			for (const auto& l_Extension : l_Extensions)
			{
				std::cout << "\t" << l_Extension.extensionName << " Version: " << l_Extension.specVersion << "\n";
			}
			std::cout << "\n";
		}
	}
	std::cout << "\n";
}

void VkInstanceUtil::GetLayers(LayerProperties_t& p_Layers)
{
	uint32_t l_uiCount = 0;
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceLayerProperties(&l_uiCount, nullptr), "Failed to get instance layer count");

	p_Layers.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceLayerProperties(&l_uiCount, p_Layers.data()), "Failed to get instance layers");
}

void VkInstanceUtil::GetExtensions(ExtensionProperties_t& p_Extensions, const std::string& p_LayerName)
{
	uint32_t l_uiCount = 0;
	const char* l_LayerName = !p_LayerName.empty() ? p_LayerName.c_str() : nullptr;

	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceExtensionProperties(l_LayerName, &l_uiCount, nullptr), "Failed to get instance extension count. Layer=" << p_LayerName);

	p_Extensions.resize(l_uiCount);
	VK_THROW_IF_NOT_SUCCESS(vkEnumerateInstanceExtensionProperties(l_LayerName, &l_uiCount, p_Extensions.data()), "Failed to get instance extensions. Layer=" << p_LayerName);
}

}
}