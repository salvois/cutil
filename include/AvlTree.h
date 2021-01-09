/*
Intrusive AVL tree container.
Copyright 2015-2020 Salvatore ISAJA. All rights reserved.

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
 * This files contains a poor man's template definition.
 * In order to use the container, you need to instantiate the poor man's
 * template macro to expand code dependent on element keys or values.
 * See the comments on the AvlTree structure.
 * Moreover, you must link the AvlTree.c file to your program.
 ******************************************************************************/
#ifndef AVLTREE_H_INCLUDED
#define	AVLTREE_H_INCLUDED

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/** Internal balance factors. */
enum AvlTree_Balance {
    AvlTree_balanced,
    AvlTree_rightHeavy,
    AvlTree_leftHeavy
};

typedef struct AvlTree_Node AvlTree_Node;

/** Node of an AVL tree. Embed into elements to be added to AVL trees. */
struct AvlTree_Node {
    uintptr_t     parent; // balance factor in lowest two bits (enum AvlTree_Balance)
    AvlTree_Node *left;
    AvlTree_Node *right;
};

/**
 * AVL tree caching the leftmost and rightmost nodes.
 *
 * The implementation is generic, in that it doesn't depends on element
 * keys or values. To create a useful container:
 * - create an actual element type embedding an AvlTree_Node structure;
 * - create a function taking (AvlTree_Node *node, AvlTree_Node *other) and
 *   returning a bool indicating whether node is less than other;
 * - instantiate an insertion function using the AvlTree_instantiateInsert
 *   macro, that expands the code to insert an element into an AVL tree
 *   aware of node ordering (using the previously defined comparator).
 */
typedef struct AvlTree {
    AvlTree_Node sentinel; // used both to point to the root (via .left) and as leaf sentinel (.parent is updated but not meaningful)
    AvlTree_Node *leftmost;
    AvlTree_Node *rightmost;
} AvlTree;

/** Returns true if the AVL tree contains no elements. */
static inline bool AvlTree_isEmpty(const AvlTree *tree) {
    return tree->sentinel.left == &tree->sentinel;
}

static inline AvlTree_Node *AvlTree_getParent(const AvlTree_Node *node) {
    return (AvlTree_Node *) (node->parent & ~3);
}

static inline int AvlTree_getBalance(const AvlTree_Node *node) {
    return node->parent & 3;
}

/** Initializes an empty AVL tree. */
void AvlTree_initialize(AvlTree *tree);

/** Removes the specified node from an AVL tree. */
void AvlTree_remove(AvlTree *tree, AvlTree_Node *node);

/** Internal function called after insertion of a node into an AVL tree. */
void AvlTree_rebalanceAfterInsertion(AvlTree *tree, AvlTree_Node *node);

/******************************************************************************
 * Example instantiation with a node with uintptr_t key.
 * Complete by calling AvlTree_instantiateInsert(AvlTreeUintptr_insert,
 *         AvlTreeUintptr_Node_isLess)
 ******************************************************************************/

typedef struct AvlTreeUintptr_Node {
    uintptr_t key;
    AvlTree_Node node;
} AvlTreeUintptr_Node;

static inline AvlTreeUintptr_Node *AvlTreeUintptr_Node_getNode(AvlTree_Node *n) {
    return (AvlTreeUintptr_Node *) ((uint8_t *) n - offsetof(AvlTreeUintptr_Node, node));
}

static inline bool AvlTreeUintptr_Node_isLess(AvlTree_Node *node, AvlTree_Node *other) {
    return AvlTreeUintptr_Node_getNode(node)->key < AvlTreeUintptr_Node_getNode(other)->key;
}


/******************************************************************************
 * Code dependent on the node key
 ******************************************************************************/

/**
 * Instantiates an insertion function for a concrete AVL tree.
 * @param functionName name of the function to generate (e.g. AvlTreeUintptr_insert)
 * @param isLess name of the function comparing nodes.
 */
#define AvlTree_instantiateInsert(functionName, isLess)\
static void functionName##_insertBeforeLeftmost(AvlTree *tree, AvlTree_Node *node) {\
    node->parent = (uintptr_t) tree->leftmost | AvlTree_balanced;\
    tree->leftmost->left = node;\
    tree->leftmost = node;\
}\
\
static void functionName##_insertAfterRightmost(AvlTree *tree, AvlTree_Node *node) {\
    node->parent = (uintptr_t) tree->rightmost | AvlTree_balanced;\
    tree->rightmost->right = node;\
    tree->rightmost = node;\
}\
\
static void functionName##_insertInOrder(AvlTree *tree, AvlTree_Node *node) {\
    AvlTree_Node *i = tree->sentinel.left;\
    while (i != &tree->sentinel) {\
        if (isLess(node, i)) {\
            if (i->left == &tree->sentinel) {\
                i->left = node;\
                break;\
            }\
            i = i->left;\
        } else {\
            if (i->right == &tree->sentinel) {\
                i->right = node;\
                break;\
            }\
            i = i->right;\
        }\
    }\
    node->parent = (uintptr_t) i | AvlTree_balanced;\
}\
\
void functionName(AvlTree *tree, AvlTree_Node *node) {\
    node->left = &tree->sentinel;\
    node->right = &tree->sentinel;\
    if (!AvlTree_isEmpty(tree)) {\
        if (isLess(node, tree->leftmost)) {\
            functionName##_insertBeforeLeftmost(tree, node);\
        } else if (!isLess(node, tree->rightmost)) {\
            functionName##_insertAfterRightmost(tree, node);\
        } else {\
            functionName##_insertInOrder(tree, node);\
        }\
        AvlTree_rebalanceAfterInsertion(tree, node);\
    } else {\
        node->parent = (uintptr_t) &tree->sentinel | AvlTree_balanced;\
        tree->sentinel.left = node;\
        tree->leftmost = node;\
        tree->rightmost = node;\
    }\
}

#endif
