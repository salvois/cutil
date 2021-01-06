/*
Unit tests for intrusive AVL tree container.
Copyright 2020 Salvatore ISAJA. All rights reserved.

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
#include "test.h"
#include "AvlTree.h"

typedef struct Value {
    int key;
    AvlTree_Node node;
} Value;

static inline Value *Value_fromNode(AvlTree_Node *n) {
    return (Value *) ((uint8_t *) n - offsetof(Value, node));
}

static inline bool Value_isLess(AvlTree_Node *node, AvlTree_Node *other) {
    return Value_fromNode(node)->key < Value_fromNode(other)->key;
}

AvlTree_instantiateInsert(AvlTreeTest_insert, Value_isLess);

static void assertTree(const char *func, int line, AvlTree *tree, Value *root, Value *leftmost, Value *rightmost) {
    ASSERTN(func, line, tree->root == (root != NULL ? &root->node : NULL));
    ASSERTN(func, line, tree->leftmost == (leftmost != NULL ? &leftmost->node : NULL));
    ASSERTN(func, line, tree->rightmost == (rightmost != NULL ? &rightmost->node : NULL));
}

static void assertNode(const char *func, int line, Value *value, Value* parent, Value* left, Value* right, enum AvlTree_Balance balance) {
    ASSERTN(func, line, AvlTree_getParent(&value->node) == (parent != NULL ? &parent->node : NULL));
    ASSERTN(func, line, AvlTree_getBalance(&value->node) == balance);
    ASSERTN(func, line, value->node.left == (left != NULL ? &left->node : NULL));
    ASSERTN(func, line, value->node.right == (right != NULL ? &right->node : NULL));
}

#define ASSERT_TREE(tree, root, leftmost, rightmost) assertTree(__func__, __LINE__, tree, root, leftmost, rightmost)
#define ASSERT_NODE(value, parent, left, right, balance) assertNode(__func__, __LINE__, value, parent, left, right, balance)

static void AvlTreeTest_initialize() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    
    ASSERT_TREE(&tree, NULL, NULL, NULL);
}

static void AvlTreeTest_insertOne() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v13 = { .key = 13 };
    
    AvlTreeTest_insert(&tree, &v13.node);
    
    //  13
    ASSERT_TREE(&tree, &v13, &v13, &v13);
    ASSERT_NODE(&v13, NULL, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_insertTwoGreater() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    
    AvlTreeTest_insert(&tree, &v13.node);
    AvlTreeTest_insert(&tree, &v14.node);
    
    //  13
    //    14
    ASSERT_TREE(&tree, &v13, &v13, &v14);
    ASSERT_NODE(&v13, NULL, NULL, &v14, AvlTree_rightHeavy);
    ASSERT_NODE(&v14, &v13, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_insertTwoLess() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v12 = { .key = 12 };
    Value v13 = { .key = 13 };
    
    AvlTreeTest_insert(&tree, &v13.node);
    AvlTreeTest_insert(&tree, &v12.node);
    
    //    13
    //  12
    ASSERT_TREE(&tree, &v13, &v12, &v13);
    ASSERT_NODE(&v13, NULL, &v12, NULL, AvlTree_leftHeavy);
    ASSERT_NODE(&v12, &v13, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_insertThreeWithLeftRotation() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    
    AvlTreeTest_insert(&tree, &v13.node);
    AvlTreeTest_insert(&tree, &v14.node);
    AvlTreeTest_insert(&tree, &v15.node);
    
    //    14
    //  13  15
    ASSERT_TREE(&tree, &v14, &v13, &v15);
    ASSERT_NODE(&v14, NULL, &v13, &v15, AvlTree_balanced);
    ASSERT_NODE(&v13, &v14, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v15, &v14, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_insertThreeWithRightRotation() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    
    AvlTreeTest_insert(&tree, &v15.node);
    AvlTreeTest_insert(&tree, &v14.node);
    AvlTreeTest_insert(&tree, &v13.node);
    
    //    14
    //  13  15
    ASSERT_TREE(&tree, &v14, &v13, &v15);
    ASSERT_NODE(&v14, NULL, &v13, &v15, AvlTree_balanced);
    ASSERT_NODE(&v13, &v14, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v15, &v14, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_insertThreeWithRightLeftRotation() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    
    AvlTreeTest_insert(&tree, &v13.node);
    AvlTreeTest_insert(&tree, &v15.node);
    AvlTreeTest_insert(&tree, &v14.node);
    
    //    14
    //  13  15
    ASSERT_TREE(&tree, &v14, &v13, &v15);
    ASSERT_NODE(&v14, NULL, &v13, &v15, AvlTree_balanced);
    ASSERT_NODE(&v13, &v14, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v15, &v14, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_insertThreeWithLeftRightRotation() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    
    AvlTreeTest_insert(&tree, &v15.node);
    AvlTreeTest_insert(&tree, &v13.node);
    AvlTreeTest_insert(&tree, &v14.node);
    
    //    14
    //  13  15
    ASSERT_TREE(&tree, &v14, &v13, &v15);
    ASSERT_NODE(&v14, NULL, &v13, &v15, AvlTree_balanced);
    ASSERT_NODE(&v13, &v14, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v15, &v14, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_complexInsert() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v1  = { .key = 1 };
    Value v8  = { .key = 8 };
    Value v9  = { .key = 9 };
    Value v11 = { .key = 11 };
    Value v12 = { .key = 12 };
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    Value v16 = { .key = 16 };
    Value v17 = { .key = 17 };
    
    AvlTreeTest_insert(&tree, &v13.node);
    AvlTreeTest_insert(&tree, &v14.node);
    AvlTreeTest_insert(&tree, &v15.node);
    AvlTreeTest_insert(&tree, &v12.node);
    AvlTreeTest_insert(&tree, &v11.node);
    AvlTreeTest_insert(&tree, &v17.node);
    AvlTreeTest_insert(&tree, &v16.node);
    AvlTreeTest_insert(&tree, &v8.node);
    AvlTreeTest_insert(&tree, &v9.node);
    AvlTreeTest_insert(&tree, &v1.node);
    
    //               14
    //        9             16
    //    8      12       15  17
    //   1     11  13
    ASSERT_TREE(&tree, &v14, &v1, &v17);
    ASSERT_NODE(&v14, NULL, &v9,  &v16, AvlTree_leftHeavy);
    ASSERT_NODE(&v9,  &v14, &v8,  &v12, AvlTree_balanced);
    ASSERT_NODE(&v16, &v14, &v15, &v17, AvlTree_balanced);
    ASSERT_NODE(&v8,  &v9,  &v1,  NULL, AvlTree_leftHeavy);
    ASSERT_NODE(&v12, &v9,  &v11, &v13, AvlTree_balanced);
    ASSERT_NODE(&v15, &v16, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v17, &v16, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v1,  &v8,  NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v11, &v12, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v13, &v12, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_removeNodeWithoutChildren() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v11 = { .key = 11 };
    Value v12 = { .key = 12 };
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    AvlTreeTest_insert(&tree, &v15.node);
    AvlTreeTest_insert(&tree, &v14.node);
    AvlTreeTest_insert(&tree, &v13.node);
    AvlTreeTest_insert(&tree, &v12.node);
    AvlTreeTest_insert(&tree, &v11.node);
    //           14
    //     12         15
    //   11  13

    AvlTree_remove(&tree, &v11.node);
    
    //           14
    //     12         15
    //       13
    ASSERT_TREE(&tree, &v14, &v12, &v15);
    ASSERT_NODE(&v14, NULL, &v12, &v15, AvlTree_leftHeavy);
    ASSERT_NODE(&v12, &v14, NULL, &v13, AvlTree_rightHeavy);
    ASSERT_NODE(&v15, &v14, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v13, &v12, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_removeNodeWithLeftChildOnly() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v11 = { .key = 11 };
    Value v12 = { .key = 12 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    AvlTreeTest_insert(&tree, &v15.node);
    AvlTreeTest_insert(&tree, &v14.node);
    AvlTreeTest_insert(&tree, &v12.node);
    AvlTreeTest_insert(&tree, &v11.node);
    //           14
    //     12         15
    //   11

    AvlTree_remove(&tree, &v12.node);
    
    //    14
    //  11  15
    ASSERT_TREE(&tree, &v14, &v11, &v15);
    ASSERT_NODE(&v14, NULL, &v11, &v15, AvlTree_balanced);
    ASSERT_NODE(&v11, &v14, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v15, &v14, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_removeNodeWithRightChildOnly() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v12 = { .key = 12 };
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    AvlTreeTest_insert(&tree, &v15.node);
    AvlTreeTest_insert(&tree, &v14.node);
    AvlTreeTest_insert(&tree, &v12.node);
    AvlTreeTest_insert(&tree, &v13.node);
    //           14
    //     12         15
    //       13

    AvlTree_remove(&tree, &v12.node);
    
    //    14
    //  13  15
    ASSERT_TREE(&tree, &v14, &v13, &v15);
    ASSERT_NODE(&v14, NULL, &v13, &v15, AvlTree_balanced);
    ASSERT_NODE(&v13, &v14, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v15, &v14, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_removeNodeWithBothChildren() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v11 = { .key = 11 };
    Value v12 = { .key = 12 };
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    AvlTreeTest_insert(&tree, &v15.node);
    AvlTreeTest_insert(&tree, &v14.node);
    AvlTreeTest_insert(&tree, &v13.node);
    AvlTreeTest_insert(&tree, &v12.node);
    AvlTreeTest_insert(&tree, &v11.node);
    //           14
    //     12         15
    //   11  13

    AvlTree_remove(&tree, &v12.node);
    
    //           14
    //     13         15
    //   11
    ASSERT_TREE(&tree, &v14, &v11, &v15);
    ASSERT_NODE(&v14, NULL, &v13, &v15, AvlTree_leftHeavy);
    ASSERT_NODE(&v13, &v14, &v11, NULL, AvlTree_leftHeavy);
    ASSERT_NODE(&v15, &v14, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v11, &v13, NULL, NULL, AvlTree_balanced);
}

static void AvlTreeTest_removeNodeWithDeepChildren() {
    AvlTree tree;
    AvlTree_initialize(&tree);
    Value v11 = { .key = 11 };
    Value v12 = { .key = 12 };
    Value v13 = { .key = 13 };
    Value v14 = { .key = 14 };
    Value v15 = { .key = 15 };
    AvlTreeTest_insert(&tree, &v15.node);
    AvlTreeTest_insert(&tree, &v14.node);
    AvlTreeTest_insert(&tree, &v13.node);
    AvlTreeTest_insert(&tree, &v12.node);
    AvlTreeTest_insert(&tree, &v11.node);
    //           14
    //     12         15
    //   11  13

    AvlTree_remove(&tree, &v14.node);
    
    //           12
    //     11         15
    //              13
    ASSERT_TREE(&tree, &v12, &v11, &v15);
    ASSERT_NODE(&v12, NULL, &v11, &v15, AvlTree_rightHeavy);
    ASSERT_NODE(&v11, &v12, NULL, NULL, AvlTree_balanced);
    ASSERT_NODE(&v15, &v12, &v13, NULL, AvlTree_leftHeavy);
    ASSERT_NODE(&v13, &v15, NULL, NULL, AvlTree_balanced);
}

void AvlTreeTest_run() {
    RUN_TEST(AvlTreeTest_initialize);
    RUN_TEST(AvlTreeTest_insertOne);
    RUN_TEST(AvlTreeTest_insertTwoGreater);
    RUN_TEST(AvlTreeTest_insertTwoLess);
    RUN_TEST(AvlTreeTest_insertThreeWithLeftRotation);
    RUN_TEST(AvlTreeTest_insertThreeWithRightRotation);
    RUN_TEST(AvlTreeTest_insertThreeWithRightLeftRotation);
    RUN_TEST(AvlTreeTest_insertThreeWithLeftRightRotation);
    RUN_TEST(AvlTreeTest_complexInsert);
    RUN_TEST(AvlTreeTest_removeNodeWithoutChildren);
    RUN_TEST(AvlTreeTest_removeNodeWithLeftChildOnly);
    RUN_TEST(AvlTreeTest_removeNodeWithRightChildOnly);
    RUN_TEST(AvlTreeTest_removeNodeWithBothChildren);
    RUN_TEST(AvlTreeTest_removeNodeWithDeepChildren);
}
