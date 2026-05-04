#ifndef _ASSET_LOADERS_HPP_
#define _ASSET_LOADERS_HPP_
#include <SDL3/SDL_iostream.h>
#include <vector>
#include <vec.hpp>
#include "AssetTypes.hpp"
#include <string>

struct Internal_FaceIndex
{
	Uint32 data[3];
};


std::string GetLineIter(const char* dataStream, const size_t& size, size_t& start);

template<size_t dim> mfg::vec<dim, float> ParseObjVector(std::string line);
std::vector<Internal_FaceIndex> ParseObjFace(std::string line);
Internal_FaceIndex ParseFaceIndex(std::string line);

std::shared_ptr<Model> LoadObj(const char* path);
Asset LoadTexture(const char* path);

#endif