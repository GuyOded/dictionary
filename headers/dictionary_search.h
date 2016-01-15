/*
 * dictionary_search.h
 *
 *  Created on: Dec 29, 2015
 *      Author: guy
 */

#ifndef HEADERS_DICTIONARY_SEARCH_H_
#define HEADERS_DICTIONARY_SEARCH_H_

#define NTS '\0'
#define CAP_A 'A'
#define CAP_Z 'Z'
#define LOW_A 'a'
#define LOW_Z 'z'
#define LETTERS_IN_ABC 26
/*#define DICTIONARY_NAME "example.md"*/
#define WORD_MAX_LENGTH 100
#define PRINT_FILE_LINE(void) do{\
	fprintf(stderr, "In %s, Line: %d\n", __FILE__,\
			__LINE__);\
}while(0)

void set_letters_pos(long *letters, FILE *pfile);

#endif /* HEADERS_DICTIONARY_SEARCH_H_ */
