/*
 * temp_name.c
 *
 * This file contains all the functions that handle the
 * word replacement within a file.
 *
 */
#include <stdio.h>
#include "headers/dictionary_search.h"
#include "headers/public_functions.h"

/* DECLARATIONS */

static BOOLEAN get_and_search_word(char c, char *word, char *buffer, long *letters,
		FILE *replace_file, FILE *text_file, FILE *p_dictionary);

static BOOLEAN is_letter(char let, char type);

static void cpy_string_to_file(FILE *fp, char *str);

static void str_to_lower(char c[]);

static char to_lower(char c);

static BOOLEAN try_open_files(int argc, char *argv[]);

/* DEFENITIONS */

/**
 * Checks the integrity of the arguments provided and makes sure
 * that all the files can be opened.
 *
 * @param argc - arguments counter
 * @param argv - argument vector
 * @return True if found no errors  or false if stumbled into an error
 * 		   while trying to open files.
 */
static BOOLEAN try_open_files(int argc, char *argv[]) // T: try_open_files ?? seems more like try_open_files_according_to_argc_and_argv_and_close_them_on_failure_print_usage...
{
	if(argc != 3)
	{
		fprintf(stderr, "Wrong number of arguments {%d for 2}\n", argc-1);
		fprintf(stderr, "%s <dictionary file> <text file>\n", argv[0]);
		return false;
	}

	FILE *text_file = fopen(argv[2], "r");
	if(text_file == NULL)
	{
		printf("can't open file: %s\n", argv[1]);
		PRINT_FILE_LINE();
		return false;
	}
	fclose(text_file);

	FILE *p_dictionary = fopen(argv[1], "r");
	if(p_dictionary == NULL)
	{
		printf("can't open file: %s\n", argv[2]);
		PRINT_FILE_LINE();
		return false;
	}
	fclose(p_dictionary);

	FILE *p_replace_file = fopen(TEMP_NAME, "w");
	if(p_replace_file == NULL)
	{
		printf("can't open file: %s\n", TEMP_NAME);
		PRINT_FILE_LINE();
		return false;
	}
	fclose(p_replace_file);

	return true;
}
/*
 * BOOLEAN replace_main(FILE *fp, int replace_interval)
 *
 * Basically the function handles all the file creations,
 * it goes over the received file and replaces the words that
 * have a synonym in a different file.
 *
 */
BOOLEAN replace_main(int argc, char *argv[])
{
	if(!try_open_files(argc, argv))
		return false;

	FILE *text_file = fopen(argv[2], "r");
	FILE *p_dictionary = fopen(argv[1], "r");
	FILE *p_replace_file = fopen(TEMP_NAME, "w+");

	long letters[LETTERS_IN_ABC];
	set_letters_pos(letters, p_dictionary);


	char c, word[WORD_MAX_LENGTH], buffer[WORD_MAX_LENGTH];

	while((c = getc(text_file)) != EOF)
	{
		if(is_letter(c, 'I'))
		{
			get_and_search_word(c, word, buffer, letters, p_replace_file ,
					text_file, p_dictionary);
		}
		else
		{
			fputc(c, p_replace_file );
		}
	}
	/* close the files we opened, we keep the original text_file */
	fclose(p_dictionary);
	fclose(p_replace_file);
	return true;
}


/*
 * BOOLEAN get_and_search_word(c, word, buffer, replace_file,
 * 							  text_file, p_dictionary)
 *
 * Gets the following word from text_file searches it in p_dictionary
 * using find_synonym (saves to buffer). If a synonym was found returns
 * true or false otherwise.
 *
 */
static BOOLEAN get_and_search_word(char c, char *word, char *buffer, long *letters,
		FILE *replace_file, FILE *text_file, FILE *p_dictionary)
{
	int index = 0;
	*(word + index++) = c;
	BOOLEAN result;

	while((c = getc(text_file)) != EOF)
	{
		/* We assume that if c isn't a
		 * letter we reached the end of the word */
		if(!is_letter(c, 'I'))
		{
			*(word + index++) = NTS;
			str_to_lower(word);
			/* basically if a synonym was found replace it or else
			 * keep the original word
			 */
			if(find_synonym(word, p_dictionary, letters, buffer))
			{
				printf("replaced %s--->%s\n", word, buffer);
				cpy_string_to_file(replace_file, buffer);
				result = true;
			}
			else
			{
				cpy_string_to_file(replace_file, word);
				result = false;
			}
			fputc(c, replace_file);
			return result;
		}
		else
		{
			if(index == WORD_MAX_LENGTH - 1)
			{
				*(word + index) = NTS;
				cpy_string_to_file(text_file, word);
				fputc(c, replace_file);
				fprintf(stderr, "Found too long word");
				PRINT_FILE_LINE();
				return false;
			}
			*(word + index++) = c;
		}
	}
	return false;
}


/*
 * static void cpy_string_to_file(FILE *fp, char *str)
 *
 * Simply copies the string str to the file fp.
 *
 */
static void cpy_string_to_file(FILE *fp, char *str)
{
	int i;
	for(i = 0; (i < WORD_MAX_LENGTH) && (*(str + i) != NTS); ++i)
	{
		fputc(*(str + i), fp);
	}
}


/*
 * static BOOLEAN is_letter(char let, char type)
 *
 * Checks if the given char let is a letter of a specific type, that
 * is to say if let is lower case upper case or either.
 * If type=='U' returns true if let is an upper case letter or false
 * otherwise.
 * For type=='L' same thing happen but with lower case.
 * For type=='I' (insensitive) returns true if let is a letter false
 * other wise.
 *
 */
static BOOLEAN is_letter(char let, char type)
{
	switch (type){
	case 'U': case 'u':
		return (let >= 'A' && let <= 'Z') ? true : false;
	case 'L': case 'l':
		return (let >= 'a' && let <= 'z') ? true : false;
	case 'I': case 'i':
		return ((let >= 'a' && let <= 'z') ||
			   (let >= 'A' && let <= 'Z')) ? true : false;
	}
	return false;
}


/*
 * static char conditional_to_lower(char c)
 *
 * given a character c, returns a lower case version of c if it is
 * an upper case letter
 *
 */
static char to_lower(char c)
{
	int diff = CAP_A - LOW_A;
	return (c >= 'A' && c <= 'Z') ? (c - diff) : c;
}


/*
 * static void str_to_lower(char c[])
 *
 * given a char array converts all of its upper case characters to
 * lower case
 *
 */
static void str_to_lower(char c[])
{
	int i;
	for(i = 0; i < WORD_MAX_LENGTH && *(c + i) != NTS; i++)
	{
		*(c + i) = to_lower(*(c + i));
	}
}
