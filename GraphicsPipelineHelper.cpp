#include "GraphicsPipelineHelper.h";

static std::vector<char> readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file");
	}

	std::vector<char> buffer(file.tellg());

	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
	file.close();

	return buffer;
}

[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, vk::raii::Device& device) {
	vk::ShaderModuleCreateInfo createInfo;
	createInfo.setCodeSize(code.size() * sizeof(char));
	createInfo.setPCode(reinterpret_cast<const uint32_t*>(code.data()));

	vk::raii::ShaderModule shaderModule{ device, createInfo };

	return shaderModule;
}

void createGraphicsPipeline(vk::raii::PipelineLayout& pipelineLayout, vk::raii::Device& device, SwapChainData& swapChainData) {
	vk::raii::ShaderModule vertShaderModule = createShaderModule(readFile("shaders/vert.spv"), device);
	vk::raii::ShaderModule fragShaderModule = createShaderModule(readFile("shaders/frag.spv"), device);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
	vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);
	vertShaderStageInfo.setModule(vertShaderModule);
	vertShaderStageInfo.setPName("vertMain");

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
	fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);
	fragShaderStageInfo.setModule(fragShaderModule);
	fragShaderStageInfo.setPName("fragMain");

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

	vk::PipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.setDepthClampEnable(vk::False);
	rasterizer.setRasterizerDiscardEnable(vk::False);
	rasterizer.setPolygonMode(vk::PolygonMode::eFill);
	rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
	rasterizer.setFrontFace(vk::FrontFace::eClockwise);
	rasterizer.setDepthBiasClamp(vk::False);
	rasterizer.setDepthBiasSlopeFactor(1.0f);
	rasterizer.setLineWidth(1.0f);

	vk::PipelineMultisampleStateCreateInfo multisampling;
	multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);
	multisampling.setSampleShadingEnable(vk::False);

	// Notably this is configured to have no colour blending and will need to be modified to enable opacity
	vk::PipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.setBlendEnable(vk::False);
	colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	vk::PipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.setLogicOpEnable(vk::False);
	colorBlending.setLogicOp(vk::LogicOp::eCopy);
	colorBlending.setAttachmentCount(1);
	colorBlending.setPAttachments(&colorBlendAttachment);

	std::vector dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamicState;
	dynamicState.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
	dynamicState.setPDynamicStates(dynamicStates.data());

	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.setViewportCount(1);
	viewportState.setScissorCount(1);

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setSetLayoutCount(0);
	pipelineLayoutInfo.setPushConstantRangeCount(0);

	pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);

	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo;
	pipelineRenderingCreateInfo.setColorAttachmentCount(1);
	pipelineRenderingCreateInfo.setPColorAttachmentFormats(&swapChainData.swapChainSurfaceFormat.format);

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.setPNext(&pipelineRenderingCreateInfo);
	pipelineInfo.setStageCount(2);
	pipelineInfo.setPStages(shaderStages);
	pipelineInfo.setPVertexInputState(&vertexInputInfo);
	pipelineInfo.setPInputAssemblyState(&inputAssembly);
	pipelineInfo.setPViewportState(&viewportState);
	pipelineInfo.setPRasterizationState(&rasterizer);
	pipelineInfo.setPMultisampleState(&multisampling);
	pipelineInfo.setPColorBlendState(&colorBlending);
	pipelineInfo.setPDynamicState(&dynamicState);
	pipelineInfo.setLayout(pipelineLayout);
	pipelineInfo.setRenderPass(nullptr);
}