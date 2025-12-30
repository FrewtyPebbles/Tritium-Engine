#include "Engine/render_backends/progressive/graphics_pipeline/graphics_pipeline.h"
#include <fstream>

GraphicsPipelineBuilder::GraphicsPipelineBuilder(vk::Device* vk_device)
: vk_device(vk_device) {

}

std::shared_ptr<GraphicsPipeline> GraphicsPipelineBuilder::build() {
	// create dynamic state info.
	vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo(
		{},
		dynamic_states
	);

	// create vertex input info
	
	vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo = vk::PipelineVertexInputStateCreateInfo(
		{},
		this->vertex_input_bindings,
		this->vertex_input_attributes
	);

	// create input assembly

	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = vk::PipelineInputAssemblyStateCreateInfo(
		{},
		this->vk_primitive_topology,
		vk_primitive_restart
	);

	// create viewport state

	vk::PipelineViewportStateCreateInfo viewPortStateCreateInfo = vk::PipelineViewportStateCreateInfo(
		{},
		this->vk_viewport_count,
		nullptr,
		this->vk_scissor_count,
		nullptr
	);

	return std::make_shared<GraphicsPipeline>();
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::add_stage(
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

vk::ShaderModule GraphicsPipelineBuilder::create_shader_module(const vector<char>& spir_v) {
	vk::ShaderModuleCreateInfo createInfo = vk::ShaderModuleCreateInfo(
		{},
		spir_v.size(),
		reinterpret_cast<const uint32_t*>(spir_v.data())
	);

	return vk_device->createShaderModule(createInfo);
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::add_dynamic_state(vk::DynamicState dynamic_state) {
	this->dynamic_states.push_back(dynamic_state);
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::add_vertex_input_binding(uint32_t binding_index, size_t stride, vk::VertexInputRate input_rate) {
	this->vertex_input_bindings.push_back(vk::VertexInputBindingDescription(
		binding_index,
		stride,
		input_rate
	));
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::add_vertex_input_attribute(uint32_t binding_index, uint32_t location, vk::Format format, uint32_t offset) {
	this->vertex_input_attributes.push_back(vk::VertexInputAttributeDescription(
		location,
		binding_index,
		format,
		offset
	));
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_primitive_topology(vk::PrimitiveTopology vk_primitive_topology) {
	this->vk_primitive_topology = vk_primitive_topology;
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_primitive_restart(bool vk_primitive_restart) {
	this->vk_primitive_restart = vk_primitive_restart;
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_viewport_count(uint32_t vk_viewport_count) {
	this->vk_viewport_count = vk_viewport_count;
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_scissor_count(uint32_t vk_scissor_count) {
	this->vk_scissor_count = vk_scissor_count;
	return this;
}


vector<char> GraphicsPipelineBuilder::read_binary(const string& filename) {
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