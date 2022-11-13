#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../../includes/window/window.h"

bool init_window(
	WindowParameter window_parameters,
	void* callback_args,
	void(callback)(WindowData,void*) 
)
{
	// init sdl and ttf
	if(SDL_Init(SDL_INIT_VIDEO) == -1)
		return false;

	if(TTF_Init() == -1)
	{
		SDL_Quit();

		return false;
	}

	// create the window based on parameters
	SDL_Window* window = SDL_CreateWindow(
		window_parameters.title,
		window_parameters.x < 0 ? SDL_WINDOWPOS_CENTERED : window_parameters.x, 
		window_parameters.y < 0 ? SDL_WINDOWPOS_CENTERED : window_parameters.y, 
		window_parameters.width,
		window_parameters.height,
		SDL_WINDOW_ALLOW_HIGHDPI
	);

	
	if(window == NULL)
	{
		SDL_Quit();
		TTF_Quit();

		return false;
	}

	WindowData window_data = {
		.window_parameters = window_parameters,
		.window = window,
		.renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
		.font = TTF_OpenFont(window_parameters.font_path, 16)
	};

	if
	(
		window_data.renderer == NULL ||
		window_data.font == NULL ||
		SDL_SetRenderDrawColor(
			window_data.renderer,
			window_data.window_parameters.bg_color.r,
			window_data.window_parameters.bg_color.g,
			window_data.window_parameters.bg_color.b,
			window_data.window_parameters.bg_color.a
		) != 0
	)
	{
		if(window_data.renderer != NULL)
			SDL_DestroyRenderer(window_data.renderer);

		if(window_data.font != NULL)
			TTF_CloseFont(window_data.font);

		SDL_DestroyWindow(window);
		SDL_Quit();
		TTF_Quit();

		return false;
	}

	// call the callback
	callback(window_data,callback_args);

	// free sdl resources
	SDL_DestroyRenderer(window_data.renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(window_data.font);

	SDL_Quit();
	TTF_Quit();

	return true;
}

