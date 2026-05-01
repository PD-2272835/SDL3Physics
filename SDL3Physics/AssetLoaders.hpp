#ifndef _ASSET_LOADERS_HPP_
#define _ASSET_LOADERS_HPP_
#include <SDL3/SDL_iostream.h>
#include <vector>
#include <vec.hpp>
#include "AssetTypes.hpp"
#include <string>

std::string GetLineIter(const char* dataStream, const size_t& size, size_t& start);

template<size_t dim> mfg::vec<dim, float> ParseObjCoord(std::string& line);

Asset LoadObj(const char* path);
Asset LoadTexture(const char* path);

#endif