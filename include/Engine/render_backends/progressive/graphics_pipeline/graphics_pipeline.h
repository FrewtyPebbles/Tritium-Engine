#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <string>

using std::vector;
using std::string;

// === NOTES ===

// Shaders will be all compiled when the game is built.

class GraphicsPipeline {
public:
	// Creates the graphics pipeline
	GraphicsPipeline(vk::Device* vk_device);
	void cleanup();

	// BUILDER FUNCTIONS
	// These are used to build the pipeline
	
	GraphicsPipeline* add_stage(
		string shader_path,
		string entry_point,
		vk::ShaderStageFlagBits stage
	);
private:

	vk::ShaderModule create_shader_module(const vector<char>& spir_v);

	static vector<char> read_binary(const string& filename);

	vector<vk::PipelineShaderStageCreateInfo> stages;

	vk::Device* vk_device;
};