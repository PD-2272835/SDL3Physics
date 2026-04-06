#include <SDL3/SDL_gpu.h>
#include "Shader.hpp"

//Create shader from already loaded file
Shader::Shader(SDL_GPUDevice* device, void* code, size_t codeSize, Uint32 format, SDL_GPUShaderStage stage, Uint32 samplers, Uint32 storageBuffers, Uint32 storageTextures, Uint32 uniformBuffers)
{
	Device = device;
	Info = { 
		codeSize, 
		(Uint8*)code, 
		"main", 
		format, 
		stage, 
		samplers, 
		storageBuffers, 
		storageTextures, 
		uniformBuffers
	};
	ID = SDL_CreateGPUShader(Device, &Info);
}

//Create Shader from file path with SDL_LoadFile
Shader::Shader(SDL_GPUDevice* device, const char* filePath, Uint32 format, SDL_GPUShaderStage stage, Uint32 samplers, Uint32 storageBuffers, Uint32 storageTextures, Uint32 uniformBuffers)
{
	Device = device;

	Info = {};
	void* code = SDL_LoadFile(filePath, &Info.code_size);
	Info.code = (Uint8*)code;
	Info.entrypoint = "main";
	Info.format = format;
	Info.stage = stage;
	Info.num_samplers = samplers;
	Info.num_storage_buffers = storageBuffers;
	Info.num_storage_textures = storageTextures;
	Info.num_uniform_buffers = uniformBuffers;

	ID = SDL_CreateGPUShader(Device, &Info);
	SDL_free(code);
}


//this object should not be used after Delete is called
void Shader::Delete()
{
	SDL_ReleaseGPUShader(Device, ID);
	Device = nullptr;
}
