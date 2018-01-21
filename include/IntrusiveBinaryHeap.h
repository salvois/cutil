/*
Intrusive binary min-heap container.
Copyright 2009-2018 Salvatore ISAJA. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED THE COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/******************************************************************************
 * This is a poor man's template file.
 * In order to use the container, you need to instantiate the poor man's
 * template macros for header and implementation.
 ******************************************************************************/
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Instantiates the header for an intrusive binary heap container.
 * @param IntrusiveBinaryHeap name of the container to instantiate.
 */
#define IntrusiveBinaryHeap_header(IntrusiveBinaryHeap) \
\
typedef struct IntrusiveBinaryHeap##_Node IntrusiveBinaryHeap##_Node;\
\
struct IntrusiveBinaryHeap##_Node {\
    size_t index;\
    IntrusiveBinaryHeap##_Node *left;\
    IntrusiveBinaryHeap##_Node *right;\
};\
\
typedef struct IntrusiveBinaryHeap {\
    IntrusiveBinaryHeap##_Node *root;\
    size_t count;\
} IntrusiveBinaryHeap;\
\
static inline void IntrusiveBinaryHeap##_initialize(IntrusiveBinaryHeap *heap) {\
    heap->root = NULL;\
    heap->count = 0;\
}\
\
static inline bool IntrusiveBinaryHeap##_isEmpty(const IntrusiveBinaryHeap *heap) {\
    return heap->root == NULL;\
}\
\
static inline IntrusiveBinaryHeap##_Node* IntrusiveBinaryHeap##_peek(const IntrusiveBinaryHeap *heap) {\
    return heap->root;\
}\
\
void IntrusiveBinaryHeap##_insert(IntrusiveBinaryHeap *heap, IntrusiveBinaryHeap##_Node *node);\
void IntrusiveBinaryHeap##_remove(IntrusiveBinaryHeap *heap, IntrusiveBinaryHeap##_Node *node);\
IntrusiveBinaryHeap##_Node *IntrusiveBinaryHeap##_poll(IntrusiveBinaryHeap *heap);\
IntrusiveBinaryHeap##_Node *IntrusiveBinaryHeap##_pollAndInsert(IntrusiveBinaryHeap *heap, IntrusiveBinaryHeap##_Node *newNode);\
void IntrusiveBinaryHeap##_check(IntrusiveBinaryHeap *heap);


/**
 * Instantiates the implementation for an intrusive binary heap container.
 * @param IntrusiveBinaryHeap name of the container to instantiate.
 * @param isLess name of the function comparing nodes having the following prototype: bool isLess(InstusiveBinaryHeap_Node *node, IntrusiveBinaryHeap_Node *other)
 */
#define IntrusiveBinaryHeap_implementation(IntrusiveBinaryHeap, isLess) \
\
/**
 * Computes the level mask for the specified 1-based node index.
 * The level mask is used to mask bits of the node index to decide whether
 * to descend to the left or right child at each level to reach the node
 * with the specified index.
 * Tree nodes:       Level mask:
 *        1          0
 *    10      11     1
 * 100 101 110 111   10
 * A 0 means turn left, a 1 means turn right. For example, to reach the node
 * with index 6 (110b) we need to turn right then left (discard the first 1,
 * then 1, then 0).
 */\
static inline unsigned getLevelMask(unsigned index) {\
    if (index == 1) return 0;\
    return 1 << (32 - __builtin_clz(index) - 2);\
}\
\
/**
 * Insert a node in the appropriate position descending from the root.
 * The main idea of the algorithm is to always start from the root,
 * resolve algorithmically the path to the point where the node must be
 * inserted, then shifting the other nodes up or down to respect the heap
 * property. This is done in order to avoid swapping nodes, touching the
 * same pointers multiple times, and to allow removal of random elements.
 * This is used either when inserting new nodes, or when nodes are removed,
 * inserting the former last element down from the root.
 * The specified node must have its index set to the position we wish to
 * insert it. For a new element, it is the last level-order element of the heap,
 * for a poll it is the root, for a random removal it is the hole where
 * the removed element was.
 */\
static void insertFromRoot(IntrusiveBinaryHeap *heap, IntrusiveBinaryHeap##_Node *descendingNode) {\
    /* Find where to insert the new node, respecting the heap property */\
    size_t index = descendingNode->index;\
    size_t levelMask = getLevelMask(index);\
    IntrusiveBinaryHeap##_Node *curr = heap->root;\
    IntrusiveBinaryHeap##_Node **parentLink = &heap->root;\
    while ((levelMask != 0) && !isLess(descendingNode, curr)) {\
        parentLink = ((index & levelMask) == 0) ? &curr->left : &curr->right;\
        curr = *parentLink;\
        levelMask >>= 1;\
    }\
    /* Here curr points to the node where the new node must be placed
     * (or NULL if it shall be a leaf), and parent points to its parent
     * (or NULL if it shall be the root).
     */\
    *parentLink = descendingNode;\
    while (levelMask != 0) {\
        assert(curr != NULL);\
        /* Shift nodes down towards the last level-order leaf */\
        descendingNode->index = curr->index;\
        if ((index & levelMask) == 0) {\
            descendingNode->left = curr;\
            descendingNode->right = curr->right;\
            parentLink = &descendingNode->left;\
            descendingNode = curr;\
            curr = curr->left;\
        } else {\
            descendingNode->right = curr;\
            descendingNode->left = curr->left;\
            parentLink = &descendingNode->right;\
            descendingNode = curr;\
            curr = curr->right;\
        }\
        levelMask >>= 1;\
    }\
    descendingNode->index = index;\
    if (curr == NULL) {\
        descendingNode->left = NULL;\
        descendingNode->right = NULL;\
        return;\
    }\
    /* Here we have reached our hole, the one with "index" index.
     * Parent points to the node occupying the hole,
     * parentLink to the link to parent.
     * If the current node is not less than its children, pull up children.
     */\
    IntrusiveBinaryHeap##_Node *left = curr->left;\
    IntrusiveBinaryHeap##_Node *right = curr->right;\
    descendingNode->left = left;\
    descendingNode->right = right;\
    while (true) {\
        IntrusiveBinaryHeap##_Node *higher = left;\
        if (higher == NULL) higher = right;\
        else if (right != NULL) higher = isLess(left, right) ? left : right;\
        if (higher == NULL || isLess(descendingNode, higher)) {\
            descendingNode->index = index;\
            descendingNode->left = left;\
            descendingNode->right = right;\
            *parentLink = descendingNode;\
            return;\
        }\
        IntrusiveBinaryHeap##_Node *childLeft = higher->left;\
        IntrusiveBinaryHeap##_Node *childRight = higher->right;\
        *parentLink = higher;\
        if (higher == left) {\
            higher->right = right;\
            parentLink = &higher->left;\
        } else {\
            higher->left = left;\
            parentLink = &higher->right;\
        }\
        size_t childIndex = higher->index;\
        higher->index = index;\
        index = childIndex;\
        left = childLeft;\
        right = childRight;\
    }\
}\
\
static IntrusiveBinaryHeap##_Node *removeLast(IntrusiveBinaryHeap *heap) {\
    assert(heap->count > 1);\
    size_t levelMask = getLevelMask(heap->count);\
    IntrusiveBinaryHeap##_Node *curr = heap->root;\
    IntrusiveBinaryHeap##_Node **parentLink;\
    while (true) {\
        parentLink = ((heap->count & levelMask) == 0) ? &curr->left : &curr->right;\
        curr = *parentLink;\
        levelMask >>= 1;\
        if (levelMask == 0) break;\
    }\
    *parentLink = NULL;\
    return curr;\
}\
\
/** Insert the specified node into the heap. */\
void IntrusiveBinaryHeap##_insert(IntrusiveBinaryHeap *heap, IntrusiveBinaryHeap##_Node *newNode) {\
    heap->count++;\
    newNode->index = heap->count;\
    insertFromRoot(heap, newNode);\
}\
\
/** Removes the specified node from the heap. */\
void IntrusiveBinaryHeap##_remove(IntrusiveBinaryHeap *heap, IntrusiveBinaryHeap##_Node *node) {\
    assert(heap->root != NULL);\
    if (heap->count > 1) {\
        IntrusiveBinaryHeap##_Node *last = removeLast(heap);\
        heap->count--;\
        if (node != last) {\
            last->index = node->index;\
            insertFromRoot(heap, last);\
        }\
    } else {\
        heap->root = NULL;\
        heap->count = 0;\
    }\
}\
\
/** Removes the node for the minimum element from the heap. */\
IntrusiveBinaryHeap##_Node *IntrusiveBinaryHeap##_poll(IntrusiveBinaryHeap *heap) {\
    assert(heap->root != NULL);\
    IntrusiveBinaryHeap##_Node *result = heap->root;\
    if (heap->count > 1) {\
        IntrusiveBinaryHeap##_Node *last = removeLast(heap);\
        heap->count--;\
        last->index = 1;\
        insertFromRoot(heap, last);\
    } else {\
        heap->root = NULL;\
        heap->count = 0;\
    }\
    return result;\
}\
\
/** Combines a poll and an insert in a single efficient operation. */\
IntrusiveBinaryHeap##_Node *IntrusiveBinaryHeap##_pollAndInsert(IntrusiveBinaryHeap *heap, IntrusiveBinaryHeap##_Node *newNode) {\
    assert(heap->root != NULL);\
    IntrusiveBinaryHeap##_Node *result = heap->root;\
    newNode->index = 1;\
    insertFromRoot(heap, newNode);\
    return result;\
}


/**
 * Instantiates the implementation for checking invariants of an intrusive binary heap container.
 * This performs a level order traversal, and uses malloc to allocate a queue of unvisited nodes.
 * @param IntrusiveBinaryHeap name of the container to instantiate.
 * @param isLess name of the function comparing nodes having the following prototype: bool isLess(BinaryHeap_Node **value, BinaryHeap_Node **other)
 */
#define IntrusiveBinaryHeap_debugImplementation(IntrusiveBinaryHeap, isLess) \
\
static void check(IntrusiveBinaryHeap *heap, IntrusiveBinaryHeap##_Node *parent, IntrusiveBinaryHeap##_Node *node) {\
    if (parent != NULL) {\
        assert(!isLess(node, parent));\
        assert(node->index > parent->index);\
    }\
    if (node->left != NULL) {\
        check(heap, node, node->left);\
    }\
    if (node->right != NULL) {\
        check(heap, node, node->right);\
    }\
}\
\
/** Checks structural invariants for the heap. */\
void IntrusiveBinaryHeap##_check(IntrusiveBinaryHeap *heap) {\
    if (heap->root != NULL) {\
        check(heap, NULL, heap->root);\
    }\
    IntrusiveBinaryHeap##_Node **nodes = malloc(heap->count * sizeof(IntrusiveBinaryHeap##_Node *));\
    size_t head = 0;\
    nodes[0] = heap->root;\
    size_t tail = 1;\
    while (head < heap->count) {\
        assert(nodes[head]->index == head + 1);\
        if (nodes[head]->left != NULL) nodes[tail++] = nodes[head]->left;\
        if (nodes[head]->right != NULL) nodes[tail++] = nodes[head]->right;\
        head++;\
    }\
    free(nodes);\
}
