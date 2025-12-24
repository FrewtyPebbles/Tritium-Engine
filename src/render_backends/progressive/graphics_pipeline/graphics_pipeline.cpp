#include "Engine/render_backends/progressive/graphics_pipeline/graphics_pipeline.h"
#include <fstream>

GraphicsPipeline::GraphicsPipeline(vk::Device* vk_device)
: vk_device(vk_device) {

}

GraphicsPipeline* GraphicsPipeline::add_stage(
	string shader_path,
	string entry_point,
	vk::ShaderStageFlagBits stage
) {
	auto shaderCode = this->read_binary(shader_path);

	vk::ShaderModule shaderModule = this->create_shader_module(shaderCode);

	vk::PipelineShaderStageCreateInfo shaderStageCreateInfo = vk::PipelineShaderStageCreateInfo(
		{},
		stage,
		shaderModule,
		entry_point.c_str()
	);

	stages.push_back(shaderStageCreateInfo);

	// clean up
	this->vk_device->destroyShaderModule(shaderModule);

	return this;
}

vk::ShaderModule GraphicsPipeline::create_shader_module(const vector<char>& spir_v) {
	vk::ShaderModuleCreateInfo createInfo = vk::ShaderModuleCreateInfo(
		{},
		spir_v.size(),
		reinterpret_cast<const uint32_t*>(spir_v.data())
	);

	return vk_device->createShaderModule(createInfo);
}


vector<char> GraphicsPipeline::read_binary(const string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open shader file: " + filename);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	vector<char> buffer(fileSize);
	
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();
	
	return buffer;
}