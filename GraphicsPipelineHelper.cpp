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

[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, vk::raii::Device& device) const {
	vk::ShaderModuleCreateInfo createInfo;
	createInfo.setCodeSize(code.size() * sizeof(char));
	createInfo.setPCode(reinterpret_cast<const uint32_t*>(code.data()));

	vk::raii::ShaderModule shaderModule{ device, createInfo };

	return shaderModule;
}

void createGraphicsPipeline(vk::raii::Device& device) {
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
}