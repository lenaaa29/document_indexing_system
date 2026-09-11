#include "search_index.h"

/*
 * Creates an empty keyword list.
 *
 * Input:
 *  none
 *
 * Returns:
 *  Pointer to the initialized KeywordList.
 *
 * Description:
 *  Allocates the list structure and a sentinel node used to simplify
 *  insertion and deletion operations.
 */
KeywordList *createKeywordList() {
    KeywordList *list = malloc(sizeof(KeywordList));
    if (!list) {
        fprintf(stderr, "Error: memory allocation failed for KeywordList.\n");
        return NULL;
    }

    list->sentinel = malloc(sizeof(KeywordNode));
    if (!list->sentinel) {
        fprintf(stderr, "Error: memory allocation failed for keyword sentinel node.\n");
        free(list);
        return NULL;
    }

    list->sentinel->word = NULL;
    list->sentinel->next = list->sentinel;
    list->sentinel->prev = list->sentinel;

    return list;
}

/*
 * Checks if a keyword exists in a list.
 *
 * Input:
 *  list - keyword list
 *  word - keyword to search
 *
 * Returns:
 *  1 if the keyword exists, otherwise 0.
 *
 * Description:
 *  Traverses the list and compares each stored keyword with the input.
 */
int keywordExists(KeywordList *list, char *word) {
	KeywordNode *cur = list->sentinel->next;

	// traverse list until sentinel
	while (cur != list->sentinel) {
		if (strcmp(cur->word, word) == 0) {
			return 1;
		}
		cur = cur->next;
	}
	return 0;
}

/*
 * Inserts a keyword into the list.
 *
 * Input:
 *  list - keyword list
 *  word - keyword to insert
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Creates a new node containing the keyword and inserts it at the
 *  beginning of the list.
 */
void addKeyword(KeywordList *list, char *word) {
    KeywordNode *node = malloc(sizeof(KeywordNode));
    if (!node) {
        fprintf(stderr, "Error: memory allocation failed for KeywordNode.\n");
        return;
    }

    node->word = duplicateString(word);
    if (!node->word) {
        fprintf(stderr, "Error: memory allocation failed for keyword string.\n");
        free(node);
        return;
    }

	// insert at head (after sentinel)
    node->next = list->sentinel->next;
    node->prev = list->sentinel;

    list->sentinel->next->prev = node;
    list->sentinel->next = node;
}

/*
 * Removes a keyword from the list.
 *
 * Input:
 *  list - keyword list
 *  word - keyword to remove
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Searches for the node containing the keyword, removes it from
 *  the list, and frees its memory.
 */
void removeKeyword(KeywordList *list, char *word) {
    KeywordNode *cur = list->sentinel->next;

	// traverse list to find the node
    while (cur != list->sentinel) {
        if (strcmp(cur->word, word) == 0) {
            cur->prev->next = cur->next;
            cur->next->prev = cur->prev;

            free(cur->word);
            free(cur);
            return;
        }
        cur = cur->next;
    }
}