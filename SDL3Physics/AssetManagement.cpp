#include "AssetManagement.hpp"

AssetManagement* AssetManagement::pInstance_{ nullptr };
std::mutex AssetManagement::mutex_;

AssetManagement *AssetManagement::GetInstance()
{
	std::lock_guard<std::mutex>lock(mutex_);
	if (pInstance_ == nullptr)
	{
		pInstance_ = new AssetManagement();
	}
	return pInstance_;
}


std::shared_ptr<Asset> AssetManagement::GetAsset(const char* AssetPath)
{
	auto iterator = mAssets.find(AssetPath);
	std::shared_ptr<Asset> res;
	if (iterator != mAssets.end())
	{
		if (res = iterator->second.lock())
		{
			return res; //Asset is already loaded
		}
	}

	//Asset not loaded
	res = AssetManagement::LoadAsset(AssetPath); //create new entry/update Asset Entry
	mAssets[AssetPath] = res;
	return res;
}

//this assumes a relative filepath, and that the file actually exists
std::shared_ptr<Asset> AssetManagement::LoadAsset(const char* AssetPath)
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
			LoadObj(AssetPath);

		}
	}

	return nullptr;
}