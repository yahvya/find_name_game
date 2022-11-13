#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../includes/utils/util.h"
#include "../includes/window/window.h"
#include "../includes/window/project.h"

int main(int argc,char** argv)
{
	DataGetterReturn csv_getter_return_data;

	if(!set_datas_from_csv(&csv_getter_return_data,"../resources/docs/words.csv") )
	{
		printf("La récupération des données a echoué");

		exit(EXIT_FAILURE);
	}

	WindowParameter window_parameters = WINDOW_DEFAULT_PARAMETERS;

	strcpy(window_parameters.title,"Find name");
	strcpy(window_parameters.font_path,"../resources/fonts/font.ttf");

	if
	(
		!init_window(
			window_parameters,
			&csv_getter_return_data,
			manage_window
		) 
	)
		printf("\nEchec de l'ouverture de la fenêtre\n");

	free_data_getter_return(&csv_getter_return_data);

	return 0;
}