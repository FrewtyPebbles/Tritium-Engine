#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>
#include <memory>

using std::vector;
using std::string;

// === NOTES ===

// Shaders will be all compiled when the game is built.

class GraphicsPipeline;

// We use a builder to make graphics pipeline creation more modular,
// but also to clean up unnessecary variables after creation.
class GraphicsPipelineBuilder {
public:
	// Creates the graphics pipeline builder
	GraphicsPipelineBuilder(vk::Device* vk_device);

	// BUILDER FUNCTIONS
	// These are used to build the pipeline
	
	GraphicsPipelineBuilder* add_vertex_input_binding(uint32_t binding_index, size_t stride, vk::VertexInputRate input_rate);

	GraphicsPipelineBuilder* add_vertex_input_attribute(uint32_t binding_index, uint32_t location, vk::Format format, uint32_t offset);

	GraphicsPipelineBuilder* add_stage(
		string shader_path,
		string entry_point,
		vk::ShaderStageFlagBits stage
	);

	GraphicsPipelineBuilder* add_dynamic_state(vk::DynamicState dynamic_state);

	GraphicsPipelineBuilder* set_primitive_topology(vk::PrimitiveTopology vk_primitive_topology);

	GraphicsPipelineBuilder* set_primitive_restart(bool vk_primitive_restart);

	// this builds out the final pipeline
	std::shared_ptr<GraphicsPipeline> build();

protected:

	static vector<char> read_binary(const string& filename);

private:

	vk::ShaderModule create_shader_module(const vector<char>& spir_v);

	

	vk::Device* vk_device;

	// Pipeline create info

	vector<vk::PipelineShaderStageCreateInfo> stages;

	vector<vk::DynamicState> dynamic_states;

	
	vector<vk::VertexInputBindingDescription> vertex_input_bindings;
	
	vector<vk::VertexInputAttributeDescription> vertex_input_attributes;

	vk::PrimitiveTopology vk_primitive_topology = vk::PrimitiveTopology::eTriangleList;

	bool vk_primitive_restart = false;
};


class GraphicsPipeline {
public:
	using Builder = GraphicsPipelineBuilder;

	GraphicsPipeline() = default;

};