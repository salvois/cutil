/*
Generic intrusive O(n) priority queue based on an unordered doubly linked list.
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
 * Instantiates the header for a priority queue based on an unordered linked list.
 * @param UnorderedListPriorityQueue name of the container to instantiate.
 */
#define UnorderedListPriorityQueue_header(UnorderedListPriorityQueue) \
\
typedef struct UnorderedListPriorityQueue##_Node UnorderedListPriorityQueue##_Node;\
\
struct UnorderedListPriorityQueue##_Node {\
    UnorderedListPriorityQueue##_Node *prev;\
    UnorderedListPriorityQueue##_Node *next;\
};\
\
typedef struct UnorderedListPriorityQueue {\
    UnorderedListPriorityQueue##_Node sentinel;\
    UnorderedListPriorityQueue##_Node *min;\
} UnorderedListPriorityQueue;\
\
void UnorderedListPriorityQueue##_initialize(UnorderedListPriorityQueue *queue) {\
    queue->sentinel.prev = &queue->sentinel;\
    queue->sentinel.next = &queue->sentinel;\
    queue->min = NULL;\
}\
\
static inline bool UnorderedListPriorityQueue##_isEmpty(const UnorderedListPriorityQueue *queue) {\
    return queue->sentinel.next == &queue->sentinel;\
}\
\
void UnorderedListPriorityQueue##_insert(UnorderedListPriorityQueue *queue, UnorderedListPriorityQueue##_Node *node);\
void UnorderedListPriorityQueue##_insertFront(UnorderedListPriorityQueue *queue, UnorderedListPriorityQueue##_Node *node);\
UnorderedListPriorityQueue##_Node *UnorderedListPriorityQueue##_poll(UnorderedListPriorityQueue *queue);\
void UnorderedListPriorityQueue##_remove(UnorderedListPriorityQueue *queue, UnorderedListPriorityQueue##_Node *node);


/**
 * Instantiates the implementation for a priority queue based on an unordered linked list.
 * @param UnorderedListPriorityQueue name of the container to instantiate.
 * @param isLess name of the function comparing nodes having the following prototype: bool isLess(Node *node, Node *other)
 */
#define UnorderedListPriorityQueue_implementation(UnorderedListPriorityQueue, isLess) \
\
static void updateMin(UnorderedListPriorityQueue *queue) {\
    UnorderedListPriorityQueue##_Node *min = NULL;\
    for (UnorderedListPriorityQueue##_Node *i = queue->sentinel.next; i != &queue->sentinel; i = i->next) {\
        if (min == NULL || isLess(i, min)) min = i;\
    }\
    queue->min = min;\
}\
\
/** Inserts the specified node into the queue, after nodes with the same priority. */\
void UnorderedListPriorityQueue##_insert(UnorderedListPriorityQueue *queue, UnorderedListPriorityQueue##_Node *node) {\
    node->next = &queue->sentinel;\
    node->prev = queue->sentinel.prev;\
    queue->sentinel.prev->next = node;\
    queue->sentinel.prev = node;\
    if (queue->min != NULL && isLess(node, queue->min)) queue->min = node;\
}\
\
/** Inserts the specified node into the queue, before nodes with the same priority. */\
void UnorderedListPriorityQueue##_insertFront(UnorderedListPriorityQueue *queue, UnorderedListPriorityQueue##_Node *node) {\
    node->prev = &queue->sentinel;\
    node->next = queue->sentinel.next;\
    queue->sentinel.next->prev = node;\
    queue->sentinel.next = node;\
    if (queue->min != NULL && !isLess(queue->min, node)) queue->min = node;\
}\
\
/** Returns the node for the minimum element without removing it from the queue. */\
UnorderedListPriorityQueue##_Node *UnorderedListPriorityQueue##_peek(UnorderedListPriorityQueue *queue) {\
    if (queue->min == NULL) updateMin(queue);\
    return queue->min;\
}\
\
/** Removes the node for the minimum element from the queue. */\
UnorderedListPriorityQueue##_Node *UnorderedListPriorityQueue##_poll(UnorderedListPriorityQueue *queue) {\
    if (queue->min == NULL) updateMin(queue);\
    UnorderedListPriorityQueue##_Node *result = queue->min;\
    result->prev->next = result->next;\
    result->next->prev = result->prev;\
    queue->min = NULL;\
    return result;\
}\
\
/** Removes the specified node from the queue. */\
void UnorderedListPriorityQueue##_remove(UnorderedListPriorityQueue *queue, UnorderedListPriorityQueue##_Node *node) {\
    node->prev->next = node->next;\
    node->next->prev = node->prev;\
    if (node == queue->min) queue->min = NULL;\
}
