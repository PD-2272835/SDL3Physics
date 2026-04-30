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
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);
};

struct Model : Asset
{
	std::vector<std::shared_ptr<Mesh>> Meshes;
};

#endif