#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "../../includes/utils/util.h"
#include "../../includes/window/project.h"

#define MAX_LINE_SIZE 500
#define COUNT_OF_INSTRUCTIONS 5
#define IMAGES_DIMENSION_W 250
#define IMAGES_DIMENSION_H 220

#define FREE_TEXT_TEXTURES for(int i = 0; i < COUNT_OF_INSTRUCTIONS; i++)\
			SDL_DestroyTexture(text_textures[i]);

#define FREE_LANG_TEXTURES for(int i = 0; i < datas->count_of_langs; i++)\
			SDL_DestroyTexture(lang_textures[i]);

#define FREE_LANG_ALLOCATED_DATAS free(lang_textures);\
		free(langs_rects);

#define CHOICE_INDICATOR_CONFIG choice_indicator_rect.x = langs_rects[*(current_lang)].x + langs_rects[*(current_lang)].w + 10;\
			choice_indicator_rect.y = langs_rects[*(current_lang)].y;\
			choice_indicator_rect.h = langs_rects[*(current_lang)].h;

#define SET_RESPONSE for(int i = 0; i < datas->count_of_langs; i++)\
     {\
         if(strcmp(datas->langs[*(current_lang)],to_ask_data->langs_values[i]->lang) == 0)\
         {\
             response = to_ask_data->langs_values[i]->value;\
             break;\
         }\
	}

// print the lang choice elements on window , return true if success or false if user close window or something failed
bool make_lang_choice(WindowData window_data,DataGetterReturn* datas,int* current_lang)
{
	// start pos
	int start = (40 * COUNT_OF_INSTRUCTIONS) + 40;

	bool end_well = true;
	bool quit_loop = false;

	char instructions[COUNT_OF_INSTRUCTIONS][MAX_LINE_SIZE] = {
		"Veuillez choisir la langue en utilisant les fleches haut et bas !",
		"Appuyez sur la touche <entree> pour valider",
		"Appuyez sur d pour choisir la langue par defaut",
		"Entrez la commande special change_lang! pour changer la langue",
		"Entrez la commande special end_game! pour quitter le jeux"
	};

	SDL_Rect text_rects[COUNT_OF_INSTRUCTIONS];
	SDL_Texture* text_textures[COUNT_OF_INSTRUCTIONS];
	SDL_Surface* text_surface;

	// try to print instructions
	for(int i = 0; i < COUNT_OF_INSTRUCTIONS; i++)
	{
		text_surface = TTF_RenderText_Solid(window_data.font,instructions[i],window_data.window_parameters.text_color);

		if(text_surface == NULL)
		{
			for(int i_ = 0; i_ < i; i_++)
				SDL_DestroyTexture(text_textures[i_]);

			return false;
		}

		text_textures[i] = SDL_CreateTextureFromSurface(window_data.renderer,text_surface);

		if(text_textures[i] == NULL)
		{
			for(int i_ = 0; i_ < i; i_++)
				SDL_DestroyTexture(text_textures[i_]);

			SDL_FreeSurface(text_surface);

			return false;
		}

		text_rects[i].x = 10;
		text_rects[i].y = 40 * (i + 1);
		text_rects[i].w = text_surface->w;
		text_rects[i].h = text_surface->h;

		SDL_FreeSurface(text_surface);
	}

	// allocate lang textures and rects array
	SDL_Texture** lang_textures = malloc(sizeof(SDL_Texture*) * datas->count_of_langs);
	SDL_Rect* langs_rects = malloc(sizeof(SDL_Rect) * datas->count_of_langs);

	if
	(
		lang_textures == NULL || 
		langs_rects == NULL
	)
	{
		for(int i = 0; i < COUNT_OF_INSTRUCTIONS; i++)
			SDL_DestroyTexture(text_textures[i]);

		if(langs_rects != NULL)
			free(langs_rects);

		if(lang_textures != NULL)
			free(lang_textures);

		return false;
	}

	// try to print langs
	for(int i = 0; i < datas->count_of_langs; i++)
	{
		text_surface = TTF_RenderText_Solid(window_data.font,datas->langs[i],window_data.window_parameters.text_color);

		if(text_surface == NULL)
		{
			for(int i_ = 0; i_ < i; i_++)
				SDL_DestroyTexture(lang_textures[i_]);

			FREE_TEXT_TEXTURES

			free(lang_textures);
			free(langs_rects);

			return false;
		}

		lang_textures[i] = SDL_CreateTextureFromSurface(window_data.renderer,text_surface);

		if(lang_textures[i] == NULL)
		{
			for(int i_ = 0; i_ < i; i_++)
				SDL_DestroyTexture(text_textures[i_]);

			SDL_FreeSurface(text_surface);

			free(lang_textures);
			free(langs_rects);

			return false;
		}

		langs_rects[i].x = 10;
		langs_rects[i].y = start + (30 * (i + 1) );
		langs_rects[i].w = text_surface->w;
		langs_rects[i].h = text_surface->h;

		SDL_FreeSurface(text_surface);
	}

	// create the choice indicator
	text_surface = SDL_CreateRGBSurface(0,10,15,32,0,0,0,0);
	
	SDL_Texture* choice_indicator_texture = NULL;
	
	SDL_Rect choice_indicator_rect;
	
	if(text_surface != NULL)
	{
		SDL_FillRect(
			text_surface,
			NULL,
			SDL_MapRGB(
				text_surface->format,
				window_data.window_parameters.text_color.r,
				window_data.window_parameters.text_color.g,
				window_data.window_parameters.text_color.b
			)
		);

		choice_indicator_texture = SDL_CreateTextureFromSurface(window_data.renderer,text_surface);

		if(choice_indicator_texture != NULL)
		{
			choice_indicator_rect.w = text_surface->w;
			CHOICE_INDICATOR_CONFIG
		}
	}

	if
	(
		text_surface == NULL ||
		choice_indicator_texture == NULL
	)
	{
		FREE_TEXT_TEXTURES
		FREE_LANG_TEXTURES

		if(text_surface != NULL)
			SDL_FreeSurface(text_surface);

		if(choice_indicator_texture != NULL)
			SDL_DestroyTexture(choice_indicator_texture);

		FREE_LANG_ALLOCATED_DATAS

		return false;
	}

	SDL_FreeSurface(text_surface);

	while(!quit_loop)
	{
		SDL_WaitEvent(&window_data.event);

		switch(window_data.event.type)
		{
			case SDL_QUIT:
				end_well = false;
				quit_loop = true;
			; break;

			case SDL_KEYDOWN:
				switch(window_data.event.key.keysym.sym)
				{
					case SDLK_RETURN:
						// end the lang getter
						quit_loop = true;
					; break;

					case SDLK_d:
						// set the current lang as default lang and end the lang getter
						*(current_lang) = 0;
						quit_loop = true;
					; break;

					case SDLK_UP:
						// move the current lang
						*(current_lang) = *(current_lang) == 0 ? datas->count_of_langs - 1 : *(current_lang) - 1;

						// replace the current choice cursor
						CHOICE_INDICATOR_CONFIG
					; break;

					case SDLK_DOWN:
						// move the current lang
						*(current_lang) = *(current_lang) == datas->count_of_langs - 1 ? 0 : *(current_lang) + 1;

						// replace the current choice cursor
						CHOICE_INDICATOR_CONFIG;
					; break;
				}
			; break;
		}

		// print elements

		SDL_RenderClear(window_data.renderer);

		for(int i = 0; i < COUNT_OF_INSTRUCTIONS; i++)
			SDL_RenderCopy(window_data.renderer,text_textures[i],NULL,text_rects + i);

		for(int i = 0; i < datas->count_of_langs; i++)
			SDL_RenderCopy(window_data.renderer,lang_textures[i],NULL,langs_rects + i);

		SDL_RenderCopy(window_data.renderer,choice_indicator_texture,NULL,&choice_indicator_rect);

		SDL_RenderPresent(window_data.renderer);
	}

	// free resources

	FREE_TEXT_TEXTURES
	FREE_LANG_TEXTURES
	FREE_LANG_ALLOCATED_DATAS

	SDL_DestroyTexture(choice_indicator_texture);

	return end_well;
}

// print the image to user and get response, return true if success or false if user close window or something failed
bool ask_player(WindowData window_data,DataGetterReturn* datas,int* current_lang,int* player_score,int* count_of_try)
{
	// get a random data in list
	Data* to_ask_data = datas->datas[rand() % datas->count_of_datas];

	bool end_choice = false;
	bool end_well = true;

	char* response;

	char player_input[MAX_LINE_SIZE];
	char current_state[35];

	// get the response's the player have to write
	SET_RESPONSE


	memset(player_input,0,sizeof(char) * MAX_LINE_SIZE);

	// load image
	SDL_Surface* image_surface = IMG_Load(to_ask_data->img_path);

	if(image_surface == NULL)
		return false;

	SDL_Texture* image_texture = SDL_CreateTextureFromSurface(window_data.renderer,image_surface);

	if(image_texture == NULL)
	{
		SDL_FreeSurface(image_surface);

		return false;
	}

	// center the image in screen
	SDL_Rect image_rect = {
		.x = (window_data.window_parameters.width - IMAGES_DIMENSION_W) / 2,
		.y = 40,
		.w = IMAGES_DIMENSION_W,
		.h = IMAGES_DIMENSION_H
	};

	SDL_FreeSurface(image_surface);

	// create the enter name text
	SDL_Surface* enter_name_surface = TTF_RenderText_Solid(window_data.font,"ENTREZ LE NOM",window_data.window_parameters.text_color);
	
	if(enter_name_surface == NULL)
	{
		SDL_DestroyTexture(image_texture);

		return false;
	}

	SDL_Texture* enter_name_texture = SDL_CreateTextureFromSurface(window_data.renderer,enter_name_surface);

	if(enter_name_texture == NULL)
	{
		SDL_DestroyTexture(image_texture);
		SDL_FreeSurface(enter_name_surface);

		return false;
	}

	SDL_Rect enter_name_rect = {
		.x = (window_data.window_parameters.width - enter_name_surface->w) / 2,
		.y = 70 + IMAGES_DIMENSION_H,
		.w = enter_name_surface->w,
		.h = enter_name_surface->h
	};

	SDL_FreeSurface(enter_name_surface);

	// user input ouput elements
	SDL_Surface* player_input_surface = NULL;
	SDL_Texture* player_input_texture = NULL;
	SDL_Rect player_input_rect = {.y = enter_name_rect.y + enter_name_rect.h + 10};

	SDL_Surface* player_score_surface = NULL;
	SDL_Texture* player_score_texture = NULL;
	SDL_Rect player_score_rect = {
		.y = 10,
		.x = 10
	};

	do
	{
		SDL_RenderClear(window_data.renderer);

		// add current score and try in string
		sprintf(current_state,"SCORE: %d - TENTATIVES: %d",*(player_score),*(count_of_try));

		// create elements to print the user input
		player_score_surface = TTF_RenderText_Solid(window_data.font,current_state,window_data.window_parameters.text_color);

		if(player_score_surface == NULL)
		{
			end_well = false;

			break;
		}

		player_score_texture = SDL_CreateTextureFromSurface(window_data.renderer,player_score_surface);

		if(player_score_texture == NULL)
		{
			end_well = false;

			break;
		}

		player_score_rect.w = player_score_surface->w;
		player_score_rect.h = player_score_surface->h;

		SDL_FreeSurface(player_score_surface);

		player_score_surface = NULL;

		// print player input if there is 1 char min
		if(strlen(player_input) > 0)
		{
			// create elements to print the user input
			player_input_surface = TTF_RenderText_Solid(window_data.font,player_input,window_data.window_parameters.text_color);

			if(player_input_surface == NULL)
			{
				end_well = false;

				break;
			}

			player_input_texture = SDL_CreateTextureFromSurface(window_data.renderer,player_input_surface);

			if(player_input_texture == NULL)
			{
				end_well = false;

				break;
			}

			player_input_rect.x = (window_data.window_parameters.width - player_input_surface->w) / 2;
			player_input_rect.w = player_input_surface->w;
			player_input_rect.h = player_input_surface->h;

			SDL_FreeSurface(player_input_surface);

			player_input_surface = NULL;

			SDL_RenderCopy(window_data.renderer,player_input_texture,NULL,&player_input_rect);
		}

		// print elements
		SDL_RenderCopy(window_data.renderer,image_texture,NULL,&image_rect);
		SDL_RenderCopy(window_data.renderer,enter_name_texture,NULL,&enter_name_rect);
		SDL_RenderCopy(window_data.renderer,player_score_texture,NULL,&player_score_rect);

		SDL_RenderPresent(window_data.renderer);

		SDL_WaitEvent(&window_data.event);

		// manage events
		switch(window_data.event.type)
		{
			case SDL_QUIT:
				end_well = false;
				end_choice = true;
			; break;

			case SDL_KEYDOWN:
				switch(window_data.event.key.keysym.sym)
				{
					case SDLK_RETURN:
						// check if response is godd when user want to validate
						if(strcasecmp(player_input,response) == 0)
						{
							// increase player score
							(*player_score)++;

							end_choice = true;
						}
						else if(strcmp(player_input,"change_lang!") == 0)
						{
							memset(player_input,0,sizeof(char) * strlen(player_input) );

							// player want to change lang

							if(!make_lang_choice(window_data,datas,current_lang) )
							{
								end_well = false;

								end_choice = true;
							}

							SET_RESPONSE

							(*count_of_try)--;
						}
						else if(strcmp(player_input,"end_game!") == 0)
						{
							end_well = false;

							end_choice = true;

							(*count_of_try)--;
						}

						(*count_of_try)++;
					; break;

					case SDLK_BACKSPACE:
						// remove the last char when player want to delete
						if(*(player_input) != '\0')
							memset(player_input + strlen(player_input) - 1,0,sizeof(char) );
					; break;
				}
			; break;

			case SDL_TEXTINPUT:
				// add the char to text when player want to add
				strcat(player_input,window_data.event.text.text);
			; break;
		}
	}while(!end_choice);

	// free resources
	if(player_input_surface != NULL)
		SDL_FreeSurface(player_input_surface);

	if(player_score_surface != NULL)
		SDL_FreeSurface(player_input_surface);

	if(player_input_texture != NULL)
		SDL_DestroyTexture(player_input_texture);

	if(player_score_texture != NULL)
		SDL_DestroyTexture(player_input_texture);

	SDL_DestroyTexture(image_texture);
	SDL_DestroyTexture(enter_name_texture);

	return end_well;
}

// project window manager
void manage_window(WindowData window_data,void* csv_data)
{
	DataGetterReturn* datas = (DataGetterReturn*) csv_data;

	bool end_game = false;

	int current_lang = 0;
	int player_score = 0;
	int count_of_try = 0;

	if(!make_lang_choice(window_data,datas,&current_lang) )
		return;

	// reset seed to allow to use rand
	srand(time(NULL) );

	while(!end_game)
	{
		SDL_WaitEvent(&window_data.event);

		switch(window_data.event.type)
		{
			case SDL_QUIT:
				end_game = true;
			; break;
		}

		SDL_RenderClear(window_data.renderer);

		if(!ask_player(window_data,datas,&current_lang,&player_score,&count_of_try) )
			end_game = true;
		
		SDL_RenderPresent(window_data.renderer);
	}
}

