/*
Intrusive leftist (min) heap container.
Copyright 2017-2018 Salvatore ISAJA. All rights reserved.

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
 * Instantiates the header for an intrusive leftist heap container.
 * @param LeftistHeap name of the container to instantiate.
 */
#define LeftistHeap_header(LeftistHeap) \
\
typedef struct LeftistHeap##_Node LeftistHeap##_Node;\
\
struct LeftistHeap##_Node {\
    LeftistHeap##_Node *parent;\
    LeftistHeap##_Node *left;\
    LeftistHeap##_Node *right;\
    size_t s;\
};\
\
typedef struct LeftistHeap {\
    LeftistHeap##_Node *root;\
} LeftistHeap;\
\
static void LeftistHeap##_initialize(LeftistHeap *heap) {\
    heap->root = NULL;\
}\
\
static inline bool LeftistHeap##_isEmpty(const LeftistHeap *tree) {\
    return tree->root == NULL;\
}\
\
void LeftistHeap##_insert(LeftistHeap *heap, LeftistHeap##_Node *node);\
void LeftistHeap##_remove(LeftistHeap *heap, LeftistHeap##_Node *node);\
LeftistHeap##_Node *LeftistHeap##_poll(LeftistHeap *heap);\
void LeftistHeap##_merge(LeftistHeap *heap, LeftistHeap *other);\
void LeftistHeap##_check(LeftistHeap *heap);


/**
 * Instantiates the implementation for an intrusive leftist heap container.
 * @param LeftistHeap name of the container to instantiate.
 * @param isLess name of the function comparing nodes having the following prototype: bool isLess(LeftistHeap##_Node *node, LeftistHeap##_Node *other)
 */
#define LeftistHeap_implementation(LeftistHeap, isLess) \
\
/**
 * Implements the main merge routine as described in:
 * https://www.geeksforgeeks.org/leftist-tree-leftist-heap/
 * 
 * Paraphrasing it in pseudocode:
 * loop
 *     compare the roots of two heaps
 *     push the smaller node onto a stack
 *     move to the right child of smaller node
 * until a null node is reached
 * loop
 *     pop the last processed node from the stack
 *     attach the last processed node as the right child of the node on top of the stack
 *     swap children if needed to restore the leftist heap property
 * until the stack is empty
 * the last popped element is the new root
 * 
 * @return The new root.
 */\
static LeftistHeap##_Node *merge(LeftistHeap##_Node *n1, LeftistHeap##_Node *n2) {\
    LeftistHeap##_Node *smaller;\
    LeftistHeap##_Node *bigger;\
    if (isLess(n1, n2)) {\
        smaller = n1;\
        bigger = n2;\
    } else {\
        smaller = n2;\
        bigger = n1;\
    }\
    LeftistHeap##_Node *newRoot = smaller;\
    LeftistHeap##_Node *top = smaller;\
    while (true) {\
        LeftistHeap##_Node *n = smaller->right;\
        if (n == NULL) break;\
        if (isLess(n, bigger)) {\
            smaller = n;\
        } else {\
            smaller = bigger;\
            bigger = n;\
        }\
        smaller->parent = top;\
        top = smaller;\
    }\
    LeftistHeap##_Node *last = bigger;\
    bigger->parent = top;\
    while (last != newRoot) {\
        top->right = last;\
        size_t leftS = (top->left != NULL) ? top->left->s : 0;\
        size_t rightS = (top->right != NULL) ? top->right->s : 0;\
        if (leftS < rightS) {\
            LeftistHeap##_Node *t = top->left;\
            top->left = top->right;\
            top->right = t;\
            top->s = leftS + 1;\
        } else {\
            top->s = rightS + 1;\
        }\
        last = top;\
        top = top->parent;\
    }\
    return last;\
}\
\
/** Inserts the specified node into the heap. */\
void LeftistHeap##_insert(LeftistHeap *heap, LeftistHeap##_Node *node) {\
    node->parent = NULL;\
    node->left = NULL;\
    node->right = NULL;\
    node->s = 1;\
    heap->root = (heap->root != NULL) ? merge(heap->root, node) : node;\
}\
\
/** Removes the specified node from the heap. */\
void LeftistHeap##_remove(LeftistHeap *heap, LeftistHeap##_Node *node) {\
    if (node == heap->root) {\
        assert(node->parent == NULL);\
        LeftistHeap##_poll(heap);\
        return;\
    }\
    LeftistHeap##_Node *parent = node->parent;\
    LeftistHeap##_Node *child;\
    assert(parent != NULL);\
    if (node->left == NULL) {\
        child = node->right;\
    } else if (node->right == NULL) {\
        child = node->left;\
    } else {\
        child = merge(node->left, node->right);\
    }\
    if (child != NULL) child->parent = parent;\
    if (parent->left == node) parent->left = child;\
    else parent->right = child;\
}\
\
/** Removes the node for the minimum element from the heap. */\
LeftistHeap##_Node *LeftistHeap##_poll(LeftistHeap *heap) {\
    LeftistHeap##_Node *root = heap->root;\
    if (root->left == NULL) {\
        heap->root = root->right;\
    } else if (root->right == NULL) {\
        heap->root = root->left;\
    } else {\
        heap->root = merge(root->left, root->right);\
    }\
    if (heap->root != NULL) heap->root->parent = NULL;\
    return root;\
}\
\
/** Merges to non-empty heaps. */\
void LeftistHeap##_merge(LeftistHeap *heap, LeftistHeap *other) {\
    heap->root = merge(heap->root, other->root);\
}\
\
static void check(LeftistHeap##_Node *node) {\
    if (node->parent != NULL) assert(!isLess(node, node->parent));\
    size_t leftS = 0;\
    if (node->left != NULL) {\
        assert(node->left->parent == node);\
        check(node->left);\
        leftS = node->left->s;\
    }\
    size_t rightS = 0;\
    if (node->right != NULL) {\
        assert(node->right->parent == node);\
        check(node->right);\
        rightS = node->right->s;\
    }\
    assert(rightS <= leftS);\
    assert(node->s == rightS + 1);\
}\
\
/** Checks heap invariants. */\
void LeftistHeap##_check(LeftistHeap *heap) {\
    if (heap->root != NULL) {\
        assert(heap->root->parent == NULL);\
        check(heap->root);\
    }\
}
