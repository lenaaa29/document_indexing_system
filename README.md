# Document Indexing System

A C-based document indexing system for storing files, associating them with keywords, and retrieving documents using keyword search, prefix search, and score-based ranking.

The project focuses on efficient data organization and combines linked lists, a multiway trie, and a max-heap to support different types of indexing and retrieval operations.

---

## Overview

The system manages a collection of files, where each file has:

* a unique file identifier
* a score used for ranking
* a collection of associated keywords

The system supports the following workflow:

1. Files are added to the system together with their keywords.
2. Keywords are indexed in a multiway trie.
3. Files can be searched using exact keywords.
4. Prefix queries can retrieve files associated with matching keywords.
5. The `TOPK` operation uses file scores to retrieve the highest-ranked results.
6. Files and keywords can be removed while keeping the index consistent.

The implementation uses shared pointers between data structures, allowing files to be referenced by the trie and heap without creating unnecessary copies.

---

## Features

* Add and manage files
* Associate keywords with files
* Remove files from the index
* Add and remove keywords
* Search files by keyword
* Search files by keyword prefix
* Retrieve the top-k files by score
* Print all indexed keywords
* Lexicographic ordering of keywords and file identifiers
* Dynamic memory management
* Automatic cleanup of unused trie nodes
* Input processing and command execution

---

## Technologies

* **C**
* **Make**
* **Linux / WSL**
* Standard C library
* Dynamic memory allocation

---

## System Design

The system is divided into several logical components:

* **File storage** — circular doubly linked list containing all files
* **Keyword storage** — circular doubly linked lists associated with individual files
* **Keyword indexing** — multiway trie used for exact and prefix searches
* **Ranking** — max-heap used by the `TOPK` operation
* **Command processing** — handles the different operations supported by the system
* **System state** — all major components are grouped inside the main system structure

The use of pointers allows the same `File` objects to be referenced by different structures without duplicating their data.

---

## Data Structures

### File and Keyword Lists

Files and their associated keywords are stored using **circular doubly linked lists with sentinel nodes**.

The file list contains all files currently stored in the system.

Each file contains:

* a file identifier
* a score
* a keyword list
* pointers to the previous and next files

Each keyword list contains `KeywordNode` elements storing:

* the keyword string
* a pointer to the previous node
* a pointer to the next node

The sentinel node represents the beginning and end of each list. When a list is empty, the sentinel points to itself.

This design:

* avoids special cases for insertion and removal
* allows traversal in both directions
* simplifies list manipulation
* allows constant-time pointer updates once the required node is found

The main list operations include:

* `createFile()`
* `addFileToList()`
* `findFile()`
* `deleteFileFromList()`
* `addKeyword()`
* `removeKeyword()`
* `keywordExists()`

---

### Multiway Trie

Keywords are indexed using a **multiway trie**.

Each trie node contains:

* the character represented by the node
* a terminal flag indicating the end of a keyword
* a dynamic array of child nodes
* references to files containing the corresponding keyword

Each path from the root to a terminal node represents a complete keyword.

The trie provides:

* efficient keyword lookup
* efficient prefix searching
* natural representation of strings
* lexicographic traversal

Children are maintained in **lexicographic order**, allowing keywords to be printed alphabetically.

The main trie operations include:

* `insertWord()` — inserts a keyword and associates it with a file
* `searchWord()` — searches for a keyword
* `deleteFromTrie()` — removes a file reference from a keyword
* `traverseAndPrint()` — traverses the trie and prints stored keywords
* `collectFilesFromSubtree()` — collects files associated with keywords sharing a prefix

When a keyword no longer has any associated files, its terminal node is removed. Unused parent nodes are recursively deleted as long as they are not needed by another keyword.

---

### Max-Heap

The `TOPK` operation uses a **max-heap** to retrieve files with the highest scores.

The heap contains:

* a dynamic array of heap elements
* the current heap size
* the current capacity

Each `HeapItem` stores a pointer to a `File`.

Files are ordered according to the following priority:

1. files with a higher score have higher priority
2. if scores are equal, the file with the lexicographically smaller identifier has higher priority

The heap provides:

* `heapInsert()` — inserts a file and restores the heap property
* `heapExtractMax()` — extracts the highest-priority file
* `siftUp()` — restores the heap after insertion
* `siftDown()` — restores the heap after extraction

---

## System Structure

The main system structure contains the core components required by the application:

```text
System
├── FileList *files
├── TrieNode *trie
└── FILE *output
```

The system is responsible for:

* managing all stored files
* maintaining the keyword index
* handling search and prefix operations
* supporting ranked queries
* writing results to the output file

Centralizing the main structures allows the different components of the application to operate on the same system state.

---

## Operations and Complexity

### Add File

**Time complexity: O(L · C)**

A new file is:

1. created and initialized
2. added to the file list
3. associated keywords are inserted into the trie

Where `L` represents the keyword length and `C` represents the number of keywords associated with the file.

The exact complexity also depends on the cost of locating the file and maintaining the trie.

### Delete File

**Time complexity: depends on the number of associated keywords and their lengths**

The operation:

1. locates the file
2. removes all of its keyword references from the trie
3. removes the file from the file list
4. releases its dynamically allocated memory

Unused trie nodes are also removed when necessary.

### Add Keyword

**Time complexity: O(L)** for trie insertion, excluding file lookup

The operation:

1. locates the file
2. checks whether the keyword already exists
3. adds the keyword to the file's keyword list
4. inserts the keyword into the trie

### Delete Keyword

**Time complexity: depends on keyword length**

The operation:

1. locates the file
2. removes the keyword from its keyword list
3. removes the corresponding file reference from the trie
4. removes unused trie nodes when necessary

### Find

**Time complexity: O(L + k)** excluding result sorting

The operation:

1. navigates through the trie to locate the keyword
2. retrieves the associated files
3. sorts the resulting files lexicographically
4. prints the results

Where `L` is the keyword length and `k` is the number of matching files.

### Top K

**Time complexity: O(n log n + k log n)**

The operation:

1. locates the keyword in the trie
2. inserts its associated files into a max-heap
3. repeatedly extracts the highest-priority file
4. stops after `k` files have been retrieved

Where `n` is the number of files associated with the keyword.

### Prefix

**Time complexity: depends on the size of the matching trie subtree**

The operation:

1. navigates to the node representing the prefix
2. traverses the corresponding subtree
3. collects all unique files
4. sorts the resulting files lexicographically
5. prints the results

### Print

**Time complexity: proportional to the number of trie nodes and stored file references**

The trie is traversed in lexicographic order and all stored keywords together with their associated files are printed.

---

## Memory Management

The project uses dynamic memory for:

* file structures
* linked-list nodes
* keyword strings
* trie nodes
* dynamically allocated child arrays
* file-reference arrays
* heap storage

Memory is released during system cleanup.

The cleanup process includes:

* freeing all files from the file list
* freeing keyword lists associated with each file
* freeing the entire trie
* releasing dynamically allocated arrays
* freeing the main system structure

Special care is required when deleting trie nodes because nodes can be shared by multiple keywords through common prefixes.

---

## Build

The project uses a `Makefile` for compilation.

Build the project with:

```bash
make
```

To remove generated build files:

```bash
make clean
```

The exact available Make targets depend on the current `Makefile`.

---

## Run

After compilation, the program can be run using the provided input file:

```bash
./tema1 < tema1.in
```

The resulting output is written according to the output handling implemented by the project.

The repository also contains `tema1.out` as an example output.

---

## Testing

A checking script is included in the project:

```bash
./checker.sh
```

The script can be used to verify the program against the available test configuration.

The test files used during development are not included in the GitHub repository in order to keep the published project focused on the source code and documentation.

---

## Project Structure
├── Makefile
├── README.md
├── checker.sh
├── file.c
├── heap.c
├── keyword.c
├── main.c
├── search_index.h
├── system.c
├── tree.c
├── utils.c
└── tests/

---

## Design Considerations

The main design goal was to select a data structure according to the operation it needs to support.

* **Circular doubly linked lists** provide flexible storage for files and their keywords.
* **Sentinel nodes** simplify insertion and deletion by avoiding special cases.
* **A multiway trie** provides efficient keyword and prefix searching.
* **Lexicographically ordered trie children** allow keywords to be traversed alphabetically.
* **A max-heap** provides efficient access to the highest-ranked files required by `TOPK`.
* **Shared pointers** allow files to be referenced by multiple structures without unnecessary duplication.
* **Dynamic memory allocation** allows the system to handle a variable number of files, keywords, and trie nodes.
* **Recursive trie cleanup** prevents unused nodes from remaining in memory after keywords are removed.

This separation of responsibilities keeps the implementation organized and allows each data structure to handle the operations for which it is best suited.

---

## Limitations and Possible Extensions

The current implementation is designed as a command-line C application and focuses on data structures, indexing, searching, and memory management.

Possible future improvements include:

* faster direct file lookup using a hash table
* more efficient storage of file references
* additional ranking strategies
* persistent storage of the index
* improved error handling
* more extensive automated testing
* support for larger datasets
* additional search and filtering operations
* improved command-line interface

These extensions could be added while preserving the current separation between file storage, keyword indexing, and ranking.

---

## Sources and AI Usage

The implementation is based on standard data structures and algorithms studied in C programming and data structures courses.

AI tools were used for:

* README organization and formatting
* improving the clarity of technical explanations
* organizing the documentation of the implemented data structures and operations

The project-specific implementation and integration of the data structures are based on the project's source code.
