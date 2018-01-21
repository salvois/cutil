/*
Semi-intrusive binary min-heap container.
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
 * Instantiates the header for a semi-intrusive binary heap container.
 * @param BinaryHeap name of the container to instantiate.
 */
#define BinaryHeap_header(BinaryHeap) \
\
typedef struct BinaryHeap##_Node BinaryHeap##_Node;\
\
typedef bool BinaryHeap##_Less(BinaryHeap##_Node **value, BinaryHeap##_Node **other);\
\
struct BinaryHeap##_Node {\
    BinaryHeap##_Node  *parent;\
    BinaryHeap##_Node  *left;\
    BinaryHeap##_Node  *prev;\
    BinaryHeap##_Node  *next;\
    BinaryHeap##_Node **value;\
};\
\
typedef struct BinaryHeap {\
    BinaryHeap##_Node *root;\
    BinaryHeap##_Node *last;\
} BinaryHeap;\
\
static inline bool BinaryHeap##_isEmpty(const BinaryHeap *heap) {\
    return heap->root == NULL;\
}\
\
static inline BinaryHeap##_Node* BinaryHeap##_peek(const BinaryHeap *heap) {\
    return heap->root;\
}\
\
static inline void BinaryHeap##_initialize(BinaryHeap *heap) {\
    heap->root = NULL;\
    heap->last = NULL;\
}\
\
void BinaryHeap##_insert(BinaryHeap *heap, BinaryHeap##_Node *node);\
BinaryHeap##_Node *BinaryHeap##_remove(BinaryHeap *heap, BinaryHeap##_Node *node);\
BinaryHeap##_Node *BinaryHeap##_poll(BinaryHeap *heap);\
BinaryHeap##_Node *BinaryHeap##_pollAndInsert(BinaryHeap *heap, BinaryHeap##_Node *newNode);\
void BinaryHeap##_update(BinaryHeap *heap, BinaryHeap##_Node *node);\
void BinaryHeap##_check(const BinaryHeap *heap);


/**
 * Instantiates the implementation for a semi-intrusive binary heap container.
 * @param BinaryHeap name of the container to instantiate.
 * @param isLess name of the function comparing nodes having the following prototype: bool isLess(BinaryHeap_Node **value, BinaryHeap_Node **other)
 */
#define BinaryHeap_implementation(BinaryHeap, isLess) \
\
static void setNode(BinaryHeap##_Node *node, BinaryHeap##_Node **value) {\
    node->value = value;\
    *value = node;\
}\
\
static void heapifyUp(BinaryHeap *heap, BinaryHeap##_Node *x) {\
    BinaryHeap##_Node **value = x->value;\
    BinaryHeap##_Node *p = x->parent;\
    while ((p != NULL) && isLess(value, p->value)) {\
        setNode(x, p->value);\
        x = p;\
        p = x->parent;\
    }\
    setNode(x, value);\
}\
\
static void heapifyDown(BinaryHeap *heap, BinaryHeap##_Node *x) {\
    BinaryHeap##_Node **value = x->value;\
    BinaryHeap##_Node *c = x->left;\
    while (c != NULL) {\
        if ((c->next != NULL) && isLess(c->next->value, c->value)) c = c->next;\
        if (!isLess(c->value, value)) break;\
        setNode(x, c->value);\
        x = c;\
        c = x->left;\
    }\
    setNode(x, value);\
}\
\
static BinaryHeap##_Node *removeLast(BinaryHeap *heap) {\
    BinaryHeap##_Node *result = heap->last; /* recycle the former last node as result */\
    heap->last = heap->last->prev;\
    if (heap->last != NULL) {\
        heap->last->next = NULL;\
        assert(result->parent != NULL);\
        if (result->parent->left == result) {\
            result->parent->left = NULL;\
        }\
    } else {\
        heap->root = NULL;\
    }\
    return result;\
}\
\
static void update(BinaryHeap *heap, BinaryHeap##_Node *node) {\
    assert(!BinaryHeap##_isEmpty(heap));\
    if ((node != heap->root) && isLess(node->value, node->parent->value)) {\
        heapifyUp(heap, node);\
    } else {\
        heapifyDown(heap, node);\
    }\
}\
\
/** Inserts the specified node into the heap. */\
void BinaryHeap##_insert(BinaryHeap *heap, BinaryHeap##_Node *newNode) {\
    BinaryHeap##_Node *last = heap->last;\
    newNode->left = NULL;\
    newNode->prev = last;\
    newNode->next = NULL;\
    if (heap->root == NULL) {\
        heap->root = newNode;\
        heap->last = newNode;\
        newNode->parent = NULL;\
        return;\
    }\
    BinaryHeap##_Node *parent;\
    if (heap->root == last) {\
        parent = heap->root;\
        parent->left = newNode;\
    } else {\
        parent = last->parent;\
        assert(parent != NULL);\
        assert(parent->left != NULL);\
        if (parent->left != last) {\
            parent = parent->next;\
            parent->left = newNode;\
        }\
    }\
    newNode->parent = parent;\
    last->next = newNode;\
    heap->last = newNode;\
    heapifyUp(heap, newNode);\
}\
\
/** Removes the node for the minimum element from the heap. */\
BinaryHeap##_Node *BinaryHeap##_poll(BinaryHeap *heap) {\
    assert(!BinaryHeap##_isEmpty(heap));\
    BinaryHeap##_Node *result;\
    if (heap->root != heap->last) {\
        BinaryHeap##_Node **value = heap->root->value;\
        setNode(heap->root, heap->last->value);\
        result = removeLast(heap);\
        setNode(result, value);\
        heapifyDown(heap, heap->root);\
    } else {\
        result = heap->root;\
        heap->root = NULL;\
        heap->last = NULL;\
    }\
    return result;\
}\
\
/** Removes the specified node from the heap. */\
BinaryHeap##_Node *BinaryHeap##_remove(BinaryHeap *heap, BinaryHeap##_Node *node) {\
    BinaryHeap##_Node *result;\
    assert(!BinaryHeap##_isEmpty(heap));\
    if (node != heap->last) {\
        BinaryHeap##_Node **value = node->value;\
        setNode(node, heap->last->value);\
        result = removeLast(heap);\
        setNode(result, value);\
        update(heap, node);\
    } else {\
        result = removeLast(heap);\
    }\
    return result;\
}\
\
/** Updates the heap structure after a change to the key of the specified node. */\
void BinaryHeap##_update(BinaryHeap *heap, BinaryHeap##_Node *node) {\
    update(heap, node);\
}\
\
/** Combines a poll and an insert in a single efficient operation. */\
BinaryHeap##_Node *BinaryHeap##_pollAndInsert(BinaryHeap *heap, BinaryHeap##_Node *newNode) {\
    assert(!BinaryHeap##_isEmpty(heap));\
    BinaryHeap##_Node **value = newNode->value;\
    setNode(newNode, heap->root->value);\
    setNode(heap->root, value);\
    heapifyDown(heap, heap->root);\
    return newNode;\
}\
\
/** Combines a remove and an insert in a single efficient operation. */\
void BinaryHeap##_replace(BinaryHeap *heap, BinaryHeap##_Node *oldNode, BinaryHeap##_Node *newNode) {\
    assert(!BinaryHeap##_isEmpty(heap));\
    BinaryHeap##_Node **value = newNode->value;\
    setNode(newNode, oldNode->value);\
    setNode(oldNode, value);\
    update(heap, oldNode);\
}\
\
/** Checks heap invariants. */\
void BinaryHeap##_check(const BinaryHeap *heap) {\
    for (const BinaryHeap##_Node *i = heap->root; i != NULL; i = i->next) {\
        if (i->parent != NULL) {\
            assert(!isLess(i->value, i->parent->value));\
        }\
    }\
}
