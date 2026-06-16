#ifndef _APPLICATION_HPP_
#define _APPLICATION_HPP_

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_timer.h> //clock
#include <mutex>
#include "Buffer.hpp"
#include "Camera.hpp"


//https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
struct Clock
{
	uint32_t last_tick_time = 0;
	double delta = 0;
	double unscaledTimeDelta = 0;
	float timeScale = 0.f;

	void tick()
	{
		uint32_t tick_time = SDL_GetTicks();
		unscaledTimeDelta = (tick_time - last_tick_time) / 1000.;
		delta = unscaledTimeDelta * timeScale;
		last_tick_time = tick_time;
	}
};


class Application
{
private:
	static Application* pInstance_;
	static std::mutex mutex_;

protected:
	Application() : GFXPipeline(nullptr), Device(nullptr), Window(nullptr)  {};
	~Application();

public:
	Application(Application& other) = delete;
	void operator=(const Application&) = delete;
	static Application* GetInstance();

	void Init(SDL_GPUDevice* device, uint32_t width, uint32_t height, SDL_Window* window);

	SDL_GPUDevice* Device; //all GPU work will depend on this
	SDL_GPUGraphicsPipeline* GFXPipeline; //this should be moved elsewhere? or should be Pipeline Type
	
	uint32_t WindowWidth = 1920;
	uint32_t WindowHeight = 1080;
	SDL_Window* Window;
	
	Clock Time;
	Camera mainCamera;
	
	SDL_GPUTexture* DepthTexture;

	SDL_GPUColorTargetInfo colorInfo = {};
	SDL_GPUColorTargetInfo backgroundInfo = {};
	SDL_GPUDepthStencilTargetInfo depthInfo = {};

	Buffer vSSBO; //vertexStorage Buffer
};

#endif