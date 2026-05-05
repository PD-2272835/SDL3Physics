//Following SDL_gpu hello triangle tutorial resource from Hamdy Elzanqali 
//https://hamdy-elzanqali.medium.com/let-there-be-triangles-sdl-gpu-edition-bd82cf2ef615

#define SDL_MAIN_USE_CALLBACKS 1
#define SDL_HINT_RENDER_VULKAN_DEBUG 1

//use the expanded SDL_GPUShaderFormat flags, this macro just allows formats to be changed more easily
#define PLATFORM_TARGET_TYPE (1u << 1)

#include <SDL3\SDL_main.h>
#include <SDL3\SDL.h>
#include <mfg.hpp>
#include "Shader.hpp"
#include "Buffer.hpp"
#include "Entity.hpp"
#include "AssetLoaders.hpp"

SDL_Window* window;
SDL_GPUDevice* device;
Buffer vertexBuffer;
Buffer vertexStorageBuffer;
Buffer indexBuffer;
std::shared_ptr<Mesh> model;
SDL_GPUTexture* depthTexture;
SDL_GPUTransferBuffer* transferBuffer;
SDL_GPUGraphicsPipeline* graphicsPipeline;


SDL_GPUTexture* swapchainTexture; //RenderTarget equivalent (frame buffer)

Uint32 Width = 1280;
Uint32 Height = 720;

const static Vertex quad[]
{
	{mfg::vec3(-0.5f, 0.5f, 0.0f), mfg::vec3(1.0f, 0.0f, 0.0f), mfg::vec2(1.0f, 1.0f)},	// top left
	{mfg::vec3(-0.5f, -0.5f, 0.0f), mfg::vec3(1.0f, 1.0f, 0.0f), mfg::vec2(1.f, 0.f)},	// bottom left
	{mfg::vec3(0.5f, 0.5f, 0.0f), mfg::vec3(1.0f, 0.0f, 1.0f), mfg::vec2(1.f, 0.5f)},	// top right
	{mfg::vec3(0.5f, -0.5f, 0.0f), mfg::vec3(1.0f, 0.0f, 1.0f), mfg::vec2(1.f, 0.5f)}	// bottom righta
};

const static Uint16 quadIndices[]
{
	0, 1, 2,
	2, 1, 3
};


struct UniformBuffer
{
	mfg::mat4 View;
	mfg::mat4 Model;
	float time;
};

static UniformBuffer uniformData{};

//AppInit is called at the very start of program execution
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
	std::cout << sizeof(Uint32) << " uint32\n";
	std::cout << sizeof(Uint64) << " uint64\n";
	std::cout << sizeof(char) << " char\n";
	std::cout << sizeof(mfg::vec3) << " vec3\n";
	std::cout << sizeof(float) << " float\n";
	std::cout << sizeof(int) << " int\n";
	std::cout << sizeof(std::size_t) << " size_t\n";
	std::cout << sizeof(bool) << "bool\n";
	std::cout << sizeof(Entity) << "entity\n";
	std::cout << alignof(Entity) << "\n";
	std::cout << sizeof(EntityHandle) << "handle\n";
	std::cout << sizeof(void*) << "voidptr\n";

	window = SDL_CreateWindow("Test Window!", Width, Height, SDL_WINDOW_FULLSCREEN & SDL_WINDOW_BORDERLESS); //SDL_WINDOW_FULLSCREEN & SDL_WINDOW_BORDERLESS

	//using Vulkan/NDA platform for shaders - Vulkan allows use of SDL_Shadercross (portability between platforms)
	device = SDL_CreateGPUDevice(PLATFORM_TARGET_TYPE, true, NULL);
	SDL_ClaimWindowForGPUDevice(device, window);

	Shader vertexShader(device, "shaderSource/default_vert.spv", PLATFORM_TARGET_TYPE, SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, 0, 1);
	Shader fragmentShader(device, "shaderSource/default_frag.spv", PLATFORM_TARGET_TYPE, SDL_GPU_SHADERSTAGE_FRAGMENT);

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
	colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);


	//Creating a Graphics Pipeline
	SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.vertex_shader = vertexShader.ID;//bind shaders
	pipelineInfo.fragment_shader = fragmentShader.ID;

	pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
	pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions;
	pipelineInfo.vertex_input_state.num_vertex_attributes = 3; //attribute count
	pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes; //we are using the layout defined in vertexAttributes

	pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST; //we are drawing triangles
	//rasterizer state
	//multisample state
	//depth stencil state

	const SDL_GPUTextureFormat depthFormat = SDL_GPU_TEXTUREFORMAT_D24_UNORM;

	SDL_GPUTextureCreateInfo depthTexInfo = {};
	depthTexInfo.format = depthFormat;
	depthTexInfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
	depthTexInfo.width = Width;
	depthTexInfo.height = Height;
	depthTexInfo.layer_count_or_depth = 1;
	depthTexInfo.num_levels = 1;

	depthTexture = SDL_CreateGPUTexture(device, &depthTexInfo);

	pipelineInfo.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;
	pipelineInfo.depth_stencil_state.enable_depth_test = true;
	pipelineInfo.depth_stencil_state.enable_depth_write = true;


	pipelineInfo.target_info.num_color_targets = 1;
	pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

	pipelineInfo.target_info.has_depth_stencil_target = true;
	pipelineInfo.target_info.depth_stencil_format = depthFormat;


	graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

	vertexShader.Delete();
	fragmentShader.Delete();

	model = LoadObj("C:/Users/eater/Desktop/KenneyCarsOBJ/suv-luxury.obj");

	//Fill unchanging buffers in the program start, if data changes frequently this should be done wherever it needs to be changed (eg. Iterate) 

	vertexBuffer = Buffer(device, SDL_GPU_BUFFERUSAGE_VERTEX, model->Vertices.size() * sizeof(Vertex));
	vertexStorageBuffer = Buffer(device, SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ, sizeof(mfg::mat4));
	
	SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
	vertexBuffer.UploadData(commandBuffer, (void*)model->Vertices.data(), model->Vertices.size() * sizeof(Vertex), 0);

	mfg::mat4 dataArray[] = { mfg::Perspective(mfg::ToRadians(90.f), float(Width / Height), 0.01f, 100.f) };
	vertexStorageBuffer.UploadData(commandBuffer, (void*)dataArray, sizeof(dataArray), 0);

	indexBuffer = Buffer(device, SDL_GPU_BUFFERUSAGE_INDEX, model->Indices.size() * sizeof(uint32_t));
	indexBuffer.UploadData(commandBuffer, (void*)model->Indices.data(), model->Indices.size() * sizeof(uint32_t), 0);


	SDL_SubmitGPUCommandBuffer(commandBuffer); //Do the GPU activity defined in the constructed commandBuffer



	return SDL_APP_CONTINUE;
}





//AppIterate is called every frame/background update - roughly equivalent to Unity's `Update()` callback
SDL_AppResult SDL_AppIterate(void *appstate)
{
	SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device); //Tell the GPU we want to do something with it
	
	SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapchainTexture, &Width, &Height);

	//end frame early if no swapchain texture
	if (swapchainTexture == NULL)
	{
		SDL_SubmitGPUCommandBuffer(commandBuffer);
		std::cout << "ended frame early\n";
		return SDL_APP_CONTINUE;
	}

	SDL_GPUDepthStencilTargetInfo depthInfo = {};
	depthInfo.texture = depthTexture;
	depthInfo.load_op = SDL_GPU_LOADOP_CLEAR;
	depthInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
	depthInfo.clear_depth = 1.0f;


	//set up the Color Target (RenderTargetSpec)
	SDL_GPUColorTargetInfo colorTargetInfo{}; //RenderTargetSpec equivalent
	colorTargetInfo.clear_color = { 240 / 255.f, 240 / 255.f, 240 / 255.f, 255 / 255.f }; //convert 0-255 colour values to a value from 0-1
	colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
	colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
	colorTargetInfo.texture = swapchainTexture;

	uniformData.time = SDL_GetTicksNS() / 1e9f; //fill uniform
	uniformData.Model = mfg::Rotate(1.f*uniformData.time, mfg::vec3(0, 1, 0));
	uniformData.View = mfg::View(mfg::vec3(1.f, 0.f, 0.f), mfg::vec3(0.f, 1.f, 0.f), mfg::vec3(0.f, 0.f, 1.f), mfg::vec3(0.f, -1.f, -10.f));
	SDL_PushGPUVertexUniformData(commandBuffer, 0, &uniformData, sizeof(uniformData)); //submit uniform

	//Draw Stuff (within render pass)
	SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, &depthInfo);

	//bind pipeline to renderpass
	SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

	//bind vertex buffer - TODO: create methods for this in buffer class
	SDL_GPUBufferBinding vertexBindings[1];
	vertexBindings[0].buffer = vertexBuffer.ID;
	vertexBindings[0].offset = 0;

	SDL_GPUBufferBinding indexBindings[1];
	indexBindings[0].buffer = indexBuffer.ID;
	indexBindings[0].offset = 0;
	

	SDL_BindGPUVertexBuffers(renderPass, 0, vertexBindings, 1);
	SDL_BindGPUIndexBuffer(renderPass, indexBindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);

	SDL_BindGPUVertexStorageBuffers(renderPass, 0, &vertexStorageBuffer.ID, 1); // "slot" corresponds to "binding" in the shader

	//DRAW COMMAND!!
	//SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
	SDL_DrawGPUIndexedPrimitives(renderPass, model->Indices.size(), 1, 0, 0, 0);

	SDL_EndGPURenderPass(renderPass);
	//Anything else we want to do goes below here

	SDL_SubmitGPUCommandBuffer(commandBuffer); //Do the GPU activity defined in the constructed commandBuffer
	return SDL_APP_CONTINUE;
}




//AppEvent is used to handle input/events (eg. keypress, window close)
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED ||
		event->key.key == SDLK_ESCAPE)
	{
		return SDL_APP_SUCCESS;
	}

	return SDL_APP_CONTINUE;
}

//AppQuit is called upon termination of program execution (usually used to free resources)
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	std::cout << std::endl;
	SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);
	vertexBuffer.Delete();
	vertexStorageBuffer.Delete();
	indexBuffer.Delete();
	SDL_ReleaseGPUTexture(device, depthTexture);
	SDL_DestroyGPUDevice(device);
	SDL_DestroyWindow(window);
}