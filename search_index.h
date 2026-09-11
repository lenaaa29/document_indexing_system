#ifndef SEARCH_INDEX
#define SEARCH_INDEX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_CAPACITY 10

/* KEYWORD LIST STRUCTURES */
typedef struct KeywordNode {
    char *word;                 // keyword string
    struct KeywordNode *prev;   // pointer to the previous node
    struct KeywordNode *next;   // pointer to the next node
} KeywordNode;

typedef struct KeywordList {
    KeywordNode *sentinel;      // sentinel (dummy) node
} KeywordList;


/* FILE LIST (doubly-linked) STRUCTURES */
typedef struct File {
    char *id;                  // unique file identifier
    int score;                 // file score (used for ranking)
    KeywordList *keywords;     // list of associated keywords
    struct File *prev;         // pointer to previous file
    struct File *next;         // pointer to next file
} File;

typedef struct FileList {
    File *sentinel;            // sentinel (dummy) node
} FileList;


/* TREE STRUCTURES */
typedef struct TrieNode {
    char letter;               // character stored in this node
    int terminal;              // 1 if this node marks end of a word
    struct TrieNode **children;// dynamic array of child nodes
    int childCount;            // number of children
    int capacity;              // capacity of children array

    /* List of files that contain this word */
    File **files;              // array of pointers to files
    int fileCount;             // number of associated files
    int fileCap;               // capacity of file array
} TrieNode;


/* HEAP STRUCTURES */
typedef struct {
    File *file;                // pointer to a file
} HeapItem;

typedef struct {
    HeapItem *elem;            // array of heap elements
    int size;                  // current number of elements
    int capacity;              // maximum capacity
} Heap;


/* SYSTEM STRCUTURE */
typedef struct {
    FileList *files;           // list of all files
    TrieNode *trie;            // multiway trie
    FILE *output;              // output file stream
} System;


/* FUNCTION PROTOTYPES */

/* Utility functions */
char *duplicateString(const char *src);
char *read_line(FILE *f);

/* KeywordList management */
KeywordList *createKeywordList();
void addKeyword(KeywordList *list, char *word);
void removeKeyword(KeywordList *list, char *word);
int keywordExists(KeywordList *list, char *word);

/* FileList management */
FileList *createFileList();
File *createFile(char *id, int score);
File *addFileToList(FileList *list, char *id, int score);
void deleteFileFromList(File *file);
File *findFile(FileList *list, char *id);
int fileAlreadyAdded(File **arr, int size, File *f);
int compareFiles(const void *a, const void *b);

/* Trie management */
TrieNode *createTrieNode();
void insertWord(TrieNode *root, char *word, File *file);
TrieNode *searchWord(TrieNode *root, char *word);
int deleteFromTrie(TrieNode *node, char *word, int depth, File *file);
void freeTrie(TrieNode *node);
TrieNode *findChild(TrieNode *node, char c);
TrieNode *addChild(TrieNode *node, char c);
int compareTrieChildren(const void *a, const void *b);
void addFileReferenceToNode(TrieNode *node, File *file);
void removeFileReference(TrieNode *node, File *file);
void traverseAndPrint(TrieNode *node, char **buffer, int depth, int *cap, FILE *out);
void collectFilesFromSubtree(TrieNode *node, File ***result, int *size, int *cap);

/* Heap management */
Heap *createHeap(int capacity);
void heapInsert(Heap *heap, File *file);
File *heapExtractMax(Heap *heap);
void siftUp(Heap *heap, int idx);
void siftDown(Heap *heap, int idx);
int fileHigherPriority(File *a, File *b);

/* System operations */
System *createSystem(FILE *out);
void ADD(System *sys, char *id, int score, int kwCount, char **words);
void DEL(System *sys, char *id);
void ADDKW(System *sys, char *id, char *word);
void DELKW(System *sys, char *id, char *word);
void FIND(System *sys, char *word);
void TOPK(System *sys, char *word, int k);
void PREFIX(System *sys, char *prefix);
void PRINT(System *sys);
void freeSystem(System *sys);

#endif