#include "pch.h"
#include "GraphicsPipeline.h"
#include "Device.h"
#include "VertexDeclarations.h"
#include "RenderPass.h"
#include "ShaderModule.h"
#include "ShaderManager.h"
#include "Base/WindowManager.h"

namespace Hawk {
namespace Gfx {
namespace Vulkan {

GraphicsPipeline::GraphicsPipeline(std::shared_ptr<Device> p_Device, const RenderPass* p_RenderPass, ShaderManager* p_ShaderManager)
: m_Device(p_Device)
{
	//VkPipelineCache l_Cache = VK_NULL_HANDLE; // TODO: Use pipeline caching

	VkPipelineVertexInputStateCreateInfo l_VertexInputStateInfo = {};
	l_VertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	// TODO: Declare this elsewhere, how manage binding, inputRate?
	VkVertexInputBindingDescription l_VertexBindingDesc = {};
	l_VertexBindingDesc.binding = 0;
	l_VertexBindingDesc.stride = VertexDeclarations::PosColor::Size();
	l_VertexBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	
	l_VertexInputStateInfo.vertexBindingDescriptionCount = 1;
	l_VertexInputStateInfo.pVertexBindingDescriptions = &l_VertexBindingDesc;

	std::vector<VkVertexInputAttributeDescription> l_VertexAttrDescs;
	l_VertexAttrDescs.resize(2);

	l_VertexAttrDescs[0].location = 0;
	l_VertexAttrDescs[0].binding = 0;
	l_VertexAttrDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	l_VertexAttrDescs[0].offset = 0;

	l_VertexAttrDescs[1].location = 1;
	l_VertexAttrDescs[1].binding = 0;
	l_VertexAttrDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	l_VertexAttrDescs[1].offset = sizeof(float) * 3;

	l_VertexInputStateInfo.vertexAttributeDescriptionCount = 2;
	l_VertexInputStateInfo.pVertexAttributeDescriptions = l_VertexAttrDescs.data();

	VkPipelineInputAssemblyStateCreateInfo l_AssemblyInfo = {};
	l_AssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	l_AssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	l_AssemblyInfo.primitiveRestartEnable = VK_FALSE;
	
	VkPipelineViewportStateCreateInfo l_ViewportInfo = {};
	l_ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	l_ViewportInfo.viewportCount = 1;
	l_ViewportInfo.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo l_RasterizationInfo = {};
	l_RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	l_RasterizationInfo.depthClampEnable = VK_FALSE;		// TODO: Understand this and following properties
	l_RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	l_RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	l_RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	l_RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	l_RasterizationInfo.depthBiasEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo l_DepthStencilInfo = {};
	l_DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	l_DepthStencilInfo.depthTestEnable = VK_TRUE;
	l_DepthStencilInfo.depthWriteEnable = VK_TRUE;
	l_DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	l_DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	l_DepthStencilInfo.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState l_BlendAttachment = {};
	l_BlendAttachment.blendEnable = VK_FALSE;

	// Note: This mask is applied regardless of whether blending is enabled or not
	l_BlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo l_BlendInfo = {};
	l_BlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	l_BlendInfo.logicOpEnable = VK_FALSE;
	l_BlendInfo.attachmentCount = 1;
	l_BlendInfo.pAttachments = &l_BlendAttachment;

	std::vector<VkDynamicState> l_DynamicStates;
	l_DynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	l_DynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

	VkPipelineDynamicStateCreateInfo l_DynamicInfo = {};
	l_DynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	l_DynamicInfo.dynamicStateCount = (uint32)l_DynamicStates.size();
	l_DynamicInfo.pDynamicStates = l_DynamicStates.data();

	std::vector<VkPipelineShaderStageCreateInfo> l_ShaderInfo;
	l_ShaderInfo.resize(2);
	
	//l_ShaderInfo[0] = p_ShaderManager->GetShader("test.vert", "main", VK_SHADER_STAGE_VERTEX_BIT);
	l_ShaderInfo[1] = p_ShaderManager->GetShader("triangle.vert.spv", "main", VK_SHADER_STAGE_VERTEX_BIT);

	VkGraphicsPipelineCreateInfo l_Info = {};
	l_Info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	l_Info.stageCount = 2;
	l_Info.pStages = l_ShaderInfo.data();
	l_Info.pVertexInputState = &l_VertexInputStateInfo;
	l_Info.pInputAssemblyState = &l_AssemblyInfo;
	l_Info.pTessellationState = nullptr;
	l_Info.pViewportState = &l_ViewportInfo;
	l_Info.pRasterizationState = &l_RasterizationInfo;
	l_Info.pMultisampleState = nullptr;
	l_Info.pDepthStencilState = &l_DepthStencilInfo;
	l_Info.pColorBlendState = &l_BlendInfo;
	l_Info.pDynamicState = &l_DynamicInfo;
	//l_Info.layout =
	l_Info.renderPass = p_RenderPass->GetHandle();
	l_Info.subpass = 0;
	l_Info.basePipelineHandle = VK_NULL_HANDLE;

//	VK_THROW_IF_NOT_SUCCESS(vkCreateGraphicsPipelines(m_Device->GetHandle(), l_Cache, 1, &l_Info, nullptr, &m_Handle), "Failed to create graphics pipeline");
}

GraphicsPipeline::~GraphicsPipeline()
{
	LOG("GraphicsPipeline destroyed", "vulkan", Debug);
//	vkDestroyPipeline(m_Device->GetHandle(), m_Handle, nullptr);
}

VkPipeline GraphicsPipeline::GetHandle() const
{
	return m_Handle;
}

}
}
}