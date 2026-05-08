#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_timer.h> //clock
#include <mutex>
#include "Buffer.hpp"


//https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
struct Clock
{
	uint32_t last_tick_time = 0;
	uint32_t delta = 0;

	void tick()
	{
		uint32_t tick_time = SDL_GetTicks();
		delta = tick_time - last_tick_time;
		last_tick_time = tick_time;
	}
};


class Application
{
private:
	static Application* pInstance_;
	static std::mutex mutex_;

protected:
	Application() : GFXPipeline(nullptr) {};

public:
	Application(Application& other) = delete;
	void operator=(const Application&) = delete;
	static Application* GetInstance();


	SDL_GPUGraphicsPipeline* GFXPipeline;
	Clock Time;

	SDL_GPUColorTargetInfo colorInfo = {};
	SDL_GPUColorTargetInfo backgroundInfo = {};
	SDL_GPUDepthStencilTargetInfo depthInfo = {};

	Buffer vSSBO; //vertexStorage Buffer
};

#endif