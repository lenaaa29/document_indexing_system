#include "search_index.h"

/*
 * Compares priority between two files.
 *
 * Input:
 *  a, b - file structures
 *
 * Returns:
 *  1 if file a has higher priority than b, otherwise 0.
 *
 * Description:
 *  Files are ordered by score (descending). If scores are equal,
 *  lexicographically smaller id has higher priority.
 */
int fileHigherPriority(File *a, File *b) {
	if (a->score != b->score) {
		return a->score > b->score;
	}
	return strcmp(a->id, b->id) < 0;
}

/*
 * Creates a max-heap structure.
 *
 * Input:
 *  capacity - initial capacity of the heap
 *
 * Returns:
 *  Pointer to the created Heap.
 *
 * Description:
 *  Allocates the heap structure and the internal array used
 *  to store heap elements.
 */
Heap *createHeap(int capacity) {
	Heap *heap = malloc(sizeof(Heap));
	if (!heap) {
		fprintf(stderr, "Error: memory allocation failed for heap.\n");
		return NULL;
	}

	heap->size = 0;
	// ensure minimum capacity
	heap->capacity = (capacity > 0) ? capacity : 2;
	// allocate array for heap elements
	heap->elem = malloc(heap->capacity * sizeof(HeapItem));
	if (!heap->elem) {
		free(heap);
		return NULL;
	}
	return heap;
}

/*
 * Restores heap property by moving an element upward.
 *
 * Input:
 *  heap - heap structure
 *  idx  - index of the element to adjust
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Repeatedly swaps the element with its parent while it has
 *  higher priority.
 */
void siftUp(Heap *heap, int idx) {
	while (idx > 0) {
		int p = (idx - 1) / 2;
		// if current element has higher priority than parent, swap
		if (fileHigherPriority(heap->elem[idx].file, heap->elem[p].file)) {
			HeapItem tmp = heap->elem[idx];
			heap->elem[idx] = heap->elem[p];
			heap->elem[p] = tmp;
			idx = p;
		} else {
			break;
		}
	}
}

/*
 * Restores heap property by moving an element downward.
 *
 * Input:
 *  heap - heap structure
 *  idx  - index of the element to adjust
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Swaps the element with the child of higher priority until
 *  the heap property is satisfied.
 */
void siftDown(Heap *heap, int idx) {
	while (1) {
		int max = idx;
		int l = 2 * idx + 1;	// left child
		int r = 2 * idx + 2;	// right child

		// select child with highest priority
		if (l < heap->size && fileHigherPriority(heap->elem[l].file, heap->elem[max].file)) {
			max = l;
		}
		if (r < heap->size && fileHigherPriority(heap->elem[r].file, heap->elem[max].file)) {
			max = r;
		}

		// if no change needed, heap property is satisfied
		if (max == idx) {
			break;
		}

		// swap with highest priority child
		HeapItem tmp = heap->elem[idx];
		heap->elem[idx] = heap->elem[max];
		heap->elem[max] = tmp;
		idx = max;
	}
}

/*
 * Inserts a file into the heap.
 *
 * Input:
 *  heap - heap structure
 *  file - file to insert
 *
 * Returns:
 *  nothing
 *
 * Description:
 *  Adds the file at the end of the heap and restores the heap
 *  property using siftUp.
 */
void heapInsert(Heap *heap, File *file) {
	// resize heap if needed
	if (heap->size == heap->capacity) {
		heap->capacity *= 2;
		heap->elem = realloc(heap->elem, heap->capacity * sizeof(HeapItem));
	}

	// add new element at the end and restore heap
	heap->elem[heap->size].file = file;
	siftUp(heap, heap->size++);
}

/*
 * Extracts the highest priority file from the heap.
 *
 * Input:
 *  heap - heap structure
 *
 * Returns:
 *  Pointer to the file with maximum priority.
 *
 * Description:
 *  Removes the root element and restores the heap using siftDown.
 */
File *heapExtractMax(Heap *heap) {
	if (!heap || heap->size == 0 || !heap->elem) {
		return NULL;
	}

	// store root (max element)
	File *res = heap->elem[0].file;
	// replace root with last element and restore heap
	heap->elem[0] = heap->elem[--heap->size];
	siftDown(heap, 0);
	return res;
}
