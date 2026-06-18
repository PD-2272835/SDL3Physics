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

//adapted from cient on youtube: https://youtu.be/qGqCE2divWU?t=247
std::shared_ptr<Asset> AssetManagement::GetAsset(const std::string& AssetPath)
{
	auto iterator = mAssets.find(AssetPath);

	//Asset is already loaded
	if (iterator != mAssets.end())
	{
		if (std::shared_ptr<Asset> res = iterator->second.lock())
		{
			return res;
		}
	}
	std::cout << "Loading CPU Asset: ";

	//Asset not loaded
	std::shared_ptr<Asset> res = AssetManagement::LoadAsset(AssetPath); //create new entry/update Asset Entry
	mAssets[AssetPath] = res;
	return res;
}


//this assumes a relative filepath
std::shared_ptr<Asset> AssetManagement::LoadAsset(const std::string& AssetPath)
{
	std::shared_ptr<Asset> asset = nullptr;

	//get the file extension
	std::filesystem::path p(AssetPath);
	if (std::filesystem::exists(AssetPath) && p.has_extension())
	{
		std::cout << p.filename() << "\n";
		if (p.extension() == ".png")
		{
			//TODO: load image/texture
			asset = std::shared_ptr<Asset>(new Texture());
		}
		else if (p.extension() == ".obj") {
			asset = std::shared_ptr<Asset>(new Mesh());
		}
	}
	
	if (asset != nullptr) asset.get()->LoadFromDisk(AssetPath.c_str());
	return asset;
}


bool AssetManagement::RegisterResource(const std::string& key, const std::shared_ptr<Asset> ref)
{
	//TODO: Finish asset bundles
	mAssets[key] = ref;
	return false;
}