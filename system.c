#include "search_index.h"

/*
 * Initializes the main system structure.
 *
 * Input:
 *  out - output stream used for writing command results
 *
 * Returns:
 *  Pointer to the initialized System structure.
 *
 * Description:
 *  Allocates the system, creates the file list and the root of the Trie,
 *  and stores the output stream used by the commands.
 */
System *createSystem(FILE *out) {
	System *sys = malloc(sizeof(System));
	if (!sys) {
		fprintf(stderr, "Error: memory allocation failed for system.\n");
		return NULL;
	}

	// initialize main components: file list and trie
	sys->files = createFileList();
	sys->trie = createTrieNode();

	// store output stream used for command results
	sys->output = out;
	return sys;
}

/*
 * Adds a new file to the system.
 *
 * Input:
 *  sys     - system structure
 *  id      - file identifier
 *  score   - file relevance score
 *  kwCount - number of keywords
 *  words   - array of keywords
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Creates a file entry and inserts it into the file list.
 *  Each keyword is stored in the file and inserted in the Trie,
 *  where a reference to the file is maintained.
 */
void ADD(System *sys, char *id, int score, int kwCount, char **words) {
	// check if a file with the same id already exists
	if (findFile(sys->files, id) != NULL) {
		fprintf(sys->output, "EXISTS\n");
		return;
	}

	// create file structure and insert it into file list
	File *file = addFileToList(sys->files, id, score);

	// add each keyword to both:
	// 1. the file's keyword list
	// 2. the trie
	for (int i = 0; i < kwCount; i++) {
		if (words[i] != NULL) {
			if (!keywordExists(file->keywords, words[i])) {
				addKeyword(file->keywords, words[i]);
				insertWord(sys->trie, words[i], file);
			}
		}
	}
	fprintf(sys->output, "OK\n");
}

/*
 * Removes a file from the system.
 *
 * Input:
 *  sys - system structure
 *  id  - identifier of the file to delete
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Deletes the file from the file list and removes all its
 *  references from the Trie entries corresponding to its keywords.
 */
void DEL(System *sys, char *id) {
	// search file by id
	File *file = findFile(sys->files, id);
	if (file == NULL) {
		fprintf(sys->output, "NOT FOUND\n");
		return;
	}

	// remove file from all trie entries corresponding to its keywords
	KeywordNode *cur = file->keywords->sentinel->next;
	while (cur != file->keywords->sentinel) {
		deleteFromTrie(sys->trie, cur->word, 0, file);
		cur = cur->next;
	}

	// remove file from file list (also frees its internal structures)
	deleteFileFromList(file);
	fprintf(sys->output, "OK\n");
}

/*
 * Adds a keyword to an existing file.
 *
 * Input:
 *  sys  - system structure
 *  id   - file identifier
 *  word - keyword to add
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  If the file exists and the keyword is not already associated with it,
 *  the keyword is added to the file and inserted into the Trie.
 */
void ADDKW(System *sys, char *id, char *word) {
	File *file = findFile(sys->files, id);

	// file must exist
	if (file == NULL) {
		fprintf(sys->output, "NOT FOUND\n");
		return;
	}

	// add keyword only if it doesn't already exist in the file's keyword list
	// also insert it in the trie:
	// - create new nodes if needed
	// - mark the terminal node and add a reference to this file
	if (!keywordExists(file->keywords, word)) {
		addKeyword(file->keywords, word);
		insertWord(sys->trie, word, file);
	}
	fprintf(sys->output, "OK\n");
}

/*
 * Removes a keyword from a file.
 *
 * Input:
 *  sys  - system structure
 *  id   - file identifier
 *  word - keyword to remove
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Deletes the keyword from the file and removes the corresponding
 *  reference from the Trie structure.
 */
void DELKW(System *sys, char *id, char *word) {
	File *file = findFile(sys->files, id);

	// file must exist
	if (file == NULL) {
		fprintf(sys->output, "NOT FOUND\n");
		return;
	}

	// remove keyword only if it exists in the file's keyword list
	// also update the trie:
	// - remove the reference to this file
	// - if nodes become useless (no children or not terminal), they are deleted
	if (keywordExists(file->keywords, word)) {
		removeKeyword(file->keywords, word);
		deleteFromTrie(sys->trie, word, 0, file);
	}
	fprintf(sys->output, "OK\n");
}

/*
 * Finds all files containing a given keyword.
 *
 * Input:
 *  sys  - system structure
 *  word - keyword to search
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Locates the keyword in the Trie and prints all associated
 *  file identifiers sorted lexicographically.
 */
void FIND(System *sys, char *word) {
	// returns the terminal node in the tree
	TrieNode *node = searchWord(sys->trie, word);

	// no results if word not found or has no associated files
	if (node == NULL || node->fileCount == 0) {
		fprintf(sys->output, "EMPTY\n");
		return;
	}

	// sort files by their id (lexicographically)
	qsort(node->files, node->fileCount, sizeof(File *), compareFiles);

	// print number of files followed by their ids
	fprintf(sys->output, "%d", node->fileCount);
	for (int i = 0; i < node->fileCount; i++) {
		fprintf(sys->output, " %s", node->files[i]->id);
	}
	fprintf(sys->output, "\n");
}

/*
 * Returns the top k files for a keyword based on score.
 *
 * Input:
 *  sys  - system structure
 *  word - keyword to search
 *  k    - number of results required
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Builds a max-heap from all files associated with the keyword
 *  and extracts the k highest scored files.
 */
void TOPK(System *sys, char *word, int k) {
	TrieNode *node = searchWord(sys->trie, word);

	if (node == NULL || node->fileCount == 0) {
		fprintf(sys->output, "EMPTY\n");
		return;
	}

	// build a max-heap from all files containing the word
	Heap *h = createHeap(node->fileCount);
	if (!h) {
		fprintf(stderr, "Error: heap allocation failed.\n");
		return;
	}

	for (int i = 0; i < node->fileCount; i++) {
		heapInsert(h, node->files[i]);
	}

	// extract top k elements (or all if fewer exist)
	int limit = (k < h->size) ? k : h->size;

	fprintf(sys->output, "%d", limit);
	for (int i = 0; i < limit; i++) {
		File *f = heapExtractMax(h);
		fprintf(sys->output, " %s", f->id);
	}
	fprintf(sys->output, "\n");
	free(h->elem);
	free(h);
}

/*
 * Prints all keywords stored in the Trie.
 *
 * Input:
 *  sys - system structure
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Traverses the Trie in lexicographic order and prints each keyword
 *  together with the associated files.
 */
void PRINT(System *sys) {
	// empty trie means no keywords stored
	if (sys->trie->childCount == 0) {
		fprintf(sys->output, "EMPTY\n");
		return;
	}

	// buffer used to build words during traversal
	int cap = INIT_CAPACITY;
	char *buf = malloc(cap);
	if (!buf) {
		fprintf(stderr, "Error: buffer allocation failed.\n");
		return;
	}

	// traverse trie and print all words
	traverseAndPrint(sys->trie, &buf, 0, &cap, sys->output);
	free(buf);
}

/*
 * Finds all files containing keywords with a given prefix.
 *
 * Input:
 *  sys    - system structure
 *  prefix - keyword prefix
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Navigates the Trie to the node representing the prefix and
 *  collects all files from the corresponding subtree.
 */
void PREFIX(System *sys, char *prefix) {
	TrieNode *cur = sys->trie;

	// navigate trie according to prefix
	for (int i = 0; prefix[i] != '\0'; i++) {
		cur = findChild(cur, prefix[i]);
		// if any of the letter from the prefix doesn't exist,
		// no words with this prefix exist
		if (cur == NULL) {
			fprintf(sys->output, "EMPTY\n");
			return;
		}
	}

	// collect all files from subtree rooted at prefix node
	File **result = NULL;
	int size = 0;
	int cap = 0;

	collectFilesFromSubtree(cur, &result, &size, &cap);

	// no files found
	if (size == 0) {
		fprintf(sys->output, "EMPTY\n");
		free(result);
		return;
	}

	// sort files based on their ids
	qsort(result, size, sizeof(File *), compareFiles);

	// print number of files and their ids
	fprintf(sys->output, "%d", size);
	for (int i = 0; i < size; i++) {
		fprintf(sys->output, " %s", result[i]->id);
	}
	fprintf(sys->output, "\n");

	free(result);
}

/*
 * Releases all resources used by the system.
 *
 * Input:
 *  sys - system structure
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Frees all files, the file list, the Trie structure,
 *  and finally the system object itself.
 */
void freeSystem(System *sys) {
	// iterate through all files and delete them from the file list
	File *cur = sys->files->sentinel->next;
	while (cur != sys->files->sentinel) {
		File *next = cur->next;
		deleteFileFromList(cur);
		cur = next;
	}

	// free file list structure
	free(sys->files->sentinel);
	free(sys->files);

	// free trie structure
	freeTrie(sys->trie);

	// free system itself
	free(sys);
}
