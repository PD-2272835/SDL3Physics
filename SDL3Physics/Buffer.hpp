#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_
#include <SDL3/SDL_gpu.h>

class Buffer
{
public:
	SDL_GPUBufferCreateInfo Info;
	SDL_GPUDevice* Device;
	SDL_GPUBuffer* ID;

	Buffer(SDL_GPUDevice* device, Uint8 usage, Uint32 size);


	void UploadData(SDL_GPUCommandBuffer* cmdBuffer, void* data, Uint32 dataSize, Uint32 destinationOffset);
	//void* DownloadData(Uint32 size);
	
	//void Bind();
	//void UnBind();

	
	void Delete(); //You must not use this buffer after calling Delete()
};

#endif