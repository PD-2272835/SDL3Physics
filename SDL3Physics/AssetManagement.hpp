#ifndef _ASSET_MANAGEMENT_HPP_
#define _ASSET_MANAGEMENT_HPP_
#include <memory>
#include <map>
#include <filesystem>
#include <mutex>
#include <string>

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

	std::map<std::string, std::weak_ptr<Asset>> mAssets; //resource management idea from "cient" on youtube: https://www.youtube.com/watch?v=qGqCE2divWU

	std::shared_ptr<Asset> GetAsset(const std::string& AssetPath);
	std::shared_ptr<Asset> LoadAsset(const std::string& AssetPath);

};

#endif