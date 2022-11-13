#if !defined(WINDOW_H)
#define WINDOW_H

	#include <stdbool.h>
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_ttf.h>

	#define WINDOW_DEFAULT_PARAMETERS {\
		.title = "Fenêtre",\
		.width = 700,\
		.height = 450,\
		.bg_color = {\
			.r = 23,\
			.g = 35,\
			.b = 64,\
			.a = 0\
		},\
		.text_color = {\
			.r = 117,\
			.g = 136,\
			.b = 168,\
			.a = 0\
		},\
		.x = -1,\
		.y = -1,\
		.font_path = "default"\
	}	
	
	typedef struct
	{
		char title[50];
		unsigned int width;
		unsigned int height;
		SDL_Color bg_color;
		SDL_Color text_color;
		int x;
		int y;
		char font_path[100];
	}WindowParameter;

	typedef struct
	{
		SDL_Window* window;
		WindowParameter window_parameters;
		SDL_Event event;
		SDL_Renderer* renderer;
		TTF_Font* font;
	}WindowData;

	bool init_window(WindowParameter window_parameters,void* callback_args,void(callback)(WindowData,void*) );
#endif