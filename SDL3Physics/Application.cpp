#include "Application.hpp"


Application* Application::pInstance_{ nullptr };
std::mutex Application::mutex_;

Application* Application::GetInstance()
{
	std::lock_guard<std::mutex>lock(mutex_);
	if (pInstance_ == nullptr)
	{
		pInstance_ = new Application();
	}
	return pInstance_;
}

void Application::Init(SDL_GPUDevice* device, uint32_t width, uint32_t height, SDL_Window* window)
{
	Device = device;
	WindowWidth = width;
	WindowHeight = height;
	Window = window;

	diffuseSampler = SDL_CreateGPUSampler(Device, &samplerInfo);
}


Application::~Application()
{
	
}