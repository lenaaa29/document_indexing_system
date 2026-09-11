#include "search_index.h"

/*
 * Creates an empty doubly linked list of files.
 *
 * Input:
 *  none
 *
 * Returns:
 *  Pointer to the initialized FileList.
 *
 * Description:
 *  Allocates the list structure and a sentinel node used to simplify
 *  insertion and deletion operations.
 */
FileList *createFileList() {
	FileList *list = malloc(sizeof(FileList));
	if (!list) {
		fprintf(stderr, "Error: memory allocation failed for FileList.\n");
		return NULL;
	}

	// create sentinel node (does not store valid data)
	list->sentinel = malloc(sizeof(File));
	if (!list->sentinel) {
		fprintf(stderr, "Error: memory allocation failed for sentinel node.\n");
		free(list);
		return NULL;
	}

	list->sentinel->id = NULL;
	list->sentinel->keywords = NULL;
	// circular links (empty list points to itself)
	list->sentinel->next = list->sentinel;
	list->sentinel->prev = list->sentinel;
	return list;
}

/*
 * Creates a new file structure.
 *
 * Input:
 *  id    - file identifier
 *  score - file relevance score
 *
 * Returns:
 *  Pointer to the created File.
 *
 * Description:
 *  Allocates a file object, copies the id string, and initializes
 *  its keyword list.
 */
File *createFile(char *id, int score) {
	File *file = malloc(sizeof(File));
	if (!file) {
		fprintf(stderr, "Error: memory allocation failed for File.\n");
		return NULL;
	}

	file->id = duplicateString(id);		// strdup(id);
	if (!file->id) {
		fprintf(stderr, "Error: memory allocation failed for file id.\n");
		free(file);
		return NULL;
	}

	file->score = score;
	file->keywords = createKeywordList(); // initialize keyword list for this file
	if (!file->keywords) {
		fprintf(stderr, "Error: failed to create keyword list.\n");
		free(file->id);
		free(file);
		return NULL;
	}

	file->prev = NULL;
	file->next = NULL;
	return file;
}

/*
 * Inserts a file into the file list.
 *
 * Input:
 *  list  - file list
 *  id    - file identifier
 *  score - file score
 *
 * Returns:
 *  Pointer to the inserted File.
 *
 * Description:
 *  Creates a new file and inserts it at the beginning of the list.
 */
File *addFileToList(FileList *list, char *id, int score) {
	File *file = createFile(id, score);

	file->next = list->sentinel->next;
	file->prev = list->sentinel;
	list->sentinel->next->prev = file;
	list->sentinel->next = file;
	return file;
}

/*
 * Searches for a file by id.
 *
 * Input:
 *  list - file list
 *  id   - file identifier
 *
 * Returns:
 *  Pointer to the file if found, otherwise NULL.
 *
 * Description:
 *  Traverses the list until a matching id is found.
 */
File *findFile(FileList *list, char *id) {
	File *cur = list->sentinel->next;

	// traverse until sentinel is reached again
	while (cur != list->sentinel) {
		if (strcmp(cur->id, id) == 0) {
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}

/*
 * Removes a file from the list and frees its memory.
 *
 * Input:
 *  file - file to delete
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Unlinks the file from the list, frees all keyword nodes,
 *  and releases the file structure.
 */
void deleteFileFromList(File *file) {
	// unlink file from list
	file->prev->next = file->next;
	file->next->prev = file->prev;

	// free all keyword nodes
	KeywordNode *cur = file->keywords->sentinel->next;
	while (cur != file->keywords->sentinel) {
		KeywordNode *tmp = cur;
		cur = cur->next;
		free(tmp->word);
		free(tmp);
	}

	// free keyword list structure
	free(file->keywords->sentinel);
	free(file->keywords);
	// free file data
	free(file->id);
	free(file);
}

/*
 * Checks if a file pointer already exists in an array.
 *
 * Input:
 *  arr  - array of File pointers
 *  size - number of elements
 *  f    - file to check
 *
 * Returns:
 *  1 if the file is present, otherwise 0.
 *
 * Description:
 *  Prevents duplicate entries in result arrays.
 */
int fileAlreadyAdded(File **arr, int size, File *f) {
	for (int i = 0; i < size; i++) {
		if (arr[i] == f) {
			return 1;
		}
	}
	return 0;
}

/*
 * Comparison function for sorting files by id.
 *
 * Input:
 *  a, b - pointers to File pointers
 *
 * Returns:
 *  Result of strcmp between the two file ids.
 *
 * Description:
 *  Used by qsort to sort files lexicographically.
 */
int compareFiles(const void *a, const void *b) {
	File *f1 = *(File **)a;
	File *f2 = *(File **)b;
	return strcmp(f1->id, f2->id);
}