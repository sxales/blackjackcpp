#pragma once

#include <string>
#include "Rect.h"
#include "LWindow.h"
#include "ResourceManager.h"

class Button : public Rect {
private:
	std::string txt;
	bool over=false;
public:
	Button() {}
	Button(std::string _txt, int _x, int _y, int _width, int _height) {
		txt = _txt;
		over = false;

		x = _x;
		y = _y;
		width = _width;
		height = _height;
	}

	void draw() {
		/*SDL_Rect outline1 = { x, y, width, height };
		if (over) SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 112, 163, 204, 0xFF);
		else SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 65, 105, 225, 0xFF);
		SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &outline1);*/

		drawBox(((over) ? 10 : 12), x, y, width, height);
		int fs = height / 2;
		renderText(txt, x + (width - fs * txt.length()) / 2, y + fs / 2, ResourceManager::getInstance()->getAsset("font"), ((over) ? 8 : 1), fs);
	}

	#define FONT_SIZE 40
	void renderText(std::string message, int x, int y, LTexture* font, int subtype, int size) {
		for (int i = 0; i < message.length(); i++) {
			int ch = message.at(i);
			SDL_Rect spriteTexture;
			if (ch < 126 && ch >= 32) {
				//select sprite texture
				spriteTexture = { (ch - 32) * FONT_SIZE, subtype * FONT_SIZE, FONT_SIZE, FONT_SIZE };
			}
			else {
				//draw question mark
				//select sprite texture
				spriteTexture = { 41 * FONT_SIZE,  subtype * FONT_SIZE, FONT_SIZE, FONT_SIZE };
			}
			//scale to on screen size
			SDL_Rect rescale = SDL_Rect{ 0, 0, size, size };
			//renter the actor
			font->render(x + (i * size), y, &spriteTexture, &rescale, 0);
		}
	}

	void drawBox(int color, int x, int y, int width, int height) {
		SDL_Rect texture = { 64 * (color % 8), 64 * (int)floor(color / 8), 16, 16 };//top left corner
		SDL_Rect resize = { 0, 0, 16, 16 };
		ResourceManager::getInstance()->getAsset("tileset")->render(x, y, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//top left corner
		ResourceManager::getInstance()->getAsset("tileset")->render(x, y + height - 16, &texture, &resize, 0.0, NULL, SDL_FLIP_VERTICAL);//bottom left 
		ResourceManager::getInstance()->getAsset("tileset")->render(x + width - 16, y, &texture, &resize, 0.0, NULL, SDL_FLIP_HORIZONTAL);//top right corner
		ResourceManager::getInstance()->getAsset("tileset")->render(x + width - 16, y + height - 16, &texture, &resize, 0.0, NULL, SDL_RendererFlip(SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL));//bottom right corner

		texture = { 64 * (color % 8) + 16, 64 * (int)floor(color / 8), 16, 16 };
		resize = { 0, 0, width - 32, 16 };
		ResourceManager::getInstance()->getAsset("tileset")->render(x + 16, y, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//top
		ResourceManager::getInstance()->getAsset("tileset")->render(x + 16, y + height - 16, &texture, &resize, 0.0, NULL, SDL_FLIP_VERTICAL);//bottom

		texture = { 64 * (color % 8), 64 * (int)floor(color / 8) + 16, 16, 16 };
		resize = { 0, 0, 16, height - 32 };
		ResourceManager::getInstance()->getAsset("tileset")->render(x, y + 16, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//left
		ResourceManager::getInstance()->getAsset("tileset")->render(x + width - 16, y + 16, &texture, &resize, 0.0, NULL, SDL_FLIP_HORIZONTAL);//right

		texture = { 64 * (color % 8) + 16, 64 * (int)floor(color / 8) + 16, 16, 16 };
		resize = { 0, 0, width - 32, height - 32 };
		ResourceManager::getInstance()->getAsset("tileset")->render(x + 16, y + 16, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//fill
	}

	bool check(int _x, int _y) {
		if (overlaps(Rect(_x, _y, 1, 1))) over = true;
		else over = false;

		return over;
	}
};