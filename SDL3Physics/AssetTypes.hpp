#ifndef _ASSET_TYPES_HPP_
#define _ASSET_TYPES_HPP_

#include <vec.hpp>
#include <vector>
#include "Intersections.hpp"
#include "Buffer.hpp"
#include "SDL3/SDL_gpu.h"
#include <memory>


struct BufferHandle
{
	Buffer* buffer; //what buffer is this data stored in?
	//strides are vertices/indices
	size_t size = 0; //size of buffer data in strides
	size_t offset = 0; //size of offset into buffer in strides
};

struct GFXHandle
{
	//std::vector<BufferHandle> buffers = {}; //buffers where this data is stored
	BufferHandle textureBuffer = {};
	BufferHandle vertexBuffer = {};
	BufferHandle indexBuffer = {};
	bool isGfxInitialized = false; //has this resource been loaded onto the GPU?
};

class Asset
{
public:
	GFXHandle Handle = {};

	//I strongly dislike this syntax for abstract functions
	//Concrete definitions are in AssetLoaders.hpp
	virtual bool LoadFromDisk(const char* filename) = 0;
	virtual bool UploadToGPU(SDL_GPUCommandBuffer* cmdBuffer, const Buffer* textureBuffer, Buffer* vertexBuffer = nullptr, Buffer* indexBuffer = nullptr) = 0;

	virtual ~Asset()
	{
		//TODO: remove from GPU upon deletion? 
		//buffers will be cleared after program execution, however long runtimes could cause issues
	};
};


struct Vertex
{
	sgm::vec3 Position;
	sgm::vec3 Normal;
	sgm::vec2 UV;

	Vertex() : Position({}), Normal({}), UV({}) {};
	Vertex(sgm::vec3 position, sgm::vec3 normal, sgm::vec2 uv)
		: Position(position), Normal(normal), UV(uv)
	{};
};

enum TextureType
{
	Diffuse = 0, //color
	Specular, //shiny
	Normal,
	Metallic,
	Roughness
};

struct Texture : Asset
{
	SDL_GPUTexture* texHandle = nullptr;
	SDL_GPUTextureCreateInfo Info = {};
	TextureType type = Diffuse;

	Texture() {};

	bool LoadFromDisk(const char* filepath) override;
	bool UploadToGPU(SDL_GPUCommandBuffer* cmdBuffer, const Buffer* textureBuffer, Buffer* vertexBuffer = nullptr, Buffer* indexBuffer = nullptr) override;

	~Texture()
	{
		SDL_ReleaseGPUTexture(Handle.textureBuffer.buffer->Device, texHandle);
	};
};


struct Mesh : Asset
{
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<std::shared_ptr<Texture>> Textures;
	AABB Bounds;

	Mesh() : Vertices({}), Indices({}), Textures({}), Bounds({}) {};
	
	Mesh(std::vector<Vertex> vertices, 
		std::vector<uint32_t> indices, 
		std::vector<std::shared_ptr<Texture>> textures)
		: Vertices(vertices), Indices(indices), Textures(textures)
	{};
	
	Mesh(std::vector<Vertex> vertices, 
		std::vector<uint32_t> indices, 
		std::vector<std::shared_ptr<Texture>> textures, 
		AABB bounds)
		: Vertices(vertices), Indices(indices), Textures(textures), Bounds(bounds)
	{};

	~Mesh() {};

	bool LoadFromDisk(const char* filepath) override;
	bool UploadToGPU(SDL_GPUCommandBuffer* cmd, const Buffer* textureBuffer, Buffer* vertexBuffer = nullptr, Buffer* indexBuffer = nullptr) override;
};

struct Model : Asset
{
	std::vector<std::shared_ptr<Mesh>> Meshes;
};

#endif