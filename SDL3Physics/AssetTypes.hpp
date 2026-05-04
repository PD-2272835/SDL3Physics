#ifndef _ASSET_TYPES_HPP_
#define _ASSET_TYPES_HPP_

#include <vec.hpp>
#include <vector>

struct Asset {};

struct Vertex
{
	mfg::vec3 Position;
	mfg::vec3 Normal;
	mfg::vec2 UV;

	Vertex();
	Vertex(mfg::vec3 position, mfg::vec3 normal, mfg::vec2 uv)
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

	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture*> textures)
		: Vertices(vertices), Indices(indices), Textures(textures)
	{};
};

struct Model : Asset
{
	std::vector<std::shared_ptr<Mesh>> Meshes;
};

#endif