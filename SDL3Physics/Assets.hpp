#ifndef _MESH_HPP_
#define _MESH_HPP_
#include <vec.hpp> //Maths
#include <vector> //std::vector
#include <memory>
#include <map>
#include <filesystem>


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


class AssetManagement
{
	std::map<const char*, std::weak_ptr<Asset>> mAssets; //resource management idea from "cient" on youtube: https://www.youtube.com/watch?v=qGqCE2divWU

	std::shared_ptr<Asset> GetAsset(const char* AssetPath);
	std::shared_ptr<Asset> LoadAsset(const char* AssetPath);
};

#endif