#ifndef _BUFFER_HPP_
#define _BUFFER_HPP_
#include <SDL3/SDL_gpu.h>

class Buffer
{
	SDL_GPUDevice* device;
	SDL_GPUBufferCreateInfo Info;
	SDL_GPUBuffer* ID;
	bool isTransferring;

	Buffer(SDL_GPUDevice device, Uint32 size, Uint8 usage);

	//you must call EndTransfer() and must not deallocate the returned ptr
	void* GetUploadHandle(Uint32 size);
	void* GetDownloadHandle(Uint32 size);
	
	void EndUpload(int transferBufferOffset, int targetRegionOffset);
	void EndDownload();
};

#endif