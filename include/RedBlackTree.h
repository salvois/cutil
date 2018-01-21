/*
Intrusive red-black tree container.
Copyright 2013-2018 Salvatore ISAJA. All rights reserved.

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

The tree balancing code is courtesy of STLPort 5.2.1:

Copyright (c) 1994 Hewlett-Packard Company
Copyright (c) 1996,1997 Silicon Graphics Computer Systems, Inc.
Copyright (c) 1997 Moscow Center for SPARC Technology
Copyright (c) 1999 Boris Fomitchev
This material is provided "as is", with absolutely no warranty expressed
or implied. Any use is at your own risk.
Permission to use or copy this software for any purpose is hereby granted
without fee, provided the above notices are retained on all copies.
Permission to modify the code and to distribute modified code is granted,
provided the above notices are retained, and a notice that the code was
modified is included with the above copyright notice.
*/

/******************************************************************************
 * This files contains a poor man's template definition.
 * In order to use the container, you need to instantiate the poor man's
 * template macro to expand code dependent on element keys or values.
 * See the comments on the RedBlackTree structure.
 * Moreover, you must link the RedBlackTree.c file to your program.
 ******************************************************************************/
#ifndef REDBLACKTREE_H_INCLUDED
#define	REDBLACKTREE_H_INCLUDED

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct RedBlackTree_Node RedBlackTree_Node;

/** Node of a red-black tree. Embed into elements to be added to red-black trees. */
struct RedBlackTree_Node {
    uintptr_t parent; // color in least significant bit (see RedBlackTree_Color)
    RedBlackTree_Node *left;
    RedBlackTree_Node *right;
};

/**
 * Red-black tree caching the leftmost and rightmost nodes.
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
typedef struct RedBlackTree {
    RedBlackTree_Node *root;
    RedBlackTree_Node *leftmost;
    RedBlackTree_Node *rightmost;
} RedBlackTree;

/** Initializes an empty red-black tree. */
static inline void RedBlackTree_initialize(RedBlackTree *tree) {
    tree->root = NULL;
    tree->leftmost = NULL;
    tree->rightmost = NULL;
}

/** Returns true if the red-black tree contains no elements. */
static inline bool RedBlackTree_isEmpty(const RedBlackTree *tree) {
    return tree->root == NULL;
}

/** Removes the specified node from a red-black tree. */
void RedBlackTree_remove(RedBlackTree *tree, RedBlackTree_Node *node);

/** Internal function called after insertion of a node into a red-black tree. */
void RedBlackTree_postInsert(RedBlackTree *tree, RedBlackTree_Node *node);

/** Internal node colors. */
typedef enum RedBlackTree_Color {
    RedBlackTree_red = 0,
    RedBlackTree_black = 1
} RedBlackTree_Color;

/** Internal function to set the parent of a node without changing its color. */
static inline void RedBlackTree_setParent(RedBlackTree_Node *node, RedBlackTree_Node *parent) {
    assert(((uintptr_t) parent & 1) == 0);
    node->parent = (node->parent & 1) | (uintptr_t) parent;
}


/******************************************************************************
 * Code dependent on the node key
 ******************************************************************************/

/**
 * Instantiates an insertion function for a concrete red-black tree.
 * @param functionName name of the function to generate (e.g. RedBlackTree_insert)
 * @param isLess name of the function comparing nodes.
 */
#define RedBlackTree_instantiateInsert(functionName, isLess)\
void functionName(RedBlackTree *tree, RedBlackTree_Node *node) {\
    node->left = NULL;\
    node->right = NULL;\
    if (tree->root != NULL) {\
        if (isLess(node, tree->leftmost)) {\
            RedBlackTree_setParent(node, tree->leftmost);\
            tree->leftmost->left = node;\
            tree->leftmost = node;\
        } else if (!isLess(node, tree->rightmost)) {\
            RedBlackTree_setParent(node, tree->rightmost);\
            tree->rightmost->right = node;\
            tree->rightmost = node;\
        } else {\
            RedBlackTree_Node *i = tree->root;\
            while (i != NULL) {\
                if (isLess(node, i)) {\
                    if (i->left == NULL) {\
                        i->left = node;\
                        break;\
                    }\
                    i = i->left;\
                } else {\
                    if (i->right == NULL) {\
                        i->right = node;\
                        break;\
                    }\
                    i = i->right;\
                }\
            }\
            RedBlackTree_setParent(node, i);\
        }\
        RedBlackTree_postInsert(tree, node);\
    } else {\
        node->parent = RedBlackTree_black;\
        tree->root = node;\
        tree->leftmost = node;\
        tree->rightmost = node;\
    }\
}

#endif
