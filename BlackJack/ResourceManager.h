#pragma once

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "LTexture.h"
#include <string>
#include <map>
using namespace std;

class ResourceManager {
private:
	ResourceManager();
	std::map<std::string, LTexture*> assets;
	//Rendered texture
	LTexture gTextTexture;
public: 
	static ResourceManager* getInstance();
	bool addAsset(std::string key, std::string path);
	LTexture* getAsset(std::string key);
	void removeAsset(std::string key);
	void removeAllAssets();
	void close();
};

#endif