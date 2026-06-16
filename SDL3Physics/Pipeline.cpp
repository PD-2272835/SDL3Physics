#include "Pipeline.hpp"

GFXPipeline& GFXPipeline::operator=(const GFXPipeline& other)
{
	if (this != &other)
	{
		Info = other.Info;
		Handle = other.Handle;
	}
	return *this;
}


GFXPipeline::GFXPipeline(GFXPipeline&& other) noexcept
{
	Handle = other.Handle;
	Info = other.Info;
	other.Handle = nullptr;
}

GFXPipeline::GFXPipeline(
	Shader* vShader,
	Shader* fShader)
{
	Application* App = Application::GetInstance();

	Info = {};
	//describing the vertex buffers
	SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
	vertexBufferDescriptions[0].slot = 0;
	vertexBufferDescriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX; //enumerating buffer gets each vertex
	vertexBufferDescriptions[0].instance_step_rate = 0;
	vertexBufferDescriptions[0].pitch = sizeof(Vertex); //bytes to jump each "cycle" (stride)

	SDL_GPUVertexAttribute vertexAttributes[3];
	vertexAttributes[0].buffer_slot = 0; //fetch data from the buffer at slot 0 (vertex buffer)
	vertexAttributes[0].location = 0; //layout 0
	vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; //vec3
	vertexAttributes[0].offset = 0; //attrib offset

	vertexAttributes[1].buffer_slot = 0; //fetch data from the buffer at slot 0 (vertex buffer)
	vertexAttributes[1].location = 1; //layout 1
	vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
	vertexAttributes[1].offset = sizeof(float) * 3; //attrib offset

	vertexAttributes[2].buffer_slot = 0;
	vertexAttributes[2].location = 2;
	vertexAttributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
	vertexAttributes[2].offset = sizeof(float) * 3;


	//enable color blending
	SDL_GPUColorTargetDescription colorTargetDescriptions[1];
	colorTargetDescriptions[0] = {};
	colorTargetDescriptions[0].blend_state.enable_blend = true;
	colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
	colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
	colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
	colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
	colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(App->Device, App->Window);


	//Creating a Graphics Pipeline
	Info.vertex_shader = vShader->Handle;//bind shaders
	Info.fragment_shader = fShader->Handle;

	Info.vertex_input_state.num_vertex_buffers = 1;
	Info.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions;
	Info.vertex_input_state.num_vertex_attributes = 3; //attribute count
	Info.vertex_input_state.vertex_attributes = vertexAttributes; //we are using the layout defined in vertexAttributes

	Info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST; //we are drawing triangles
	//rasterizer state
	//multisample state (AA)

	//depth testing

	const SDL_GPUTextureFormat depthFormat = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
	/*SDL_GPUTextureSupportsFormat(device,
	SDL_GPU_TEXTUREFORMAT_D32_FLOAT, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)
	? SDL_GPU_TEXTUREFORMAT_D32_FLOAT : SDL_GPU_TEXTUREFORMAT_D16_UNORM; //use float if supported
*/
	SDL_GPUTextureCreateInfo depthTexInfo = {};
	depthTexInfo.format = depthFormat;
	depthTexInfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
	depthTexInfo.width = App->WindowWidth;
	depthTexInfo.height = App->WindowHeight;
	depthTexInfo.layer_count_or_depth = 1;
	depthTexInfo.num_levels = 1;
	SDL_GPUTexture* DepthTexture = SDL_CreateGPUTexture(App->Device, &depthTexInfo);
	
	App->DepthTexture = DepthTexture;

	Info.depth_stencil_state.enable_depth_test = true;
	Info.depth_stencil_state.enable_depth_write = true;
	Info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;
	Info.target_info.depth_stencil_format = depthFormat;
	Info.target_info.has_depth_stencil_target = true;

	//output buffers
	Info.target_info.num_color_targets = 1;
	Info.target_info.color_target_descriptions = colorTargetDescriptions;

	Handle = SDL_CreateGPUGraphicsPipeline(App->Device, &Info);
}








GFXPipeline::GFXPipeline(
	const Shader* vShader,
	const Shader* fShader, 
	const SDL_GPURasterizerState* rState, 
	const SDL_GPUPrimitiveType primitiveType, 
	const SDL_GPUVertexInputState* vInputState, 
	const SDL_GPUMultisampleState* msState, 
	const SDL_GPUDepthStencilState* dsState, 
	const SDL_GPUGraphicsPipelineTargetInfo* tInfo, 
	const SDL_PropertiesID extensionProps)
{
	Application* app = Application::GetInstance();

	Info = {}; //ensure default initialization
	Info.vertex_shader = vShader->Handle;
	Info.fragment_shader = fShader->Handle;

	if (app->Device == nullptr) std::cerr << "Invalid GPU Device!" << std::endl;
	if (app->Window == nullptr) std::cerr << "Invalid Window!" << std::endl;

	//elevate the scope of these to be outside the statement
	SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
	SDL_GPUVertexAttribute vertexAttributes[3];
	if (vInputState != nullptr) Info.vertex_input_state = *vInputState;
	else 
	{
		//DEFAULT VERTEX BUFFER
		vertexBufferDescriptions[0].slot = 0;
		vertexBufferDescriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX; //GPU stepping through buffer gets each vertex
		vertexBufferDescriptions[0].instance_step_rate = 0;
		vertexBufferDescriptions[0].pitch = sizeof(Vertex); //bytes to jump each "cycle" (stride)

		//default vertex layout, position (vec3), normal (vec3), UV (vec2)
		vertexAttributes[0].buffer_slot = 0; //fetch data from the buffer at slot 0 (vertex buffer)
		vertexAttributes[0].location = 0; //layout 0
		vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; //vec3
		vertexAttributes[0].offset = 0; //attrib offset

		vertexAttributes[1].buffer_slot = 0; //fetch data from the buffer at slot 0 (vertex buffer)
		vertexAttributes[1].location = 1; //layout 1
		vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
		vertexAttributes[1].offset = sizeof(float) * 3; //attrib offset

		vertexAttributes[2].buffer_slot = 0;
		vertexAttributes[2].location = 2;
		vertexAttributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
		vertexAttributes[2].offset = sizeof(float) * 3;

		Info.vertex_input_state.num_vertex_buffers = 1;
		Info.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions;
		Info.vertex_input_state.num_vertex_attributes = 3; //attribute count
		Info.vertex_input_state.vertex_attributes = vertexAttributes; //we are using the layout defined in vertexAttributes
	}

	Info.primitive_type = primitiveType;

	if(rState != nullptr) Info.rasterizer_state = *rState; //no SGF default
	
	if (msState != nullptr) Info.multisample_state = *msState; //no SGF default
	

	bool usesDefaultDepth = true;
	if (dsState != nullptr)
	{
		Info.depth_stencil_state = *dsState;
		usesDefaultDepth = false;
	}
	else //SGF Default - if providing a custom dsState, target_info must define as well
	{
		Info.depth_stencil_state.enable_depth_test = true;
		Info.depth_stencil_state.enable_depth_write = true;
		Info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;
	}
	
	SDL_GPUColorTargetDescription colorTargetDescriptions[1];
	SDL_GPUTextureCreateInfo depthTexInfo = {};
	SDL_GPUTextureFormat depthFormat = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
	if (tInfo != nullptr) Info.target_info = *tInfo;
	else
	{
		//enable color blendingnot 
		colorTargetDescriptions[0] = {};
		colorTargetDescriptions[0].blend_state.enable_blend = true;
		colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
		colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
		colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
		colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
		colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(app->Device, app->Window);
	
		//Depth Texture - only use if default stencil has been used
			
		//SDL_GPUTextureSupportsFormat(app->Device, SDL_GPU_TEXTUREFORMAT_D32_FLOAT, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)? SDL_GPU_TEXTUREFORMAT_D32_FLOAT : SDL_GPU_TEXTUREFORMAT_D16_UNORM; //use float if supported

		depthTexInfo.format = depthFormat;
		depthTexInfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
		depthTexInfo.width = app->WindowWidth;
		depthTexInfo.height = app->WindowHeight;
		depthTexInfo.layer_count_or_depth = 1;
		depthTexInfo.num_levels = 1;
		app->DepthTexture = SDL_CreateGPUTexture(app->Device, &depthTexInfo);

		Info.target_info.depth_stencil_format = depthFormat;
		Info.target_info.has_depth_stencil_target = true;
		
		//Frame Texture
		Info.target_info.num_color_targets = 1;
		Info.target_info.color_target_descriptions = colorTargetDescriptions;
	}
	
	Info.props = extensionProps;


	Handle = SDL_CreateGPUGraphicsPipeline(app->Device, &Info);
}