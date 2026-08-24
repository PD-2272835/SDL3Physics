#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_
#include <SDL3/SDL_gpu.h>


/*
A GPU buffer management class
Info.usage flag (1u << 6) extends the functionality of SDL_GPUBufferUsageFlags to change the fucntionality 
of an instance to a texture management object, which behaves slightly differently to a regular data buffer
*/


class Buffer
{
public:
	SDL_GPUBufferCreateInfo Info; //.usage with flag (1u << 6) means that this buffer is a texture management object
	SDL_GPUDevice* Device;
	SDL_GPUBuffer* Handle;
	size_t End;

	Buffer() : Info({}), Device(nullptr), Handle(nullptr), End(0) {};
	Buffer(SDL_GPUDevice* device, uint8_t usage, size_t size);


	bool UploadData(SDL_GPUCommandBuffer* cmdBuffer, void* data, size_t dataSize, size_t destinationOffset);
	SDL_GPUTexture* UploadTexture(SDL_GPUCommandBuffer* cmdBuffer, const SDL_Surface* surface, const SDL_GPUTextureCreateInfo& textureInfo);

	//void* DownloadData(SDL_GPUCommandBuffer* cmdBuffer, Uint32 dataOffset, Uint32 size); TODO


	//TODO: Bind and unbind - maybe make these virtual,
	//and create specialized classes for different buffer types which override them?
	//void Bind();
	//void UnBind();

	//TODO: ~Buffer();
	//Delete should probably be private, but for now this is fine
	void Delete(); //This object must not be used after calling Delete()
};
#endif