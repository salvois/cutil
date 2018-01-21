/*
Generic intrusive O(n) priority queue based on an ordered doubly linked list.
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
#include <stdbool.h>

/**
 * Instantiates the header for a priority queue based on an ordered linked list.
 * @param OrderedListPriorityQueue name of the container to instantiate.
 */
#define OrderedListPriorityQueue_header(OrderedListPriorityQueue) \
\
typedef struct OrderedListPriorityQueue##_Node OrderedListPriorityQueue##_Node;\
\
struct OrderedListPriorityQueue##_Node {\
    OrderedListPriorityQueue##_Node *prev;\
    OrderedListPriorityQueue##_Node *next;\
};\
\
typedef struct OrderedListPriorityQueue {\
    OrderedListPriorityQueue##_Node sentinel;\
} OrderedListPriorityQueue;\
\
void OrderedListPriorityQueue##_initialize(OrderedListPriorityQueue *queue) {\
    queue->sentinel.prev = &queue->sentinel;\
    queue->sentinel.next = &queue->sentinel;\
}\
\
static inline bool OrderedListPriorityQueue##_isEmpty(const OrderedListPriorityQueue *queue) {\
    return queue->sentinel.next == &queue->sentinel;\
}\
\
static inline OrderedListPriorityQueue##_Node *OrderedListPriorityQueue##_peek(const OrderedListPriorityQueue *queue) {\
    return queue->sentinel.next;\
}\
void OrderedListPriorityQueue##_insert(OrderedListPriorityQueue *queue, OrderedListPriorityQueue##_Node *node);\
void OrderedListPriorityQueue##_insertFront(OrderedListPriorityQueue *queue, OrderedListPriorityQueue##_Node *node);\
OrderedListPriorityQueue##_Node *OrderedListPriorityQueue##_poll(OrderedListPriorityQueue *queue);\
void OrderedListPriorityQueue##_remove(OrderedListPriorityQueue *queue, OrderedListPriorityQueue##_Node *node);

/**
 * Instantiates the implementation for a priority queue based on an ordered linked list.
 * @param OrderedListPriorityQueue name of the container to instantiate.
 * @param isLess name of the function comparing nodes having the following prototype: bool isLess(Node *node, Node *other)
 */
#define OrderedListPriorityQueue_implementation(OrderedListPriorityQueue, isLess) \
\
/** Inserts the specified node into the queue, after nodes with the same priority. */\
void OrderedListPriorityQueue##_insert(OrderedListPriorityQueue *queue, OrderedListPriorityQueue##_Node *node) {\
    OrderedListPriorityQueue##_Node *i = queue->sentinel.next;\
    while (i != &queue->sentinel && !isLess(node, i)) i = i->next;\
    node->prev = i->prev;\
    node->next = i;\
    i->prev->next = node;\
    i->prev = node;\
}\
\
/** Inserts the specified node into the queue, before nodes with the same priority. */\
void OrderedListPriorityQueue##_insertFront(OrderedListPriorityQueue *queue, OrderedListPriorityQueue##_Node *node) {\
    OrderedListPriorityQueue##_Node *i = queue->sentinel.next;\
    while (i != &queue->sentinel && isLess(i, node)) i = i->next;\
    node->prev = i->prev;\
    node->next = i;\
    i->prev->next = node;\
    i->prev = node;\
}\
\
/** Removes the node for the minimum element from the queue. */\
OrderedListPriorityQueue##_Node *OrderedListPriorityQueue##_poll(OrderedListPriorityQueue *queue) {\
    OrderedListPriorityQueue##_Node *result = queue->sentinel.next;\
    result->prev->next = result->next;\
    result->next->prev = result->prev;\
    return result;\
}\
\
/** Removes the specified node from the queue. */\
void OrderedListPriorityQueue##_remove(OrderedListPriorityQueue *queue, OrderedListPriorityQueue##_Node *node) {\
    node->prev->next = node->next;\
    node->next->prev = node->prev;\
}
