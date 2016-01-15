#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "headers/dictionary_search.h"

#include "headers/public_functions.h"

#define UNDEFINED_INDEX -1 /*in letters array means that the letter was not found*/
#define WORD_TERMINATION '-' /* used in get word length to stop the while */
#define SYNONYM_TERMINATION '|'
#define PRINT_ERROR 0
#define IS_LOW_ALPHA_NUMERIC(c) (((c) >= LOW_A && (c) <= LOW_Z) || \
	((c) >= '0' && (c) <= '9')) ? 1 : 0

/* declarations */
static int get_word_length(FILE *p_dictionary);

static void next_line(FILE *p_dictionary);

static char char_to_upper(char c);

static int strcmp(char *s, char *t);

static BOOLEAN get_next_word(FILE *p_dictionary, char *buff);

static BOOLEAN cmp_words(char *word, FILE *p_dictionary);

static BOOLEAN get_synonym(FILE *p_dictionary, char *buff);

/* definition */

/*
 * BOOLEAN *find_synonym(char *word, FILE *p_dictionary, long *letters, char *buff)
 *
 * Finds Identical word in the dictionary file.
 * Returns a true if found a synonym(that will be saved to buff) or false
 * if failed.
 *
 * params: char *word - the word that needs to be replaced.
 * 		   FILE *p_dictionary - pointer to the dictionary text file.
 * 		   long *letters - Array that the function set_letters_pos returned.
 * 		   char *buff - the string that the synonym will be saved to.
 *
 */
BOOLEAN find_synonym(char *word, FILE *p_dictionary, long *letters, char *buff)
{
	long char_pos;
	char first_letter = char_to_upper(*word);

	/* set file pointer position to that letter if it exists */
	if((char_pos = *(letters + first_letter - CAP_A)) == -1)
	{
		if(PRINT_ERROR)
		{
			fprintf(stderr, "%c wasn't found\n", first_letter);
			PRINT_FILE_LINE();
		}
		return false;
	}

	fseek(p_dictionary, char_pos, SEEK_SET);
	/* set file pointer to next line and start comparing */
	next_line(p_dictionary);
	if(cmp_words(word, p_dictionary) == false)
	{
		return false;
	}

	return get_synonym(p_dictionary, buff);
}


/*
 * void set_letters_pos(long *letters, FILE *p_dictionary)
 *
 * sets the char index for each letter that is defined
 * in the file dictionary.
 *
 */
void set_letters_pos(long *letters, FILE *p_dictionary)
{
	int i;
	char c;
	int letter_counter = 0;

	/* initialize array elements to UNDEFINED_INDEX */
	for(i = 0; i < LETTERS_IN_ABC; ++i)
	{
		*(letters + i) = UNDEFINED_INDEX;
	}

	for(i = 0; (c = getc(p_dictionary)) != EOF; ++i)
	{
		if(c >= CAP_A && c <= CAP_Z)
		{
			if(*(letters + c - 'A') != -1)
			{
				fprintf(stderr, "File format is invalid: defined same letter twice\n");
				exit(1);
			}
			*(letters + c - 'A') = i;
			letter_counter++;
		}
	}

}


/*
 * BOOLEAN get_synonym(FILE *p_dictionary, char *buff)
 *
 * Given a file pointer that points to the first character
 * of the synonym region
 *
 *
 */
static BOOLEAN get_synonym(FILE *p_dictionary, char *buff)
{
	BOOLEAN flag;
	char s_num_of_synonyms[3];

	/* get number of synonyms this word has */
	flag = get_next_word(p_dictionary, s_num_of_synonyms);
	if(flag == false)
	{
		if (PRINT_ERROR)
		{
			fprintf(stderr, "get_next_word returned false\n");
			PRINT_FILE_LINE();
		}
		return false;
	}

	int num_of_synonyms = 0;

	/* convert string to int */
	int i;
	for(i = 0; *(s_num_of_synonyms + i) != NTS; i++)
	{
		num_of_synonyms *= 10;
		num_of_synonyms += (*(s_num_of_synonyms + i) - '0');
	}
	/* generate random number to pick a synonym */
	srand(time(NULL));
	i = rand() % num_of_synonyms;

	for(; i > 0; i--)
	{
		flag = get_next_word(p_dictionary, buff);
		if(flag == false)
		{
			if (PRINT_ERROR)
			{
				fprintf(stderr, "get_next_word returned false\n");
				PRINT_FILE_LINE();
			}
			return false;
		}

	}
	flag = get_next_word(p_dictionary, buff);
	return flag;
}


/*
 * char *cmp_words(char *word, FILE *p_dictionary)
 *
 * The function compares all the words under a certain
 * letter to the given string word until there is a reason
 * to stop. The function returns (enum) BOOL.true if it found a match
 * or BOOL.false if it failed.
 *
 */
static BOOLEAN cmp_words(char *word, FILE *p_dictionary)
{
	/* initializing variables */
	char cmp_to[WORD_MAX_LENGTH];
	int cmp_diff;

	while(get_next_word(p_dictionary, cmp_to) != false)
	{
		cmp_diff = strcmp(word, cmp_to);
		if(cmp_diff == 0)
		{return true;}
		else if(cmp_diff < 0)
		{break;}
		next_line(p_dictionary);
	}

	return false;
}


/*
 * BOOLEAN get_next_word(FILE *p_dictionary, char *buff)
 *
 * given a file pointer with SEEK_CUR in the beginning
 * of a line, returns true and assigns the word to buff
 * if succeeded in finding the next word or false if get_word_length() failed.
 *
 */
static BOOLEAN get_next_word(FILE *p_dictionary, char *buff)
{
	/* Note that an external buffer is necessary as returning
	 * a pointer created in a local scope is very unsafe because it will
	 * be released as soon as the function is exited.
	 * Furthermore dynamically allocating an array within the function
	 * will force the caller function to free it even though it
	 * didn't create it.
	 */

	int i;
	/* might be better to use realloc instead of finding length */
	int length = get_word_length(p_dictionary);

	/* check if the function failed to find anything */
	if(length == 0)
	{
		return false;
	}

	for(i = 0; i < length; ++i)
	{
		if(i == WORD_MAX_LENGTH)
		{
			if (PRINT_ERROR)
			{
				fprintf(stderr, "word is too long\n");
				PRINT_FILE_LINE();
			}
			return false;
		}
		*(buff+i) = getc(p_dictionary);
	}
	*(buff + i) = NTS;
	getc(p_dictionary);

	return true;
}


/*
 * int get_word_length(FILE *p_dictionary)
 *
 * gets the length of the word from the dictionary file.
 * The function will terminate the counting and return 0
 * if a character other than a lower case letter was found.
 *
 */
static int get_word_length(FILE *p_dictionary)
{
	/* declarations */
	char c;
	int length = 0;

	/* if '-' or '|' is found exit loop */
	while((c=getc(p_dictionary)) != WORD_TERMINATION &&
			c != SYNONYM_TERMINATION)
	{
		/* if c isn't with in range of small letters return 0
		 * only small letters are allowed in the words and synonym
		 * of the dictionary
		 */
		if(!IS_LOW_ALPHA_NUMERIC(c))
		{
			return 0;
		}
		length++;
	}
	/* reset the input stream to the start */
	fseek(p_dictionary, -(length + 1), SEEK_CUR);

	return length;
}


/*
 * void next_line(FILE *p_dictionary)
 *
 * sets the file pointer to the next line or EOF
 *
 */
static void next_line(FILE *p_dictionary)
{
	char c;
	while((c = getc(p_dictionary)) != '\n' && (c != EOF))
		;
}


/*
 * void char_to_upper(char *c)
 *
 * converts what ever c points to, to an upper case version
 *
 */
static char char_to_upper(char c)
{
	return ((c >= CAP_A) && (c <= CAP_Z)) ? c : (c + CAP_A - 'a');
}

/*
 * static int strcmp(char *s, char *t)
 *
 * Compares the strings s and t. Returns negative int if t > s,
 * positive if t < s or 0 if t = s
 *
 */
static int strcmp(char *s, char *t)
{
	for(; *s == *t; s++, t++)
	{
		if(*s == NTS)
		{
			return 0;
		}
	}
	return *s - *t;
}
