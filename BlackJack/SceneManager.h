#pragma once

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <list>
#include <memory>
#include <SDL.h>
#include "Scene.h"
#include "Listener.h"

//class Scene;
//class Message;

class SceneManager : public Listener {
private:
	std::list<std::unique_ptr<Scene>> scenes;
	//void notify(Message m);
public:
	void update() { if (scenes.size() > 0) scenes.back()->update(); };
	void render(float interpolation) { if (scenes.size() > 0) scenes.back()->render(interpolation); };
	void handleEvents(SDL_Event& e) { if (scenes.size() > 0) scenes.back()->handleEvents(e); };

	void pushScene(Scene* s) { 
		if (s->load()) {
			scenes.push_back(std::unique_ptr<Scene>(s));
			s->addListener(this);
		}
		else throw "Unable to load scene!";
	};
	void popScene() { if (scenes.back()->unload()) scenes.pop_back(); else throw "Unable to unload scene!"; };
	void notify() { popScene(); };
};

#endif