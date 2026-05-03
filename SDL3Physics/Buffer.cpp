#include "Buffer.hpp"

Buffer::Buffer(SDL_GPUDevice* device, Uint8 usage, Uint32 size)
	: Device(device), Transfer(nullptr)
{
	Info = {
		usage, //buffer usage
		size, //size of buffer in bytes
		0 //extension properties
	};
	ID = SDL_CreateGPUBuffer(Device, &Info);
}


void Buffer::UploadData(SDL_GPUCommandBuffer* cmdBuffer, void* data, Uint32 dataSize, Uint32 destinationOffset)
{
	SDL_GPUTransferBufferCreateInfo createInfo { 
		SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, 
		dataSize, 
		0 
	};
	
	Transfer = SDL_CreateGPUTransferBuffer(Device, &createInfo);
	SDL_memcpy(SDL_MapGPUTransferBuffer(Device, Transfer, false), data, dataSize); //fill data into the transfer buffer
	SDL_UnmapGPUTransferBuffer(Device, Transfer);

	
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);

	//copy data from the first byte of the transfer buffer
	SDL_GPUTransferBufferLocation location{Transfer, 0};

	//Upload the full size of the transfer buffer to this buffer, 
	//starting from the user-provided byte offset into this buffer
	SDL_GPUBufferRegion region{ID, destinationOffset, dataSize};

	SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
	
	SDL_EndGPUCopyPass(copyPass); //must end pass before
	SDL_ReleaseGPUTransferBuffer(Device, Transfer);
}