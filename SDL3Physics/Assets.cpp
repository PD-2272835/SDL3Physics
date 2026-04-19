#include "Assets.hpp"

std::shared_ptr<Asset> AssetManagement::GetAsset(const char* AssetPath)
{
	auto iterator = mAssets.find(AssetPath);
	if (iterator != mAssets.end())
	{
		if (std::shared_ptr<Asset> res = iterator->second.lock())
		{
			return res; //Asset is already loaded
		}
	}

	//Asset not loaded
	AssetManagement::LoadAsset(AssetPath); //Asset not in map, create new entry
}

//this assumes a relative filepath, and that the file actually exists
Asset* AssetManagement::LoadAsset(const char* AssetPath)
{
	//get the file extension
	std::filesystem::path p(AssetPath);
	if (p.has_extension())
	{
		if (p.extension() == ".img" || ".bmp" || ".png" || ".jpg")
		{
			//TODO: load image/texture
		}
		else if (p.extension() == ".obj") {
			//TODO: load model
		}
	}

}