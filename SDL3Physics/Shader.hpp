#ifndef _SHADER_HPP_
#define _SHADER_HPP_
#include <SDL3/SDL_gpu.h>

class Shader
{
public:
	SDL_GPUDevice* Device;
	SDL_GPUShaderCreateInfo Info;
	SDL_GPUShader* ID;

	Shader(SDL_GPUDevice* device, void* code, size_t codeSize, Uint32 format, SDL_GPUShaderStage stage, Uint32 samplers = 0, Uint32 storageBuffers = 0, Uint32 storageTextures = 0, Uint32 uniformBuffers = 0);
	Shader(SDL_GPUDevice* device, const char* filePath, Uint32 format, SDL_GPUShaderStage stage, Uint32 samplers = 0, Uint32 storageBuffers = 0, Uint32 storageTextures = 0, Uint32 uniformBuffers = 0);

	void Delete();
};
#endif