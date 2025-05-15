#include "ResourceManager.h"
#include "LWindow.h"

ResourceManager::ResourceManager() {
	std::map<std::string, LTexture*> assets = std::map<std::string, LTexture*>();

	addAsset("Error", "Data/Textures/error.png");
}

ResourceManager* ResourceManager::getInstance() {
	static ResourceManager instance = ResourceManager();
	return &instance;
}

bool ResourceManager::addAsset(std::string key, std::string path) {
	ResourceManager::assets[key] = new LTexture();
	//Load texture
	if (!ResourceManager::assets[key]->loadFromFile(path)) {
		printf("Failed to load texture %s!\n",key.c_str());
		delete ResourceManager::assets[key];
		return false;
	}
	return true;
}

LTexture* ResourceManager::getAsset(std::string key) {
	if (ResourceManager::assets[key] != nullptr) return ResourceManager::assets[key];
	else return ResourceManager::assets["Error"];
}

void ResourceManager::removeAsset(std::string key) {
	ResourceManager::assets[key]->free();
	delete ResourceManager::assets[key];
	ResourceManager::assets.erase(key);
}

void ResourceManager::removeAllAssets() {
	for (std::map<std::string, LTexture*>::iterator it = ResourceManager::assets.begin(); it != ResourceManager::assets.end(); ++it) {
		(*it).second->free();
		delete ResourceManager::assets[it->first];
		ResourceManager::assets.erase(it->first);
	}
}

void ResourceManager::close() {
	ResourceManager::gTextTexture.free();
	ResourceManager::removeAllAssets();
}