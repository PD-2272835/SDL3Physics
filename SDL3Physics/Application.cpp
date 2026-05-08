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


