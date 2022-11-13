#if !defined(UTIL_H)
#define UTIL_H
	
	#include <stdbool.h>
	
	typedef enum{
		FAILED_TO_OPEN_FILE,
		ALLOCATION_FAILED,
		NO_ERROR
	}UtilError;

	typedef struct
	{
		char* lang;
		char* value;
	}Dictionary;

	typedef struct
	{
		char* img_path;
		Dictionary** langs_values;
	}Data;

	typedef struct
	{
		Data** datas;
		char** langs;
		int count_of_langs;
		int count_of_datas;
		UtilError error;
	}DataGetterReturn;

	// try to fill the given struct with datas from the csv file and return if success
	bool set_datas_from_csv(DataGetterReturn* return_data,char* csv_file_path);

	// free all datas in struct
	void free_data_getter_return(DataGetterReturn* return_data);
#endif