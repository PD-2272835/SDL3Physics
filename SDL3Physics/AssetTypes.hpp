#ifndef _ASSET_TYPES_HPP_
#define _ASSET_TYPES_HPP_

#include <vec.hpp>
#include <vector>
#include "Intersections.hpp"


struct GFXHandle
{
	uint32_t vertexOffset = 0;	//size of offset into vertex buffer in bytes
	uint32_t vertexSize = 0;	//size of vertex buffer data in bytes
	uint32_t indexOffset = 0;	//size of offset into index buffer in bytes
	uint32_t indexSize = 0;		//size of the index data in bytes
	bool gfxInitialized = false; //has this resource been loaded onto the GPU
	//THIS DOES NOT YET MANAGE TEXTURES
};

struct Asset
{
	GFXHandle handle = {};
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
	uint32_t ID;
	void* TexData;
	TextureType type;

	~Texture()
	{
		delete[] TexData;
	}
};


struct Mesh : Asset
{
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
	std::vector<Texture*> Textures;
	AABB Bounds;

	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture*> textures)
		: Vertices(vertices), Indices(indices), Textures(textures)
	{};
	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture*> textures, AABB bounds)
		: Vertices(vertices), Indices(indices), Textures(textures), Bounds(bounds)
	{};
};

struct Model : Asset
{
	std::vector<std::shared_ptr<Mesh>> Meshes;
};

#endif