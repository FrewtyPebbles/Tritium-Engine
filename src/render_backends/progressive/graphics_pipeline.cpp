#include "Engine/render_backends/progressive/graphics_pipeline.h"
#include <fstream>

///////////////////////////////
// GRAPHICS PIPELINE BUILDER //
///////////////////////////////

GraphicsPipelineBuilder::GraphicsPipelineBuilder(const string& name, Logger* logger, VirtualDevice* device)
	: name(name), logger(logger), device(device) {

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
		this->vk_primitive_restart
	);

	// create viewport state

	vk::PipelineViewportStateCreateInfo viewPortStateCreateInfo = vk::PipelineViewportStateCreateInfo(
		{},
		this->vk_viewport_count,
		nullptr,
		this->vk_scissor_count,
		nullptr
	);

	// create rasterizer

	vk::PipelineRasterizationStateCreateInfo rasterizationCreateInfo = vk::PipelineRasterizationStateCreateInfo(
		{},
		this->vk_depth_clamp_enable,
		this->vk_rasterizer_discard_enable,
		this->vk_polygon_mode,
		this->vk_cull_mode,
		this->vk_front_face,
		this->vk_depth_bias_enable,
		this->vk_depth_bias_constant_factor,
		this->vk_depth_bias_clamp,
		this->vk_depth_bias_slope_factor,
		this->vk_line_width
	);

	// create multisampling state

	vk::PipelineMultisampleStateCreateInfo multisamplingCreateInfo = vk::PipelineMultisampleStateCreateInfo(
		{},
		this->vk_multisampling_rasterization_samples,
		this->vk_sample_shading_enable,
		this->vk_min_sample_shading,
		this->vk_p_sample_mask,
		this->vk_alpha_to_coverage_enable,
		this->vk_alpha_to_one_enable
	);

	// create color blend state

	vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo;
		colorBlendCreateInfo.logicOpEnable = this->vk_color_blend_logical_op_enable;
		colorBlendCreateInfo.logicOp = this->vk_color_blend_logical_op;
		colorBlendCreateInfo.attachmentCount = this->vk_color_blend_attachments.size();
		colorBlendCreateInfo.pAttachments = this->vk_color_blend_attachments.data();
		colorBlendCreateInfo.blendConstants[0] = this->vk_color_blend_constants[0];
		colorBlendCreateInfo.blendConstants[1] = this->vk_color_blend_constants[1];
		colorBlendCreateInfo.blendConstants[2] = this->vk_color_blend_constants[2];
		colorBlendCreateInfo.blendConstants[3] = this->vk_color_blend_constants[3];

	// create pipeline layout

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo(
		{},
		this->vk_descriptor_set_layouts,
		this->vk_push_constant_ranges
	);

	vk::PipelineLayout pipelineLayout = this->device->get_vulkan_device()->createPipelineLayout(pipelineLayoutCreateInfo);

	return std::make_shared<GraphicsPipeline>(this->logger, this->device, pipelineLayout);
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
	this->device->get_vulkan_device()->destroyShaderModule(shaderModule);

	return this;
}

vk::ShaderModule GraphicsPipelineBuilder::create_shader_module(const vector<char>& spir_v) {
	vk::ShaderModuleCreateInfo createInfo = vk::ShaderModuleCreateInfo(
		{},
		spir_v.size(),
		reinterpret_cast<const uint32_t*>(spir_v.data())
	);

	return this->device->get_vulkan_device()->createShaderModule(createInfo);
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
	if (this->device->vk_device_features.multiViewport == vk::True) {
		this->vk_viewport_count = vk_viewport_count;
	} else {
		this->vk_viewport_count = 1;
	}
		
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_scissor_count(uint32_t vk_scissor_count) {
	if (this->device->vk_device_features.multiViewport == vk::True) {
		this->vk_scissor_count = vk_scissor_count;
	}
	else {
		this->vk_scissor_count = 1;
	}

	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_depth_clamp_enable(bool vk_depth_clamp_enable) {
	if (this->device->vk_device_features.depthClamp) {
		this->vk_depth_clamp_enable = vk_depth_clamp_enable;
	} else {
		this->logger->log(
			"The enabled GPU \""
			+ std::string(static_cast<const char*>(this->device->vk_device_properties.deviceName))
			+ "\" does not support depth clamping. Hardware depth clamping will be disabled.",
			"rendering",
			Log::Domain::RENDERING,
			Log::Severity::INFO
		);
		this->vk_depth_clamp_enable = false;
	}
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_rasterizer_discard_enable(bool vk_rasterizer_discard_enable) {
	this->vk_rasterizer_discard_enable = vk_rasterizer_discard_enable;
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_polygon_mode(vk::PolygonMode vk_polygon_mode) {
	if (
		this->device->vk_device_features.fillModeNonSolid
		&& (vk::PolygonMode::ePoint == vk_polygon_mode || vk::PolygonMode::eLine == vk_polygon_mode)
		|| vk_polygon_mode == vk::PolygonMode::eFill
	) {
		this->vk_polygon_mode = vk_polygon_mode;
	} else {
		string polygonModeString = "Unknown";
		switch (vk_polygon_mode) {
			case vk::PolygonMode::ePoint:
				polygonModeString = "Point";
				break;
			case vk::PolygonMode::eLine:
				polygonModeString = "Line";
				break;
		}
		this->logger->log(
			"The enabled GPU \""
			+ std::string(static_cast<const char*>(this->device->vk_device_properties.deviceName))
			+ "\" does not support the specified polygon mode \""
			+ polygonModeString.c_str()
			+ "\".  Falling back to hardware \"Fill\" polygon mode.",
			"rendering",
			Log::Domain::RENDERING,
			Log::Severity::WARNING
		);
		this->vk_polygon_mode = vk::PolygonMode::eFill;
	}
	
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_line_width(const float& vk_request_line_width) {
	if (this->device->vk_device_features.wideLines) {
		this->vk_line_width = std::clamp(vk_request_line_width,
			this->device->vk_device_properties.limits.lineWidthRange[0],
			this->device->vk_device_properties.limits.lineWidthRange[1]
		);

		if (vk_request_line_width != this->vk_line_width) {
			this->logger->log(
				"The enabled GPU \"" + std::string(static_cast<const char*>(this->device->vk_device_properties.deviceName)) +
				"\" does not support " + std::to_string(vk_request_line_width) +
				"f width lines. Clamping to supported range ["
				+ std::to_string(this->device->vk_device_properties.limits.lineWidthRange[0])
				+ ", "
				+ std::to_string(this->device->vk_device_properties.limits.lineWidthRange[1])
				+ "].",
				"rendering",
				Log::Domain::RENDERING,
				Log::Severity::WARNING
			);
		}
	} else {
		this->logger->log(
			"The enabled GPU \""
			+ string(static_cast<const char*>(this->device->vk_device_properties.deviceName))
			+ "\" does not support wide lines. Line width will fall back to 1.0f.",
			"rendering",
			Log::Domain::RENDERING,
			Log::Severity::WARNING
		);

		this->vk_line_width = 1.0f;
	}

	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_cull_mode(vk::CullModeFlags vk_cull_mode) {
	this->vk_cull_mode = vk_cull_mode;
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_front_face(vk::FrontFace vk_front_face) {
	this->vk_front_face = vk_front_face;
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_depth_bias(
	bool vk_depth_bias_enable,
	float vk_depth_bias_constant_factor,
	float vk_depth_bias_clamp,
	float vk_depth_bias_slope_factor
) {
	this->vk_depth_bias_enable = vk_depth_bias_enable;
	this->vk_depth_bias_constant_factor = vk_depth_bias_constant_factor;
	this->vk_depth_bias_clamp = vk_depth_bias_clamp;
	this->vk_depth_bias_slope_factor = vk_depth_bias_slope_factor;
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_multisampling(
	bool vk_sample_shading_enable,
	vk::SampleCountFlagBits vk_rasterization_samples,
	float vk_min_sample_shading,
	vk::SampleMask* vk_p_sample_mask,
	bool vk_alpha_to_coverage_enable,
	bool vk_alpha_to_one_enable
) {
	// TODO : Make api for the end game developer to query for support information when using the progressive
	// render backend.

	// set settings and fallback with warning if not supported.

	// SAMPLE RATE SHADING

	if (this->device->vk_device_features.sampleRateShading || !vk_sample_shading_enable) {
		this->vk_sample_shading_enable = vk_sample_shading_enable;
	} else {
		this->logger->log(
			"The enabled GPU \""
			+ string(static_cast<const char*>(this->device->vk_device_properties.deviceName))
			+ "\" does not support sample rate shading. Sample rate shading will be disabled.",
			"rendering",
			Log::Domain::RENDERING,
			Log::Severity::WARNING
		);

		this->vk_sample_shading_enable = false;
	}

	// RASTERIZATION SAMPLES

	// get supported sample rate:

	vk::SampleCountFlags supportedSampleCounts =
		this->device->vk_device_properties.limits.framebufferColorSampleCounts;

	if (this->vk_depth_test_enabled)
		supportedSampleCounts &=
		this->device->vk_device_properties.limits.framebufferDepthSampleCounts;

	if (this->vk_stencil_test_enabled)
		supportedSampleCounts &=
		this->device->vk_device_properties.limits.framebufferStencilSampleCounts;

	if (supportedSampleCounts & vk_rasterization_samples) {
		this->vk_multisampling_rasterization_samples = vk_rasterization_samples;
	} else {
		vk::SampleCountFlagBits fallbackSampleCount = vk::SampleCountFlagBits::e1;
		if (supportedSampleCounts & vk::SampleCountFlagBits::e64 && vk_rasterization_samples > vk::SampleCountFlagBits::e64)
			fallbackSampleCount = vk::SampleCountFlagBits::e64;
		else if (supportedSampleCounts & vk::SampleCountFlagBits::e32 && vk_rasterization_samples > vk::SampleCountFlagBits::e32)
			fallbackSampleCount = vk::SampleCountFlagBits::e32;
		else if (supportedSampleCounts & vk::SampleCountFlagBits::e16 && vk_rasterization_samples > vk::SampleCountFlagBits::e16)
			fallbackSampleCount = vk::SampleCountFlagBits::e16;
		else if (supportedSampleCounts & vk::SampleCountFlagBits::e8 && vk_rasterization_samples > vk::SampleCountFlagBits::e8)
			fallbackSampleCount = vk::SampleCountFlagBits::e8;
		else if (supportedSampleCounts & vk::SampleCountFlagBits::e4 && vk_rasterization_samples > vk::SampleCountFlagBits::e4)
			fallbackSampleCount = vk::SampleCountFlagBits::e4;
		else if (supportedSampleCounts & vk::SampleCountFlagBits::e2 && vk_rasterization_samples > vk::SampleCountFlagBits::e2)
			fallbackSampleCount = vk::SampleCountFlagBits::e2;

		this->logger->log(
			"The enabled GPU \""
			+ string(static_cast<const char*>(this->device->vk_device_properties.deviceName))
			+ "\" does not support "
			+ std::to_string(static_cast<uint32_t>(vk_rasterization_samples))
			+ "xMSAA. Falling back to "
			+ std::to_string(static_cast<uint32_t>(fallbackSampleCount))
			+ "xMSAA.",
			"rendering",
			Log::Domain::RENDERING,
			Log::Severity::WARNING
		);

		this->vk_multisampling_rasterization_samples = fallbackSampleCount;
	}

	// MIN SAMPLE SHADING
	if (this->vk_sample_shading_enable) {
		this->vk_min_sample_shading = std::clamp(vk_min_sample_shading, 0.0f, 1.0f);
	}
	else {
		// if we failed to enable sample shading rate and the required feature isnt available then
		// log a warning.
		if (!this->device->vk_device_features.sampleRateShading && vk_sample_shading_enable) {
			this->logger->log(
				"The enabled GPU \""
				+ string(static_cast<const char*>(this->device->vk_device_properties.deviceName))
				+ "\" does not support sample rate shading. Minimum sample shading will fall back to 0.0f.",
				"rendering",
				Log::Domain::RENDERING,
				Log::Severity::WARNING
			);
		}
		

		this->vk_min_sample_shading = 0.0f;
	}

	this->vk_p_sample_mask = vk_p_sample_mask;

	this->vk_alpha_to_coverage_enable = vk_alpha_to_coverage_enable;

	this->vk_alpha_to_one_enable = vk_alpha_to_one_enable;

	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::add_color_blend_attachment(
	vk::ColorComponentFlags vk_color_write_mask,
	bool vk_blend_enable,
	vk::BlendFactor vk_src_color_blend_factor,
	vk::BlendFactor vk_dst_color_blend_factor,
	vk::BlendOp vk_color_blend_op,
	vk::BlendFactor vk_src_alpha_blend_factor,
	vk::BlendFactor vk_dst_alpha_blend_factor,
	vk::BlendOp vk_alpha_blend_op
) {
	this->vk_color_blend_attachments.push_back(vk::PipelineColorBlendAttachmentState(
		vk_blend_enable,
		vk_src_color_blend_factor,
		vk_dst_color_blend_factor,
		vk_color_blend_op,
		vk_src_alpha_blend_factor,
		vk_dst_alpha_blend_factor,
		vk_alpha_blend_op,
		vk_color_write_mask
	));
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_color_blend_logical_op(
	bool enabled,
	vk::LogicOp op
) {
	this->vk_color_blend_logical_op_enable = enabled;
	this->vk_color_blend_logical_op = op;
	return this;
}

GraphicsPipelineBuilder* GraphicsPipelineBuilder::set_color_blend_constants(
	float r,
	float g,
	float b,
	float a
) {
	this->vk_color_blend_constants[0] = r;
	this->vk_color_blend_constants[1] = g;
	this->vk_color_blend_constants[2] = b;
	this->vk_color_blend_constants[3] = a;
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


///////////////////////
// GRAPHICS PIPELINE //
///////////////////////

GraphicsPipeline::GraphicsPipeline(Logger* logger, VirtualDevice* device, vk::PipelineLayout vk_pipeline_layout)
: logger(logger), device(device), vk_pipeline_layout(vk_pipeline_layout) {

}

void GraphicsPipeline::clean_up() {
	this->device->get_vulkan_device()->destroyPipelineLayout(this->vk_pipeline_layout);
}