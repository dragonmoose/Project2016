#pragma once
#include "System.h"
#include "Instance.h"
#include <vector>
#include <memory>

namespace Hawk {
namespace Gfx {
namespace Vulkan {

class PhysicalDevice final
{
public:
	PhysicalDevice(std::shared_ptr<Instance> p_Instance);
	PhysicalDevice(std::shared_ptr<Instance> p_Instance, uint32 p_uiDeviceID);
	~PhysicalDevice();
	PhysicalDevice(const PhysicalDevice&) = delete;
	PhysicalDevice& operator=(const PhysicalDevice&) = delete;

	static bool IsLayerAvailable(VkPhysicalDevice p_Device, const std::string& p_Name);
	static bool IsExtensionAvailable(VkPhysicalDevice p_Device, const std::string& p_Name, const std::string& p_LayerName = std::string());
	VkPhysicalDevice GetHandle() const;

	using QueueFamilyProperties = std::vector<VkQueueFamilyProperties>;
	static void GetQueueFamilyProperties(const VkPhysicalDevice p_Device, QueueFamilyProperties& p_Properties);

	VkFormat GetBackBufferDepthStencilFormat() const;
	VkFormat GetBackBufferColorFormat() const;
	void GetFrameBufferLimits(uint32& p_uiWidth, uint32& p_uiHeight, uint32& p_uiLayers) const;
	bool TryGetMemoryTypeIndex(uint32 p_uiMemoryTypeBits, VkMemoryPropertyFlags p_Flags, uint32& p_uiIndex) const;

#ifdef HAWK_DEBUG
	void CmdPrintDevices();
	void CmdPrintQueueFamilies(uint32 p_uiDeviceIndex);
	void CmdPrintLayers(uint32 p_uiDeviceIndex, bool p_bKeepUnsupported);
	void CmdPrintExtensions(uint32 p_uiDeviceIndex, bool p_bKeepUnsupported);
	void CmdPrintDepthStencilFormats(uint32 p_uiDeviceIndex, bool p_bKeepUnsupported);
#endif

private:
	using Devices = std::vector<VkPhysicalDevice>;
	using LayerProperties = std::vector<VkLayerProperties>;
	using ExtensionProperties = std::vector<VkExtensionProperties>;

	static void GetAllLayers(VkPhysicalDevice p_Device, LayerProperties& p_Layers, bool p_bKeepUnsupported);
	static void GetAllExtensions(VkPhysicalDevice p_Device, ExtensionProperties& p_Extensions, const std::string& p_LayerName = std::string());

	void GetDevices(Devices& p_Devices) const;
	VkPhysicalDevice GetDeviceByIndex(uint32 p_uiIndex) const;
	VkPhysicalDevice GetDeviceByID(uint32 p_uiDeviceID) const;

	static void GetDeviceProperties(const VkPhysicalDevice p_Device, VkPhysicalDeviceProperties& p_Properties);
	static bool IsDepthStencilFormatSupported(VkPhysicalDevice p_Device, VkFormat p_Format);
	static bool IsColorFormatSupported(VkPhysicalDevice p_Device, VkFormat p_Format);

	void Init();
	void SelectBackBufferDepthStencilFormat();
	void SelectBackBufferColorFormat();
	
	std::shared_ptr<Instance> m_Instance;
	VkPhysicalDevice m_Handle;

	VkFormat m_BackBufferDepthStencilFormat;
	VkFormat m_BackBufferColorFormat;
	VkPhysicalDeviceMemoryProperties m_MemoryProperties;
};

}
}
}