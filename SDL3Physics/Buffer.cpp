#include "Buffer.hpp"

Buffer::Buffer(SDL_GPUDevice* device, uint8_t usage, size_t size)
{
	Device = device;
	Info = {
		usage, //buffer usage
		static_cast<Uint32>(size), //size of buffer in bytes
		0 //extension properties
	};
	Handle = SDL_CreateGPUBuffer(Device, &Info);
	End = 0; //tail of buffer data in bytes
}


//dataSize = size of data in bytes
//destinationOffset = offset of data into the buffer in bytes
bool Buffer::UploadData(SDL_GPUCommandBuffer* cmdBuffer, void* data, size_t dataSize, size_t destinationOffset)
{
	if (End + dataSize > Info.size) //check that there is space in this buffer
	{
		return false;
	}

	SDL_GPUTransferBufferCreateInfo createInfo { 
		SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, 
		dataSize, 
		0 
	};
	
	SDL_GPUTransferBuffer* transfer = SDL_CreateGPUTransferBuffer(Device, &createInfo);
	SDL_memcpy(SDL_MapGPUTransferBuffer(Device, transfer, false), data, dataSize); //fill data into the transfer buffer
	SDL_UnmapGPUTransferBuffer(Device, transfer);


	//copy data from the first byte of the transfer buffer
	SDL_GPUTransferBufferLocation location
	{
		transfer,
		0
	};

	//Upload the full size of the transfer buffer to this buffer, 
	//starting from the user-provided byte offset into this buffer
	SDL_GPUBufferRegion region
	{
		Handle, 
		destinationOffset, 
		dataSize
	};

	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);
	SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
	
	SDL_EndGPUCopyPass(copyPass); //must end pass before
	SDL_ReleaseGPUTransferBuffer(Device, transfer);

	End += dataSize;
	return true;
}

SDL_GPUTexture* Buffer::UploadTexture(SDL_GPUCommandBuffer* cmdBuffer, const SDL_Surface* surface, const SDL_GPUTextureCreateInfo &textureInfo)
{
	if (!(Info.usage & (1u << 6))) return nullptr; //not sure if this check even works


	//Create and fill a GPU transfer buffer to send to the GPU
	SDL_GPUTransferBufferCreateInfo createInfo
	{
		SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		static_cast<Uint32>(surface->w * surface->h * 4),
		0
	};

	SDL_GPUTransferBuffer* transfer = SDL_CreateGPUTransferBuffer(Device, &createInfo);
	SDL_memcpy(SDL_MapGPUTransferBuffer(Device, transfer, false), surface->pixels, surface->w * surface->h * 4);
	SDL_UnmapGPUTransferBuffer(Device, transfer);

	//Create the Texture
	SDL_GPUTexture* texture = SDL_CreateGPUTexture(Device, &textureInfo);
	if (texture == nullptr) 
	{
		return nullptr;
	}

	SDL_GPUTextureTransferInfo transferInfo
	{
		transfer,
		0, 
	};

	SDL_GPUTextureRegion region{
		texture,
		static_cast<Uint32>(surface->w),
		static_cast<Uint32>(surface->h),
		1
	};

	//upload the surface data to the GPU
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);
	SDL_UploadToGPUTexture(copyPass, &transferInfo, &region, false);
	SDL_EndGPUCopyPass(copyPass);
	
	SDL_ReleaseGPUTransferBuffer(Device, transfer);

	return texture;
}

void Buffer::Delete()
{
	SDL_ReleaseGPUBuffer(Device, Handle);
	Device = nullptr;
	Info = {};
}