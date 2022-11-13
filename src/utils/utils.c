#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../includes/utils/util.h"

#define LINE_MAX_LENGTH 500

#define END_EXEC_IF(cond) if(cond)\
	{\
		return_data->error = ALLOCATION_FAILED;\
		free_data_getter_return(return_data);\
		fclose(csv_file);\
		return false;\
	}

#define END_EXEC_WTASK_IF(cond,task) if(cond)\
	{\
		task\
	}\
	END_EXEC_IF(cond)

#define FREE_VALUES free(return_data->datas[return_data->count_of_datas]->img_path);\
	free(return_data->datas[return_data->count_of_datas]);

bool set_datas_from_csv(DataGetterReturn* return_data,char* csv_file_path)
{
	// struct default values
	return_data->datas = NULL;
	return_data->langs = NULL;
	return_data->count_of_langs = 0;
	return_data->count_of_datas = 0;
	return_data->error = NO_ERROR;

	// open the csv file
	FILE* csv_file = fopen(csv_file_path,"r");

	if(csv_file == NULL)
	{
		return_data->error = FAILED_TO_OPEN_FILE;

		return false;
	}

	return_data->langs = malloc(sizeof(char**) );
	return_data->datas = malloc(sizeof(Data**) );

	if
	(
		return_data->langs == NULL ||
		return_data->datas == NULL
	)
	{
		if(return_data->langs != NULL)
			free(return_data->langs);
		else
			free(return_data->datas);

		return_data->error = ALLOCATION_FAILED;

		fclose(csv_file);

		return false;
	}

	char line_container[LINE_MAX_LENGTH];
	char delimiters[] = ",";
	char* line_part;
	char** realloc_addr;

	Data** data_realloc_addr;

	fgets(line_container,LINE_MAX_LENGTH,csv_file);

	// remove line break
	line_container[strcspn(line_container,"\n\r")] = '\0';

	// remove img col
	strtok(line_container,delimiters);

	// get langs and fill return struct
	while((line_part = strtok(NULL,delimiters) ) != NULL)
	{
		realloc_addr = realloc(return_data->langs,sizeof(char**) * (return_data->count_of_langs + 1) );

		if(realloc_addr != NULL)
		{
			return_data->langs = realloc_addr;

			return_data->langs[return_data->count_of_langs] = malloc(sizeof(char) * (strlen(line_part) + 1) );
		}

		END_EXEC_IF(
			realloc_addr == NULL ||
			return_data->langs[return_data->count_of_langs] == NULL
		)

		strcpy(return_data->langs[return_data->count_of_langs],line_part);

		return_data->count_of_langs++;
	}

	// fill struct by parsing file
	while(!feof(csv_file) )
	{
		// try add one more data to the array
		data_realloc_addr = realloc(return_data->datas,sizeof(Data**) * (return_data->count_of_datas + 1) );

		if(data_realloc_addr != NULL)
		{
			return_data->datas = data_realloc_addr;

			return_data->datas[return_data->count_of_datas] = malloc(sizeof(Data) );
		}
			
		END_EXEC_IF(
			data_realloc_addr == NULL ||
			return_data->datas[return_data->count_of_datas] == NULL
		)

		fgets(line_container,LINE_MAX_LENGTH,csv_file);

		// remove line break
		line_container[strcspn(line_container,"\n\r")] = '\0';

		line_part = strtok(line_container,delimiters);

		// get and set in struct the image path

		return_data->datas[return_data->count_of_datas]->img_path = malloc(sizeof(char) * (strlen(line_part) + 1) );
		return_data->datas[return_data->count_of_datas]->langs_values = malloc(sizeof(Dictionary**) * return_data->count_of_langs);

		END_EXEC_IF(return_data->datas[return_data->count_of_datas]->img_path == NULL)

		// free the image and the container which are allocated
		END_EXEC_WTASK_IF(
			return_data->datas[return_data->count_of_datas]->langs_values == NULL,
			FREE_VALUES
		)

		strcpy(return_data->datas[return_data->count_of_datas]->img_path,line_part);

		// try to fill the langs values
		for(int i = 0; i < return_data->count_of_langs; i++)
		{
			line_part = strtok(NULL,delimiters);

			// allocate Dictionnary struct and his content
			return_data->datas[return_data->count_of_datas]->langs_values[i] = malloc(sizeof(Dictionary) );

			if(return_data->datas[return_data->count_of_datas]->langs_values[i] != NULL)
			{
				return_data->datas[return_data->count_of_datas]->langs_values[i]->value = malloc(sizeof(char*) * (strlen(line_part) + 1) );
			}

			END_EXEC_WTASK_IF(
				return_data->datas[return_data->count_of_datas]->langs_values[i] == NULL ||
				return_data->datas[return_data->count_of_datas]->langs_values[i]->value == NULL,
				FREE_VALUES
			)

			return_data->datas[return_data->count_of_datas]->langs_values[i]->lang = return_data->langs[i];
			strcpy(return_data->datas[return_data->count_of_datas]->langs_values[i]->value,line_part);
		}

		return_data->count_of_datas++;
	}

	fclose(csv_file);

	return true;
}

void free_data_getter_return(DataGetterReturn* return_data)
{
	// free langs
	for(int i = 0; i < return_data->count_of_langs; i++)
		free(return_data->langs[i]);
	
	// free datas content
	for(int i = 0; i < return_data->count_of_datas; i++)
	{
		free(return_data->datas[i]->img_path);

		for(int i_ = 0; i_ < return_data->count_of_langs; i_++)
		{
			free(return_data->datas[i]->langs_values[i_]->value);
			free(return_data->datas[i]->langs_values[i_]);
		}

		free(return_data->datas[i]->langs_values);
		free(return_data->datas[i]);
	}

	return_data->count_of_langs = 0;
	return_data->count_of_datas = 0;

	// free containers
	free(return_data->langs);
	free(return_data->datas);
}