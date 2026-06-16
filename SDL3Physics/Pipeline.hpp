#ifndef _PIPELINE_HPP_
#define _PIPELINE_HPP_

#include <SDL3/SDL_gpu.h>
#include "Application.hpp"
#include "Shader.hpp"
#include "AssetTypes.hpp"

class GFXPipeline
{
public:
	SDL_GPUGraphicsPipelineCreateInfo Info{};
	SDL_GPUGraphicsPipeline* Handle = nullptr;

	
	GFXPipeline() {};
	GFXPipeline(const GFXPipeline& other) : Info(other.Info), Handle(other.Handle) {};
	GFXPipeline(SDL_GPUGraphicsPipelineCreateInfo* info) : Info(*info), Handle(SDL_CreateGPUGraphicsPipeline(Application::GetInstance()->Device, &Info)) {};
	GFXPipeline(Shader* vShader, Shader* fShader);
	GFXPipeline(const Shader* vShader, const Shader* fShader, const SDL_GPURasterizerState* rState, const SDL_GPUPrimitiveType primitiveType = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		const SDL_GPUVertexInputState* vInputState = nullptr, const SDL_GPUMultisampleState* msState = nullptr, const SDL_GPUDepthStencilState* dsState = nullptr, const SDL_GPUGraphicsPipelineTargetInfo* tInfo = nullptr, const SDL_PropertiesID extensionProps = 0);
	
	GFXPipeline& operator=(const GFXPipeline& other);
	GFXPipeline(GFXPipeline&& other) noexcept;



	~GFXPipeline() { if (Handle != nullptr) {SDL_ReleaseGPUGraphicsPipeline(Application::GetInstance()->Device, Handle);} };
};

#endif