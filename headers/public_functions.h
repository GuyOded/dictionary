/*
 * search_and_replace.h
 *
 *  Created on: Jan 7, 2016
 *      Author: guy
 */

#ifndef HEADERS_PUBLIC_FUNCTIONS_H_
#define HEADERS_PUBLIC_FUNCTIONS_H_

#define TEMP_NAME "temp.txt"

typedef enum{
	false = 0,
	true
} BOOLEAN;

BOOLEAN find_synonym(char *word, FILE *p_dictionary, long *letters, char *buff);

BOOLEAN replace_main(int argc, char *argv[]);

#endif /* HEADERS_PUBLIC_FUNCTIONS_H_ */
