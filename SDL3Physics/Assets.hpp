#ifndef _MESH_HPP_
#define _MESH_HPP_
#include <vec.hpp> //Maths
#include <vector> //std::vector



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

struct Texture
{
	uint32_t ID;
	TextureType type;
};


struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);
};


//could create an asset manager that uses a map of weak pointers to shared pointers to manage the lifetime of resources

#endif