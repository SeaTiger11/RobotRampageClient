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

[[nodiscard]] vk::raii::ShaderModule createShaderModule(RobotRampageClient& app, const std::vector<char>& code) {
	vk::ShaderModuleCreateInfo createInfo;
	createInfo.setCodeSize(code.size() * sizeof(char));
	createInfo.setPCode(reinterpret_cast<const uint32_t*>(code.data()));

	vk::raii::ShaderModule shaderModule{ app.device, createInfo };

	return shaderModule;
}

void createGraphicsPipeline(RobotRampageClient& app) {
	vk::raii::ShaderModule vertShaderModule = createShaderModule(app, readFile("shaders/vert.spv"));
	vk::raii::ShaderModule fragShaderModule = createShaderModule(app, readFile("shaders/frag.spv"));

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
	vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);
	vertShaderStageInfo.setModule(vertShaderModule);
	vertShaderStageInfo.setPName("main");

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
	fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);
	fragShaderStageInfo.setModule(fragShaderModule);
	fragShaderStageInfo.setPName("main");

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.setVertexBindingDescriptionCount(1);
	vertexInputInfo.setVertexBindingDescriptions(bindingDescription);
	vertexInputInfo.setVertexAttributeDescriptionCount(attributeDescriptions.size());
	vertexInputInfo.setVertexAttributeDescriptions(attributeDescriptions);

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

	vk::PipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.setDepthClampEnable(vk::False);
	rasterizer.setRasterizerDiscardEnable(vk::False);
	rasterizer.setPolygonMode(vk::PolygonMode::eFill);
	rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
	rasterizer.setFrontFace(vk::FrontFace::eCounterClockwise);
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
	pipelineLayoutInfo.setSetLayoutCount(1);
	pipelineLayoutInfo.setPSetLayouts(&*app.descriptorSetLayout);
	pipelineLayoutInfo.setPushConstantRangeCount(0);

	app.pipelineLayout = vk::raii::PipelineLayout(app.device, pipelineLayoutInfo);

	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo;
	pipelineRenderingCreateInfo.setColorAttachmentCount(1);
	pipelineRenderingCreateInfo.setPColorAttachmentFormats(&app.swapChainSurfaceFormat.format);
	pipelineRenderingCreateInfo.setDepthAttachmentFormat(findDepthFormat(app));

	vk::PipelineDepthStencilStateCreateInfo depthStencil;
	depthStencil.setDepthTestEnable(vk::True);
	depthStencil.setDepthWriteEnable(vk::True);
	depthStencil.setDepthCompareOp(vk::CompareOp::eLess);
	depthStencil.setDepthBoundsTestEnable(vk::False);
	depthStencil.setStencilTestEnable(vk::False);

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.setPNext(&pipelineRenderingCreateInfo);
	pipelineInfo.setStageCount(2);
	pipelineInfo.setPStages(shaderStages);
	pipelineInfo.setPVertexInputState(&vertexInputInfo);
	pipelineInfo.setPInputAssemblyState(&inputAssembly);
	pipelineInfo.setPViewportState(&viewportState);
	pipelineInfo.setPRasterizationState(&rasterizer);
	pipelineInfo.setPMultisampleState(&multisampling);
	pipelineInfo.setPDepthStencilState(&depthStencil);
	pipelineInfo.setPColorBlendState(&colorBlending);
	pipelineInfo.setPDynamicState(&dynamicState);
	pipelineInfo.setLayout(app.pipelineLayout);
	pipelineInfo.setRenderPass(nullptr);

	app.graphicsPipeline = vk::raii::Pipeline(app.device, nullptr, pipelineInfo);
}