#include "search_index.h"

/*
 * Creates an empty Trie node.
 *
 * Input:
 *  none
 *
 * Returns:
 *  Pointer to a zero-initialized TrieNode.
 *
 * Description:
 *  Allocates memory for a Trie node and initializes all fields to 0/NULL.
 */
TrieNode *createTrieNode() {
	TrieNode *node = calloc(1, sizeof(TrieNode));
	if (!node) {
		fprintf(stderr, "Error: memory allocation failed for TrieNode.\n");
		return NULL;
	}

	return node;
}

/*
 * Finds a child node with a specific character.
 *
 * Input:
 *  node - parent Trie node
 *  c    - character to search for
 *
 * Returns:
 *  Pointer to the child node if found, otherwise NULL.
 *
 * Description:
 *  Searches the children array for a node whose letter matches c.
 */
TrieNode *findChild(TrieNode *node, char c) {
	for (int i = 0; i < node->childCount; i++) {
		if (node->children[i]->letter == c) {
			return node->children[i];
		}
	}
	return NULL;
}

/*
 * Comparison function for sorting Trie children.
 *
 * Input:
 *  a, b - pointers to TrieNode pointers
 *
 * Returns:
 *  Negative, zero, or positive value based on lexicographic order.
 *
 * Description:
 *  Used by qsort to maintain children sorted by their letter.
 */
int compareTrieChildren(const void *a, const void *b) {
	TrieNode *n1 = *(TrieNode **)a;
	TrieNode *n2 = *(TrieNode **)b;
	return (n1->letter - n2->letter);
}

/*
 * Adds a child node for a character.
 *
 * Input:
 *  node - parent Trie node
 *  c    - character to insert
 *
 * Returns:
 *  Pointer to the existing or newly created child node.
 *
 * Description:
 *  If the child already exists it is returned. Otherwise a new node is
 *  created, added to the children array, and the array is kept sorted.
 */
TrieNode *addChild(TrieNode *node, char c) {
	// check if child already exists
	TrieNode *child = findChild(node, c);
	if (child != NULL) {
		return child;
	}

	// resize children array if needed
	if (node->childCount == node->capacity) {
		node->capacity = (node->capacity == 0) ? 2 : node->capacity * 2;

		TrieNode **tmp = realloc(node->children, node->capacity * sizeof(TrieNode *));
		if (!tmp) {
			fprintf(stderr, "Error: memory allocation failed for Trie children.\n");
			return NULL;
		}

		node->children = tmp;
	}

	// create and insert new child
	child = createTrieNode();
	child->letter = c;
	node->children[node->childCount++] = child;

	// keep children sorted (for lexicographic traversal)
	qsort(node->children, node->childCount, sizeof(TrieNode *), compareTrieChildren);
	
	return child;
}

/*
 * Adds a file reference to a terminal Trie node.
 *
 * Input:
 *  node - Trie node representing a word
 *  file - file to associate
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Stores the file pointer in the node's file list if it is not already present.
 */
void addFileReferenceToNode(TrieNode *node, File *file) {
	// avoid duplicate references
	for (int i = 0; i < node->fileCount; i++) {
		if (node->files[i] == file) {
			return;
		}
	}

	// resize array if needed
	if (node->fileCount == node->fileCap) {
		node->fileCap = (node->fileCap == 0) ? 2 : node->fileCap * 2;

		File **tmp = realloc(node->files, node->fileCap * sizeof(File *));
		if (!tmp) {
			fprintf(stderr, "Error: memory allocation failed for file references.\n");
			return;
		}

		node->files = tmp;
	}

	node->files[node->fileCount++] = file;
}

/*
 * Inserts a word into the Trie.
 *
 * Input:
 *  root - root of the Trie
 *  word - keyword to insert
 *  file - file associated with the word
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Traverses or creates nodes for each character and stores the file
 *  reference in the terminal node.
 */
void insertWord(TrieNode *root, char *word, File *file) {
	TrieNode *cur = root;

	// traverse or create nodes for each character
	for (int i = 0; word[i] != '\0'; i++) {
		cur = addChild(cur, word[i]);
		if (!cur) {
			return;
		}
	}

	// mark end of word and attach file reference
	cur->terminal = 1;
	addFileReferenceToNode(cur, file);
}

/*
 * Searches for a word in the Trie.
 *
 * Input:
 *  root - root of the Trie
 *  word - keyword to search
 *
 * Returns:
 *  Pointer to the terminal node if the word exists, otherwise NULL.
 *
 * Description:
 *  Traverses the Trie character by character and checks if the final
 *  node marks a complete word.
 */
TrieNode *searchWord(TrieNode *root, char *word) {
	TrieNode *cur = root;
	for (int i = 0; word[i] != '\0'; i++) {
		cur = findChild(cur, word[i]);
		if (cur == NULL) {
			return NULL;
		}
	}
	return cur->terminal ? cur : NULL;
}

/*
 * Removes a file reference from a Trie node.
 *
 * Input:
 *  node - Trie node containing file references
 *  file - file to remove
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Removes the file pointer from the node's file array and shifts
 *  remaining elements.
 */
void removeFileReference(TrieNode *node, File *file) {
	int pos = -1;

	// find position of file
	for (int i = 0; i < node->fileCount; i++) {
		if (node->files[i] == file) {
			pos = i;
			break;
		}
	}

	// if didn't find the file
	if (pos == -1) {
		return;
	}

	// shift elements to fill the gap
	for (int i = pos; i < node->fileCount - 1; i++) {
		node->files[i] = node->files[i + 1];
	}

	node->fileCount--;
}

/*
 * Removes a file association for a word from the Trie.
 *
 * Input:
 *  node  - current Trie node
 *  word  - keyword to process
 *  depth - current position in the word
 *  file  - file to remove
 *
 * Returns:
 *  1 if the node became useless and should be deleted, otherwise 0.
 *
 * Description:
 *  Recursively removes file references and deletes nodes that are
 *  no longer needed.
 */
int deleteFromTrie(TrieNode *node, char *word, int depth, File *file) {
	if (node == NULL) {
		return 0;
	}

	// if reached end of the word (terminal node)
	if (word[depth] == '\0') {
		// remove the reference to the given file
		removeFileReference(node, file);

		// if no files remain associated, this node is no longer terminal
		if (node->fileCount == 0) {
			node->terminal = 0;
		}
	} else {
		// find the child corresponding to current character of the word
		TrieNode *child = findChild(node, word[depth]);

		// recurse into subtree
		// if child becomes useless, it should be deleted
		if (child != NULL && deleteFromTrie(child, word, depth + 1, file)) {

			// locate child index in children array
			int childIdx = -1;
			for (int i = 0; i < node->childCount; i++) {
				if (node->children[i] == child) {
					childIdx = i;
					break;
				}
			}

			// only if child index was found
			if (childIdx != -1) {
				// free memory associated with child
				free(child->files);
				free(child->children);
				free(child);

				// remove child from children array (shift left)
				for (int j = childIdx; j < node->childCount - 1; j++) {
					node->children[j] = node->children[j + 1];
				}
				node->childCount--;
			}
		}
	}

	// return 1 if current node became useless:
	// it is not terminal and has no children
	return (!node->terminal && node->childCount == 0);
}

/*
 * Traverses the Trie and prints stored words.
 *
 * Input:
 *  node   - current Trie node
 *  buffer - word buffer used during traversal
 *  depth  - current depth in the word
 *  cap    - buffer capacity
 *  out    - output stream
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Performs a lexicographic traversal of the Trie and prints each
 *  word together with its associated files.
 */
void traverseAndPrint(TrieNode *node, char **buffer, int depth, int *cap, FILE *out) {
	if (node == NULL) {
		return;
	}

	// if node marks a word, print the word, as well as the number of files
	// associated to it and all corresponding files sorted by their ids
	if (node->terminal) {
		(*buffer)[depth] = '\0';	// mark end of word
		fprintf(out, "%s %d", *buffer, node->fileCount);
	
		// sort files lexicographically before printing
		qsort(node->files, node->fileCount, sizeof(File *), compareFiles);
		for (int i = 0; i < node->fileCount; i++) {
			fprintf(out, " %s", node->files[i]->id);
		}
		fprintf(out, "\n");
	}

	// recursively visit children (in sorted order)
	for (int i = 0; i < node->childCount; i++) {
		// resize buffer if needed
		if (depth + 1 >= *cap) {
			*cap *= 2;

			char *tmp = realloc(*buffer, *cap);
			if (!tmp) {
				fprintf(stderr, "Error: memory allocation failed during buffer resize.\n");
				return;
			}

			*buffer = tmp;
		}

		// append character and continue traversal
		(*buffer)[depth] = node->children[i]->letter;
		traverseAndPrint(node->children[i], buffer, depth + 1, cap, out);
	}
}

/*
 * Collects all unique files from a Trie subtree.
 *
 * Input:
 *  node   - subtree root
 *  result - dynamic array of file pointers
 *  size   - current number of collected files
 *  cap    - capacity of the result array
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Traverses the subtree and gathers unique file references from
 *  terminal nodes.
 */
void collectFilesFromSubtree(TrieNode *node, File ***result, int *size, int *cap) {
	if (node == NULL) {
		return;
	}

	// if node is terminal, add its files
	if (node->terminal) {
		for (int i = 0; i < node->fileCount; i++) {
			// avoid duplicates in result
			if (!fileAlreadyAdded(*result, *size, node->files[i])) {
				// resize result array if needed
				if (*size == *cap) {
					*cap = (*cap == 0) ? 2 : (*cap) * 2;

					File **tmp = realloc(*result, (*cap) * sizeof(File *));
					if (!tmp) {
						fprintf(stderr, "Error: memory allocation failed for result array.\n");
						return;
					}

					*result = tmp;
				}

				(*result)[(*size)++] = node->files[i];
			}
		}
	}

	// recurse into all children
	for (int i = 0; i < node->childCount; i++) {
		collectFilesFromSubtree(node->children[i], result, size, cap);
	}
}

/*
 * Frees the entire Trie structure.
 *
 * Input:
 *  node - root of the Trie
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Recursively releases all nodes and associated memory.
 */
void freeTrie(TrieNode *node) {
	if (node == NULL) {
		return;
	}
	for (int i = 0; i < node->childCount; i++) {
		freeTrie(node->children[i]);
	}
	free(node->children);
	free(node->files);
	free(node);
}