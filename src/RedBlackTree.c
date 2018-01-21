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
 * This file includes common code not dependent on element keys or values,
 * and must be linked with the program using the container.
 * To instantiate code dependent on element keys or values, see the comments
 * on the AvlTree structure.
 ******************************************************************************/
#include "RedBlackTree.h"

static inline RedBlackTree_Node *getParent(const RedBlackTree_Node *node) {
    return (RedBlackTree_Node *) (node->parent & ~1);
}

static inline bool isBlack(const RedBlackTree_Node *node) {
    assert(RedBlackTree_black == 1);
    return node->parent & 1;
}

static inline bool isRed(const RedBlackTree_Node *node) {
    assert(RedBlackTree_black == 0);
    return !isBlack(node);
}

static inline void setBlack(RedBlackTree_Node *node) {
    node->parent |= 1;
}

static inline void setRed(RedBlackTree_Node *node) {
    node->parent &= ~1;
}

static inline void setColor(RedBlackTree_Node *node, RedBlackTree_Color color) {
    node->parent = (node->parent & ~1) | color;
}

static void rotateLeft(RedBlackTree *tree, RedBlackTree_Node *parent) {
    RedBlackTree_Node *child = parent->right;
    RedBlackTree_setParent(child, getParent(parent));
    parent->right = child->left;
    child->left = parent;
    RedBlackTree_setParent(parent, child);
    if (parent->right != NULL) RedBlackTree_setParent(parent->right, parent);
    if (parent == tree->root) tree->root = child;
    else if (getParent(child)->right == parent) getParent(child)->right = child;
    else getParent(child)->left = child;
}

static void rotateRight(RedBlackTree *tree, RedBlackTree_Node *parent) {
    RedBlackTree_Node *child = parent->left;
    RedBlackTree_setParent(child, getParent(parent));
    parent->left = child->right;
    child->right = parent;
    RedBlackTree_setParent(parent, child);
    if (parent->left != NULL) RedBlackTree_setParent(parent->left, parent);
    if (parent == tree->root) tree->root = child;
    else if (getParent(child)->left == parent) getParent(child)->left = child;
    else getParent(child)->right = child;
}

static inline RedBlackTree_Node *findMin(RedBlackTree_Node *root) {
    while (root->left != NULL) root = root->left;
    return root;
}

static inline RedBlackTree_Node *findMax(RedBlackTree_Node *root) {
    while (root->right != NULL) root = root->right;
    return root;
}

void RedBlackTree_postInsert(RedBlackTree *tree, RedBlackTree_Node *node) {
    setRed(node);
    while (true) {
        RedBlackTree_Node *parent = getParent(node);
        if (node == tree->root || isBlack(parent)) {
            break;
        }
        RedBlackTree_Node *grandParent = getParent(parent);
        assert(grandParent != NULL);
        if (parent == grandParent->left) {
            RedBlackTree_Node *uncle = grandParent->right;
            if (uncle != NULL && isRed(uncle)) {
                setBlack(parent);
                setBlack(uncle);
                setRed(grandParent);
                node = grandParent;
            } else {
                if (node == parent->right) {
                    node = parent;
                    rotateLeft(tree, node);
                    parent = getParent(node);
                    grandParent = getParent(parent);
                }
                setBlack(parent);
                setRed(grandParent);
                rotateRight(tree, grandParent);
            }
        } else {
            RedBlackTree_Node *uncle = grandParent->left;
            if (uncle != NULL && isRed(uncle)) {
                setBlack(parent);
                setBlack(uncle);
                setRed(grandParent);
                node = grandParent;
            } else {
                if (node == parent->left) {
                    node = parent;
                    rotateRight(tree, node);
                    parent = getParent(node);
                    grandParent = getParent(parent);
                }
                setBlack(parent);
                setRed(grandParent);
                rotateLeft(tree, grandParent);
            }
        }
    }
    setBlack(tree->root);
}

void RedBlackTree_remove(RedBlackTree *tree, RedBlackTree_Node *node) {
    RedBlackTree_Node *successor = NULL; // becomes not null if node has two children
    RedBlackTree_Node *x = NULL;
    RedBlackTree_Node *xParent = NULL;
    if (node->left == NULL) x = node->right;
    else if (node->right == NULL) x = node->left;
    else {
        successor = findMin(node->right);
        x = successor->right;
    }
    RedBlackTree_Node *zp = getParent(node);
    if (successor != NULL) {
        RedBlackTree_setParent(node->left, successor);
        successor->left = node->left;
        if (successor != node->right) {
            xParent = getParent(successor);
            if (x != NULL) RedBlackTree_setParent(x, getParent(successor));
            getParent(successor)->left = x;
            successor->right = node->right;
            RedBlackTree_setParent(node->right, successor);
        } else {
            xParent = successor;
        }
        if (tree->root == node) tree->root = successor;
        else if (zp->left == node) zp->left = successor;
        else zp->right = successor;
        bool yBlack = isBlack(successor);
        successor->parent = node->parent; // both same parent and color
        setColor(node, yBlack);
    } else {
        xParent = getParent(node);
        if (x != NULL) RedBlackTree_setParent(x, getParent(node));
        if (tree->root == node) tree->root = x;
        else if (zp->left == node) zp->left = x;
        else zp->right = x;
        if (tree->leftmost == node) {
            if (node->right == NULL) {
                assert(node->left == NULL);
                tree->leftmost = zp;
            } else {
                tree->leftmost = findMin(x);
            }
        }
        if (tree->rightmost == node) {
            if (node->left == NULL) {
                assert(node->right == NULL);
                tree->rightmost = zp;
            } else {
                tree->rightmost = findMax(x);
            }
        }
    }
    // Balance
    if (isRed(node)) return;
    while (x != tree->root && (x == NULL || isBlack(x))) {
        if (x == xParent->left) {
            RedBlackTree_Node *w = xParent->right;
            if (isRed(w)) {
                setBlack(w);
                setRed(xParent);
                rotateLeft(tree, xParent);
                w = xParent->right;
            }
            if (((w->left == NULL) || isBlack(w->left)) && ((w->right == NULL) || isBlack(w->right))) {
                setRed(w);
                x = xParent;
                xParent = getParent(xParent);
            } else {
                if ((w->right == NULL) || isBlack(w->right)) {
                    if (w->left != NULL) setBlack(w->left);
                    setRed(w);
                    rotateRight(tree, w);
                    w = xParent->right;
                }
                setColor(w, isBlack(xParent));
                setBlack(xParent);
                if (w->right != NULL) setBlack(w->right);
                rotateLeft(tree, xParent);
                break;
            }
        } else {
            RedBlackTree_Node *w = xParent->left;
            if (isRed(w)) {
                setBlack(w);
                setRed(xParent);
                rotateRight(tree, xParent);
                w = xParent->left;
            }
            if (((w->right == NULL) || isBlack(w->right)) && ((w->left == NULL) || isBlack(w->left))) {
                setRed(w);
                x = xParent;
                xParent = getParent(xParent);
            } else {
                if ((w->left == NULL) || isBlack(w->left)) {
                    if (w->right != NULL) setBlack(w->right);
                    setRed(w);
                    rotateLeft(tree, w);
                    w = xParent->left;
                }
                setColor(w, isBlack(xParent));
                setBlack(xParent);
                if (w->left != NULL) setBlack(w->left);
                rotateRight(tree, xParent);
                break;
            }
        }
    }
    if (x != NULL) setBlack(x);
}
