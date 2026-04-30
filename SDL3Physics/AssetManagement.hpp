#ifndef _ASSET_MANAGEMENT_HPP_
#define _ASSET_MANAGEMENT_HPP_
#include <memory>
#include <map>
#include <filesystem>
#include <mutex>

#include "AssetTypes.hpp"
#include "AssetLoaders.hpp"

class AssetManagement
{
private:
	static AssetManagement* pInstance_; //this ptr will remain the same throughout runtime execution
	static std::mutex mutex_;

protected:
	AssetManagement() {};

public:
	AssetManagement(AssetManagement &other) = delete;
	void operator=(const AssetManagement&) = delete;
	static AssetManagement* GetInstance();

	std::map<const char*, std::weak_ptr<Asset>> mAssets; //resource management idea from "cient" on youtube: https://www.youtube.com/watch?v=qGqCE2divWU

	std::shared_ptr<Asset> GetAsset(const char* AssetPath);
	std::shared_ptr<Asset> LoadAsset(const char* AssetPath);

};

#endif