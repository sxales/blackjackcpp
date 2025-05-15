#pragma once

#include <SDL.h>
#include "Scene.h"
#include "LWindow.h"
#include "ResourceManager.h"

class Splash : public Scene {
private:
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;
	int count = 0;
	int opacity = 255;
	const int TIMEOUT = 10;
public:
	Splash() {}
	bool load() {
		ResourceManager::getInstance()->addAsset("logo", "Data/Textures/vyne.png");

		return true;
	}
	bool unload() {
		ResourceManager::getInstance()->removeAsset("logo");

		return true;
	}
	void render(float interpolation) {
		//Clear screen
		SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(LWindow::getInstance()->mRenderer);

		SDL_Rect dealerTexture = { 0, 0, 279, 115 };
		//SDL_Rect dealerResize = { 0, 0, w - 12, w - 12 };
		ResourceManager::getInstance()->getAsset("logo")->render((SCREEN_WIDTH - 279) / 2, (SCREEN_HEIGHT - 115) / 2, &dealerTexture, NULL);
		/*SDL_Rect outline1 = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 0xFF, 0xFF, 0xFF, opacity);
		SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &outline1);*/

		//Update screen
		SDL_RenderPresent(LWindow::getInstance()->mRenderer);
	}
	void update() {
		if (++count > TIMEOUT) notifyListeners();
		/*else if (count <= 10) opacity -= 25;
		else if (count <= TIMEOUT) opacity += 25;

		if (opacity < 0) opacity = 0;
		else if (opacity > 255) opacity = 255;*/
	}
	void handleEvents(SDL_Event& e) {}
};