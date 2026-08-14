#ifndef _ASSET_LOADERS_HPP_
#define _ASSET_LOADERS_HPP_
#include <SDL3/SDL_iostream.h>
#include <vector> //lists
#include <vec.hpp> //sgm mathematical vectors
#include "AssetTypes.hpp" //defininitions for Texture and other asset variations
#include "SDL3_image/SDL_image.h"
#include "Intersections.hpp"
#include <string>

//this is bad - dirties intellisense/autocomplete
struct Internal_FaceIndex
{
	Uint32 data[3]{};
};

//Helper fucntions for obj parsing
std::string GetLineIter(const char* dataStream, const size_t& size, size_t& start);

template<size_t dim> sgm::vec<dim, float> ParseObjVector(std::string line);

std::vector<Internal_FaceIndex> ParseObjFace(std::string line);

Internal_FaceIndex ParseFaceIndex(std::string line);


#endif