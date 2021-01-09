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

static inline void setParent(AvlTree_Node *node, AvlTree_Node *parent) {
    assert(((uintptr_t) parent & 3) == 0);
    node->parent = (node->parent & 3) | (uintptr_t) parent;
}

static inline void setBalance(AvlTree_Node *node, int balance) {
    node->parent = (node->parent & ~3) | balance;
}

static void rotateLeft(AvlTree_Node *parent) {
    AvlTree_Node *child = parent->right;
    int parentBalance = AvlTree_balanced;
    int childBalance = AvlTree_balanced;
    if (AvlTree_getBalance(child) != AvlTree_rightHeavy) {
        parentBalance = AvlTree_rightHeavy;
        childBalance = AvlTree_leftHeavy;
    }
    child->parent = (uintptr_t) AvlTree_getParent(parent) | childBalance;
    parent->right = child->left;
    child->left = parent;
    parent->parent = (uintptr_t) child | parentBalance;
    setParent(parent->right, parent);
    if (AvlTree_getParent(child)->right == parent) AvlTree_getParent(child)->right = child;
    else AvlTree_getParent(child)->left = child;
}

static void rotateRight(AvlTree_Node *parent) {
    AvlTree_Node *child = parent->left;
    int parentBalance = AvlTree_balanced;
    int childBalance = AvlTree_balanced;
    if (AvlTree_getBalance(child) != AvlTree_leftHeavy) {
        parentBalance = AvlTree_leftHeavy;
        childBalance = AvlTree_rightHeavy;
    }
    child->parent = (uintptr_t) AvlTree_getParent(parent) | childBalance;
    parent->left = child->right;
    child->right = parent;
    parent->parent = (uintptr_t) child | parentBalance;
    setParent(parent->left, parent);
    if (AvlTree_getParent(child)->left == parent) AvlTree_getParent(child)->left = child;
    else AvlTree_getParent(child)->right = child;
}

static void rotateLeftRight(AvlTree_Node *parent) {
    AvlTree_Node *child = parent->left;
    AvlTree_Node *grandChild = child->right;
    int parentBalance = AvlTree_balanced;
    int childBalance = AvlTree_balanced;
    if (AvlTree_getBalance(grandChild) == AvlTree_rightHeavy) childBalance = AvlTree_leftHeavy;
    else if (AvlTree_getBalance(grandChild) == AvlTree_leftHeavy) parentBalance = AvlTree_rightHeavy;
    parent->left = grandChild->right;
    child->right = grandChild->left;
    grandChild->right = parent;
    grandChild->left = child;
    grandChild->parent = (uintptr_t) AvlTree_getParent(parent) | AvlTree_balanced;
    parent->parent = (uintptr_t) grandChild | parentBalance;
    child->parent = (uintptr_t) grandChild | childBalance;
    setParent(parent->left, parent);
    setParent(child->right, child);
    if (parent == AvlTree_getParent(grandChild)->left) AvlTree_getParent(grandChild)->left = grandChild;
    else AvlTree_getParent(grandChild)->right = grandChild;
}

static void rotateRightLeft(AvlTree_Node *parent) {
    AvlTree_Node *child = parent->right;
    AvlTree_Node *grandChild = child->left;
    int parentBalance = AvlTree_balanced;
    int childBalance = AvlTree_balanced;
    if (AvlTree_getBalance(grandChild) == AvlTree_leftHeavy) childBalance = AvlTree_rightHeavy;
    else if (AvlTree_getBalance(grandChild) == AvlTree_rightHeavy) parentBalance = AvlTree_leftHeavy;
    parent->right = grandChild->left;
    child->left = grandChild->right;
    grandChild->left = parent;
    grandChild->right = child;
    grandChild->parent = (uintptr_t) AvlTree_getParent(parent) | AvlTree_balanced;
    parent->parent = (uintptr_t) grandChild | parentBalance;
    child->parent = (uintptr_t) grandChild | childBalance;
    setParent(parent->right, parent);
    setParent(child->left, child);
    if (parent == AvlTree_getParent(grandChild)->right) AvlTree_getParent(grandChild)->right = grandChild;
    else AvlTree_getParent(grandChild)->left = grandChild;
}

static inline AvlTree_Node *findMin(const AvlTree_Node *sentinel, AvlTree_Node *root) {
    while (root->left != sentinel) root = root->left;
    return root;
}

static inline AvlTree_Node *findMax(const AvlTree_Node *sentinel, AvlTree_Node *root) {
    while (root->right != sentinel) root = root->right;
    return root;
}

void AvlTree_rebalanceAfterInsertion(AvlTree *tree, AvlTree_Node *node) {
    while (node != tree->sentinel.left) {
        AvlTree_Node *parent = AvlTree_getParent(node);
        int balance = AvlTree_getBalance(parent);
        if (balance == AvlTree_balanced) {
            setBalance(parent, (node == parent->left) ? AvlTree_leftHeavy : AvlTree_rightHeavy);
            node = parent;
        } else if (balance == AvlTree_rightHeavy) {
            if (node == parent->left) {
                setBalance(parent, AvlTree_balanced);
            } else if (AvlTree_getBalance(node) == AvlTree_leftHeavy) {
                rotateRightLeft(parent);
            } else {
                rotateLeft(parent);
            }
            break;
        } else {
            assert(balance == AvlTree_leftHeavy);
            if (node == parent->right) {
                setBalance(parent, AvlTree_balanced);
            } else if (AvlTree_getBalance(node) == AvlTree_rightHeavy) {
                rotateLeftRight(parent);
            } else {
                rotateRight(parent);
            }
            break;
        }
    }
}

static void rebalanceAfterDeletion(AvlTree *tree, AvlTree_Node *current, AvlTree_Node *parent) {
    while (current != tree->sentinel.left) {
        int balance = AvlTree_getBalance(parent);
        if (balance == AvlTree_balanced) {
            setBalance(parent, (current == parent->right) ? AvlTree_leftHeavy : AvlTree_rightHeavy);
            break;
        } else if (balance == AvlTree_leftHeavy) {
            if (current == parent->left) {
                setBalance(parent, AvlTree_balanced);
                current = parent;
            } else {
                AvlTree_Node *sibling = parent->left;
                assert(sibling != &tree->sentinel);
                if (AvlTree_getBalance(sibling) == AvlTree_rightHeavy) {
                    assert(sibling->right != &tree->sentinel);
                    rotateLeftRight(parent);
                } else {
                    rotateRight(parent);
                }
                current = AvlTree_getParent(parent);
                if (AvlTree_getBalance(current) == AvlTree_rightHeavy) break;
            }
        } else {
            assert(balance == AvlTree_rightHeavy);
            if (current == parent->right) {
                setBalance(parent, AvlTree_balanced);
                current = parent;
            } else {
                AvlTree_Node *sibling = parent->right;
                assert(sibling != &tree->sentinel);
                if (AvlTree_getBalance(sibling) == AvlTree_leftHeavy) {
                    assert(sibling->left != &tree->sentinel);
                    rotateRightLeft(parent);
                } else {
                    rotateLeft(parent);
                }
                current = AvlTree_getParent(parent);
                if (AvlTree_getBalance(current) == AvlTree_leftHeavy) break;
            }
        }
        parent = AvlTree_getParent(current);
    }    
}

void AvlTree_initialize(AvlTree *tree) {
    tree->sentinel.parent = 0;
    tree->sentinel.left = &tree->sentinel;
    tree->sentinel.right = &tree->sentinel;
    tree->leftmost = &tree->sentinel;
    tree->rightmost = &tree->sentinel;
}

void AvlTree_remove(AvlTree *tree, AvlTree_Node *node) {
    if (node->left == &tree->sentinel || node->right == &tree->sentinel) {
        AvlTree_Node *parent = AvlTree_getParent(node);
        AvlTree_Node *replacement = (node->left != &tree->sentinel) ? node->left : node->right;
        setParent(replacement, parent);
        if (parent->left == node) parent->left = replacement;
        else parent->right = replacement;
        if (tree->leftmost == node) {
            if (node->right == &tree->sentinel) {
                assert(node->left == &tree->sentinel);
                tree->leftmost = parent;
            } else {
                tree->leftmost = findMin(&tree->sentinel, replacement);
            }
        }
        if (tree->rightmost == node) {
            if (node->left == &tree->sentinel) {
                assert(node->right == &tree->sentinel);
                tree->rightmost = parent;
            } else {
                tree->rightmost = findMax(&tree->sentinel, replacement);
            }
        }
        rebalanceAfterDeletion(tree, replacement, parent);
    } else {
        assert(node != tree->leftmost);
        assert(node != tree->rightmost);
        AvlTree_Node *successor = findMin(&tree->sentinel, node->right);
        AvlTree_Node *replacement = successor->right;
        AvlTree_Node *replacementParent;
        setParent(node->left, successor);
        successor->left = node->left;
        if (successor != node->right) {
            replacementParent = AvlTree_getParent(successor);
            AvlTree_Node* successorParent = AvlTree_getParent(successor);
            setParent(replacement, successorParent);
            successorParent->left = replacement;
            successor->right = node->right;
            setParent(node->right, successor);
        } else {
            replacementParent = successor;
        }
        AvlTree_Node* parent = AvlTree_getParent(node);
        successor->parent = node->parent; // both same parent and balance factor
        if (parent->left == node) parent->left = successor;
        else parent->right = successor;
        rebalanceAfterDeletion(tree, replacement, replacementParent);
    }
}
