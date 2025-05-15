#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>
#include "Listener.h"

class Scene : public Subject {
private:
	
public:
	virtual void update() {};
	virtual void render(float interpolation) {};
	virtual void handleEvents(SDL_Event& e) {}
	virtual bool load() { return true; };
	virtual bool unload() { return true; };
};

#endif