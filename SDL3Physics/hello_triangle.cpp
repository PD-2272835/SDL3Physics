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
#include "Entity.hpp"

SDL_Window* window;
SDL_GPUDevice* device;
SDL_GPUBuffer* vertexBuffer;
SDL_GPUBuffer* vertexStorageBuffer;
SDL_GPUTransferBuffer* transferBuffer;
SDL_GPUGraphicsPipeline* graphicsPipeline;


SDL_GPUTexture* swapchainTexture; //RenderTarget equivalent (frame buffer)

Uint32 Width = 1280;
Uint32 Height = 720;

struct Vertex
{
	mfg::vec3 pos; //vec3 "position"
	mfg::vec4 color; //vec4 "colour"
};

const static Vertex vertices[]
{
	{mfg::vec3(0.0f, 0.5f, 0.0f), mfg::vec4(1.0f, 0.0f, 0.0f, 1.f)},     // top vertex
	{mfg::vec3(-0.5f, -0.5f, 0.0f), mfg::vec4(1.0f, 1.0f, 0.0f, 1.f)},   // bottom left vertex
	{mfg::vec3(0.5f, -0.5f, 0.0f), mfg::vec4(1.0f, 0.0f, 1.0f, 1.f)}	// bottom right
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

	SDL_GPUVertexAttribute vertexAttributes[2];
	vertexAttributes[0].buffer_slot = 0; //fetch data from the buffer at slot 0 (vertex buffer)
	vertexAttributes[0].location = 0; //layout 0
	vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; //vec3
	vertexAttributes[0].offset = 0; //attrib offset

	vertexAttributes[1].buffer_slot = 0; //fetch data from the buffer at slot 0 (vertex buffer)
	vertexAttributes[1].location = 1; //layout 1
	vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4; //vec4
	vertexAttributes[1].offset = sizeof(float) * 3; //attrib offset


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
	pipelineInfo.vertex_input_state.num_vertex_attributes = 2; //attribute count
	pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes; //we are using the layout defined in vertexAttributes

	pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST; //we are drawing triangles
	//rasterizer state
	//multisample state
	//depth stencil state
	pipelineInfo.target_info.num_color_targets = 1;
	pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;



	graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

	vertexShader.Delete();
	fragmentShader.Delete();


	//Fill unchanging buffers in the program start, if data changes frequently this should be done wherever it needs to be changed (eg. Iterate) 

	//Creating buffers
	//Vertex Buffer
	SDL_GPUBufferCreateInfo vertexBufferInfo{}; //create generic buffer
	vertexBufferInfo.size = sizeof(vertices); //size in bytes of each vertex - like layout
	vertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX; //define buffer usage
	vertexBuffer = SDL_CreateGPUBuffer(device, &vertexBufferInfo); //release buffer at the end of this method

	//Create Transfer Buffer - used to move data betweeen CPU & GPU
	SDL_GPUTransferBufferCreateInfo transferInfo{};
	transferInfo.size = sizeof(vertices);
	transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD; //we are adding data to the GPU
	transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);


	//fill data into the transfer buffer (setting up data for the gpu bus?)
	Vertex* data = (Vertex*)SDL_MapGPUTransferBuffer(device, transferBuffer, false); //map the transferbuffer to a ptr. what is cycling?
	SDL_memcpy(data, vertices, sizeof(vertices)); //fill data
	SDL_UnmapGPUTransferBuffer(device, transferBuffer); //unmap transfer buffer ptr when no longer updating it


	//Copy Pass
	//transfer data from the transfer buffer to the vertex buffer
	SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

	//find the data
	SDL_GPUTransferBufferLocation location{};
	location.transfer_buffer = transferBuffer;
	location.offset = 0; //start copying from beginning of transfer buffer

	//where to upload the data
	SDL_GPUBufferRegion region{};
	region.buffer = vertexBuffer; //we are writing to the vertex buffer
	region.size = sizeof(vertices); //how much data we are writing in bytes
	region.offset = 0; //start writing from start of the vertex buffer

	SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
	SDL_EndGPUCopyPass(copyPass); //must end pass before submission

	//must release buffers after finishing using them
	SDL_ReleaseGPUTransferBuffer(device, transferBuffer);


	
	//SSBO proof of concept
	SDL_GPUBufferCreateInfo VertexShaderStorageBufferInfo = {};
	VertexShaderStorageBufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
	VertexShaderStorageBufferInfo.size = sizeof(mfg::mat4);
	vertexStorageBuffer = SDL_CreateGPUBuffer(device, &VertexShaderStorageBufferInfo);

	transferInfo.size = VertexShaderStorageBufferInfo.size;
	transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

	mfg::mat4* matrixData = (mfg::mat4*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);
	mfg::mat4 dataArray[] = { mfg::Perspective(mfg::ToRadians(90.f), float(Width / Height), 0.1f, 100.f)  };

	SDL_memcpy(matrixData, dataArray, sizeof(dataArray));
	
	SDL_UnmapGPUTransferBuffer(device, transferBuffer);
	copyPass = SDL_BeginGPUCopyPass(commandBuffer);

	location.transfer_buffer = transferBuffer;
	region.buffer = vertexStorageBuffer;
	region.size = sizeof(mfg::mat4);
	SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
	SDL_EndGPUCopyPass(copyPass);

	SDL_ReleaseGPUTransferBuffer(device, transferBuffer);





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

	//set up the Color Target (RenderTargetSpec)
	SDL_GPUColorTargetInfo colorTargetInfo{}; //RenderTargetSpec equivalent
	colorTargetInfo.clear_color = { 240 / 255.f, 240 / 255.f, 240 / 255.f, 255 / 255.f }; //convert 0-255 colour values to a value from 0-1
	colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
	colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
	colorTargetInfo.texture = swapchainTexture;

	uniformData.time = SDL_GetTicksNS() / 1e9f; //fill uniform
	uniformData.Model = mfg::Rotate(1.f*uniformData.time, mfg::vec3(0, 1, 0));
	uniformData.View = mfg::View(mfg::vec3(1.f, 0.f, 0.f), mfg::vec3(0.f, 1.f, 0.f), mfg::vec3(0.f, 0.f, 1.f), mfg::vec3(0.f, 0.f, -10.f));
	SDL_PushGPUVertexUniformData(commandBuffer, 0, &uniformData, sizeof(uniformData)); //submit uniform

	//Draw Stuff (within render pass)
	SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

	//bind pipeline to renderpass
	SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

	//bind vertex buffer
	SDL_GPUBufferBinding bufferBindings[1];
	bufferBindings[0].buffer = vertexBuffer;
	bufferBindings[0].offset = 0;

	SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);

	SDL_BindGPUVertexStorageBuffers(renderPass, 0, &vertexStorageBuffer, 1); // "slot" corresponds to "binding" in the shader

	//DRAW COMMAND!!
	SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

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
	SDL_ReleaseGPUBuffer(device, vertexBuffer);
	SDL_ReleaseGPUBuffer(device, vertexStorageBuffer);
	SDL_DestroyGPUDevice(device);
	SDL_DestroyWindow(window);
}