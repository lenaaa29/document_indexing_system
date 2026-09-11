#include "search_index.h"

/*
 * Creates a copy of a string.
 *
 * Input:
 *  src - source string
 *
 * Returns:
 *  Pointer to the duplicated string.
 *
 * Description:
 *  Allocates memory for a new string and copies the content of src.
 */
char *duplicateString(const char *src) {
	if (src == NULL) {
		return NULL;
	}

	// allocate enough memory for the string + null terminator
	char *dst = malloc(strlen(src) + 1);

	// copy content if allocation succeeded
	if (dst != NULL) {
		strcpy(dst, src);
	}
	return dst;
}

/*
 * Reads a full line from a file.
 *
 * Input:
 *  f - input file stream
 *
 * Returns:
 *  Dynamically allocated string containing the line,
 *  or NULL if EOF is reached with no data read.
 *
 * Description:
 *  Reads characters until newline or EOF and resizes
 *  the buffer when necessary.
 */
char *read_line(FILE *f) {
	int cap = INIT_CAPACITY;	// initial capacity 
	int len = 0;				// current length of the line

	char *line = malloc(cap);
	if (!line){
		fprintf(stderr, "Error: memory allocation failed for line in read_line.\n");
		return NULL;
	}
	int c;

	// read characters one by one
	while ((c = fgetc(f)) != EOF && c != '\n') {
		// resize buffer if needed
		if (len + 1 >= cap) {
			cap *= 2;
			char *tmp = realloc(line, cap);
			if (tmp == NULL) {
				free(line);
				return NULL;
			}
			line = tmp;
		}
		// append character to line
		line[len++] = (char)c;
	}

	// if EOF reached and nothing was read, return NULL
	if (c == EOF && len == 0) {
		free(line);
		return NULL;
	}

	// terminate string
	line[len] = '\0';
	return line;
}