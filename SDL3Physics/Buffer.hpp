#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_
#include <SDL3/SDL_gpu.h>


class Buffer
{
public:
	SDL_GPUBufferCreateInfo Info;
	SDL_GPUDevice* Device;
	SDL_GPUBuffer* Handle;
	size_t End;

	Buffer() : Info({}), Device(nullptr), Handle(nullptr), End(0) {};
	Buffer(SDL_GPUDevice* device, uint8_t usage, size_t size);


	bool UploadData(SDL_GPUCommandBuffer* cmdBuffer, void* data, size_t dataSize, size_t destinationOffset);
	//void* DownloadData(SDL_GPUCommandBuffer* cmdBuffer, Uint32 dataOffset, Uint32 size); TODO
	

	//TODO: Bind and unbind - maybe make these virtual,
	//and create specialized classes for different buffer types which override them?
	//void Bind();
	//void UnBind();

	
	void Delete(); //You must not use this buffer after calling Delete()
};

#endif