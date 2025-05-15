//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL.h>
#include <SDL_image.h>

//#include <imgui.h>
//#include <imgui_impl_sdl2.h>
//#include <imgui_impl_sdlrenderer2.h>

#include "LTexture.h"
#include "LWindow.h"
#include "SceneManager.h"
#include "Game.h"
#include "Splash.h"

LWindow* gWindow;

//Frees media and shuts down SDL
void close();

void close()
{
	//ResourceManager::getInstance()->close();
	//Destroy window
	gWindow->free();
	// Cleanup
	/*ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();*/

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	//Initialize SDL
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) > 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	//Create window
	gWindow = gWindow->getInstance();
	if (!gWindow->init("BlackJack", SCREEN_WIDTH, SCREEN_HEIGHT))
	{
		printf("Windows could not be created! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	//// Setup Dear ImGui context
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	////io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	//// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	////ImGui::StyleColorsLight();

	//// Setup Platform/Renderer backends
	//ImGui_ImplSDL2_InitForSDLRenderer(gWindow->getInstance()->mWindow, gWindow->getInstance()->mRenderer);
	//ImGui_ImplSDLRenderer2_Init(gWindow->getInstance()->mRenderer);

	if (!success)
	{
		printf("Failed to initialize!\n");
		return -1;
	}
	else
	{
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;

		SceneManager s = SceneManager();
		s.pushScene(new Game());
		s.pushScene(new Splash());

		uint32_t time_step_ms = round(1000 / 5);
		uint32_t next_game_step = SDL_GetTicks(); // initial value

		//While application is running
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}

				//Handle window events
				gWindow->handleEvent(e);
				// (Where your code calls SDL_PollEvent())
				//ImGui_ImplSDL2_ProcessEvent(&e); // Forward your event to backend

				//Handle input for the player
				//game.HandleEvents(e);
				s.handleEvents(e);
			}

			uint32_t now = SDL_GetTicks();

			uint8_t computer_is_too_slow_limit = 5; // max number of updates per frame

			// Loop until all steps are executed or computer_is_too_slow_limit is reached
			while ((now > next_game_step) && (computer_is_too_slow_limit--)) {
				//game.Update();
				s.update();
				next_game_step += time_step_ms; // count 1 game tick done	
			}
			float interpolation = (float)(next_game_step - now) / (float)time_step_ms;
			//// Start the Dear ImGui frame
			//ImGui_ImplSDLRenderer2_NewFrame();
			//ImGui_ImplSDL2_NewFrame();
			//ImGui::NewFrame();
			//ImGui::ShowDemoWindow(); // Show demo window! 
			s.render(interpolation);
			// Rendering
			/*ImGui::Render();*/
			//SDL_RenderSetScale(gWindow->getInstance()->mRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
			//SDL_SetRenderDrawColor(gWindow->getInstance()->mRenderer, 40, 75, 168, 0xFF);
			//SDL_RenderClear(gWindow->getInstance()->mRenderer);
			/*ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), gWindow->getInstance()->mRenderer);*/
			SDL_RenderPresent(gWindow->getInstance()->mRenderer);

			//game.Render(gWindow->mRenderer);
			SDL_Delay(5);//cap at 200fps to prevent wg from using too much gpu rendering a static screen
		}
		//Free resources and close SDL
		s.popScene();
		//game.Unload();

		close();
	}
	return 0;
}