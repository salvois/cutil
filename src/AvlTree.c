/*
Intrusive AVL tree container.
Copyright 2015-2018 Salvatore ISAJA. All rights reserved.

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

Thanks to Rosetta Code for providing useful insights on the implementation: 
https://rosettacode.org/wiki/AVL_tree
*/

/******************************************************************************
 * This file includes common code not dependent on element keys or values,
 * and must be linked with the program using the container.
 * To instantiate code dependent on element keys or values, see the comments
 * on the AvlTree structure.
 ******************************************************************************/
#include "AvlTree.h"

static inline AvlTree_Node *getParent(const AvlTree_Node *node) {
    return (AvlTree_Node *) (node->parent & ~3);
}

static inline void setParent(AvlTree_Node *node, AvlTree_Node *parent) {
    assert(((uintptr_t) parent & 3) == 0);
    node->parent = (node->parent & 3) | (uintptr_t) parent;
}

static inline int getBalance(const AvlTree_Node *node) {
    return node->parent & 3;
}

static inline void setBalance(AvlTree_Node *node, int balance) {
    node->parent = (node->parent & ~3) | balance;
}

static void rotateLeft(AvlTree *tree, AvlTree_Node *parent) {
    AvlTree_Node *child = parent->right;
    int parentBalance = AvlTree_balanced;
    int childBalance = AvlTree_balanced;
    if (getBalance(child) != AvlTree_rightHeavy) {
        parentBalance = AvlTree_rightHeavy;
        childBalance = AvlTree_leftHeavy;
    }
    child->parent = (uintptr_t) getParent(parent) | childBalance;
    parent->right = child->left;
    child->left = parent;
    parent->parent = (uintptr_t) child | parentBalance;
    if (parent->right != NULL) setParent(parent->right, parent);
    if (parent == tree->root) tree->root = child;
    else if (getParent(child)->right == parent) getParent(child)->right = child;
    else getParent(child)->left = child;
}

static void rotateRight(AvlTree *tree, AvlTree_Node *parent) {
    AvlTree_Node *child = parent->left;
    int parentBalance = AvlTree_balanced;
    int childBalance = AvlTree_balanced;
    if (getBalance(child) != AvlTree_leftHeavy) {
        parentBalance = AvlTree_leftHeavy;
        childBalance = AvlTree_rightHeavy;
    }
    child->parent = (uintptr_t) getParent(parent) | childBalance;
    parent->left = child->right;
    child->right = parent;
    parent->parent = (uintptr_t) child | parentBalance;
    if (parent->left != NULL) setParent(parent->left, parent);
    if (parent == tree->root) tree->root = child;
    else if (getParent(child)->left == parent) getParent(child)->left = child;
    else getParent(child)->right = child;
}

static void rotateLeftRight(AvlTree *tree, AvlTree_Node *parent) {
    AvlTree_Node *child = parent->left;
    AvlTree_Node *grandChild = child->right;
    int parentBalance = AvlTree_balanced;
    int childBalance = AvlTree_balanced;
    if (getBalance(grandChild) == AvlTree_rightHeavy) childBalance = AvlTree_leftHeavy;
    else if (getBalance(grandChild) == AvlTree_leftHeavy) parentBalance = AvlTree_rightHeavy;
    parent->left = grandChild->right;
    child->right = grandChild->left;
    grandChild->right = parent;
    grandChild->left = child;
    grandChild->parent = (uintptr_t) getParent(parent) | AvlTree_balanced;
    parent->parent = (uintptr_t) grandChild | parentBalance;
    child->parent = (uintptr_t) grandChild | childBalance;
    if (parent->left != NULL) setParent(parent->left, parent);
    if (child->right != NULL) setParent(child->right, child);
    if (parent == tree->root) tree->root = grandChild;
    else if (parent == getParent(grandChild)->left) getParent(grandChild)->left = grandChild;
    else getParent(grandChild)->right = grandChild;
}

static void rotateRightLeft(AvlTree *tree, AvlTree_Node *parent) {
    AvlTree_Node *child = parent->right;
    AvlTree_Node *grandChild = child->left;
    int parentBalance = AvlTree_balanced;
    int childBalance = AvlTree_balanced;
    if (getBalance(grandChild) == AvlTree_leftHeavy) childBalance = AvlTree_rightHeavy;
    else if (getBalance(grandChild) == AvlTree_rightHeavy) parentBalance = AvlTree_leftHeavy;
    parent->right = grandChild->left;
    child->left = grandChild->right;
    grandChild->left = parent;
    grandChild->right = child;
    grandChild->parent = (uintptr_t) getParent(parent) | AvlTree_balanced;
    parent->parent = (uintptr_t) grandChild | parentBalance;
    child->parent = (uintptr_t) grandChild | childBalance;
    if (parent->right != NULL) setParent(parent->right, parent);
    if (child->left != NULL) setParent(child->left, child);
    if (parent == tree->root) tree->root = grandChild;
    else if (parent == getParent(grandChild)->right) getParent(grandChild)->right = grandChild;
    else getParent(grandChild)->left = grandChild;
}

static inline AvlTree_Node *findMin(AvlTree_Node *root) {
    while (root->left != NULL) root = root->left;
    return root;
}

static inline AvlTree_Node *findMax(AvlTree_Node *root) {
    while (root->right != NULL) root = root->right;
    return root;
}

void AvlTree_postInsert(AvlTree *tree, AvlTree_Node *node) {
    // Balance
    while (node != tree->root) {
        AvlTree_Node *parent = getParent(node);
        int b = getBalance(parent);
        if (b == AvlTree_balanced) {
            setBalance(parent, (node == parent->left) ? AvlTree_leftHeavy : AvlTree_rightHeavy);
            node = parent;
        } else if (b == AvlTree_rightHeavy) {
            if (node == parent->left) {
                setBalance(parent, AvlTree_balanced);
            } else if (getBalance(node) == AvlTree_leftHeavy) {
                rotateRightLeft(tree, parent);
            } else {
                rotateLeft(tree, parent);
            }
            break;
        } else {
            assert(b == AvlTree_leftHeavy);
            if (node == parent->right) {
                setBalance(parent, AvlTree_balanced);
            } else if (getBalance(node) == AvlTree_rightHeavy) {
                rotateLeftRight(tree, parent);
            } else {
                rotateRight(tree, parent);
            }
            break;
        }
    }
}

void AvlTree_remove(AvlTree *tree, AvlTree_Node *node) {
    AvlTree_Node *successor = NULL; // becomes not null if node has two children
    AvlTree_Node *x = NULL;
    AvlTree_Node *xParent = NULL;
    if (node->left == NULL) x = node->right;
    else if (node->right == NULL) x = node->left;
    else {
        successor = findMin(node->right);
        x = successor->right;
    }
    if (successor != NULL) {
        assert(node != tree->leftmost);
        assert(node != tree->rightmost);
        setParent(node->left, successor);
        successor->left = node->left;
        if (successor != node->right) {
            xParent = getParent(successor);
            if (x != NULL) setParent(x, getParent(successor));
            getParent(successor)->left = x;
            successor->right = node->right;
            setParent(node->right, successor);
        } else {
            xParent = successor;
        }
        if (tree->root == node) tree->root = successor;
        else if (getParent(node)->left == node) getParent(node)->left = successor;
        else getParent(node)->right = successor;
        successor->parent = node->parent; // both same parent and balance factor
    } else {
        xParent = getParent(node);
        if (x != NULL) setParent(x, getParent(node));
        if (tree->root == node) tree->root = x;
        else if (getParent(node)->left == node) getParent(node)->left = x;
        else getParent(node)->right = x;
        if (tree->leftmost == node) {
            if (node->right == NULL) {
                assert(node->left == NULL);
                tree->leftmost = getParent(node);
            } else {
                tree->leftmost = findMin(x);
            }
        }
        if (tree->rightmost == node) {
            if (node->left == NULL) {
                assert(node->right == NULL);
                tree->rightmost = getParent(node);
            } else {
                tree->rightmost = findMax(x);
            }
        }
    }
    // Balance
    while (x != tree->root) {
        int b = getBalance(xParent);
        if (b == AvlTree_balanced) {
            setBalance(xParent, (x == xParent->right) ? AvlTree_leftHeavy : AvlTree_rightHeavy);
            return;
        } else if (b == AvlTree_leftHeavy) {
            if (x == xParent->left) {
                setBalance(xParent, AvlTree_balanced);
                x = xParent;
                xParent = getParent(x);
            } else {
                AvlTree_Node *a = xParent->left;
                assert(a != NULL);
                if (getBalance(a) == AvlTree_rightHeavy) {
                    assert(a->right != NULL);
                    rotateLeftRight(tree, xParent);
                    x = getParent(xParent);
                    xParent = getParent(x);
                } else {
                    rotateRight(tree, xParent);
                    x = getParent(xParent);
                    xParent = getParent(x);
                }
                if (getBalance(x) == AvlTree_rightHeavy) return;
            }
        } else {
            assert(b == AvlTree_rightHeavy);
            if (x == xParent->right) {
                setBalance(xParent, AvlTree_balanced);
                x = xParent;
                xParent = getParent(x);
            } else {
                AvlTree_Node *a = xParent->right;
                assert(a != NULL);
                if (getBalance(a) == AvlTree_leftHeavy) {
                    assert(a->left != NULL);
                    rotateRightLeft(tree, xParent);
                    x = getParent(xParent);
                    xParent = getParent(x);
                } else {
                    rotateLeft(tree, xParent);
                    x = getParent(xParent);
                    xParent = getParent(x);
                }
                if (getBalance(x) == AvlTree_leftHeavy) return;
            }
        }
    }
}
