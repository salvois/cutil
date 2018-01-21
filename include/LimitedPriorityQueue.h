/*
Generic intrusive O(1) priority queue for a limited number of priorities.
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
#include <string.h>

/**
 * Instantiates the header for a limited priority queue.
 * @param LimitedPriorityQueue name of the container to instantiate.
 * @param priorityCount one past the highest allowed value for priorities.
 */
#define LimitedPriorityQueue_header(LimitedPriorityQueue, priorityCount) \
\
typedef struct LimitedPriorityQueue##_Node LimitedPriorityQueue##_Node;\
\
struct LimitedPriorityQueue##_Node {\
    LimitedPriorityQueue##_Node *prev;\
    LimitedPriorityQueue##_Node *next;\
};\
\
typedef struct LimitedPriorityQueue {\
    LimitedPriorityQueue##_Node lists[priorityCount];\
    LimitedPriorityQueue##_Node *top;\
    uint32_t topmap;\
    uint32_t bitmap[priorityCount / 32];\
} LimitedPriorityQueue;\
\
static inline bool LimitedPriorityQueue##_isEmpty(const LimitedPriorityQueue *queue) { return queue->topmap == 0; }\
static LimitedPriorityQueue##_Node* LimitedPriorityQueue##_peek(const LimitedPriorityQueue *queue) { return queue->top; }\
void LimitedPriorityQueue##_initialize(LimitedPriorityQueue *queue);\
void LimitedPriorityQueue##_insertFront(LimitedPriorityQueue *queue, LimitedPriorityQueue##_Node *x);\
void LimitedPriorityQueue##_insert(LimitedPriorityQueue *queue, LimitedPriorityQueue##_Node *x);\
LimitedPriorityQueue##_Node *LimitedPriorityQueue_poll(LimitedPriorityQueue *queue);\
void LimitedPriorityQueue##_remove(LimitedPriorityQueue *queue, LimitedPriorityQueue##_Node *x);


/**
 * Instantiates the implementation for a limited priority queue.
 * @param LimitedPriorityQueue name of the container to instantiate.
 * @param priorityCount one past the highest allowed value for priorities.
 * @param Priority unsigned integral type for priorities.
 * @param getPriority function taking a pointer to a node and returning its priority.
 */
#define LimitedPriorityQueue_implementation(LimitedPriorityQueue, priorityCount, Priority, getPriority) \
\
/** Returns the index (between 0 and \a N - 1) of the first bit set, or -1 if none is set. */\
static int findFirstBitSet(const LimitedPriorityQueue *queue) {\
    assert(queue->topmap != 0);\
    int i = __builtin_ctz(queue->topmap);\
    assert(queue->bitmap[i] != 0);\
    return i * 32 + __builtin_ctz(queue->bitmap[i]);\
}\
\
void LimitedPriorityQueue##_initialize(LimitedPriorityQueue *queue) {\
    memset(queue, 0, sizeof(LimitedPriorityQueue));\
    for (size_t i = 0; i < priorityCount; i++) {\
        queue->lists[i].next = &queue->lists[i];\
        queue->lists[i].prev = &queue->lists[i];\
    }\
}\
\
/**\
 * Inserts an element as the first come element given its priority.\
 * The element must not be already in the queue.\
 */\
void LimitedPriorityQueue##_insertFront(LimitedPriorityQueue *queue, LimitedPriorityQueue##_Node *x) {\
    Priority key = getPriority(x);\
    assert(key < priorityCount);\
    LimitedPriorityQueue##_Node *l = &queue->lists[key];\
    x->prev = l;\
    x->next = l->next;\
    l->next->prev = x;\
    l->next = x;\
    queue->bitmap[key >> 5] |= 1 << (key & 0x1F);\
    queue->topmap |= 1 << (key >> 5);\
    if (queue->top == NULL || key <= getPriority(queue->top)) queue->top = x;\
}\
\
/**\
 * Inserts an element as the last come element given its priority.\
 * The element must not be already in the queue.\
 */\
void LimitedPriorityQueue##_insert(LimitedPriorityQueue *queue, LimitedPriorityQueue##_Node *x) {\
    Priority key = getPriority(x);\
    assert(key < priorityCount);\
    LimitedPriorityQueue##_Node *l = &queue->lists[key];\
    x->prev = l->prev;\
    x->next = l;\
    l->prev->next = x;\
    l->prev = x;\
    queue->bitmap[key >> 5] |= 1 << (key & 0x1F);\
    queue->topmap |= 1 << (key >> 5);\
    if (queue->top == NULL || key < getPriority(queue->top)) queue->top = x;\
}\
\
/**\
 * Removes the highest priority, first come element from the queue.\
 * Call only if not empty.\
 */\
LimitedPriorityQueue##_Node *LimitedPriorityQueue##_poll(LimitedPriorityQueue *queue) {\
    LimitedPriorityQueue##_Node *top = queue->top;\
    LimitedPriorityQueue##_remove(queue, top);\
    return top;\
}\
\
/**\
 * Removes the specified element from the queue.\
 * The element must be in the queue.\
 */\
void LimitedPriorityQueue##_remove(LimitedPriorityQueue *queue, LimitedPriorityQueue##_Node *x) {\
    assert(!LimitedPriorityQueue##_isEmpty(queue));\
    Priority key = getPriority(x);\
    assert(key < priorityCount);\
    LimitedPriorityQueue##_Node *l = &queue->lists[key];\
    x->next->prev = x->prev;\
    x->prev->next = x->next;\
    if (l->next == l) {\
        queue->bitmap[key >> 5] &= ~(1 << (key & 0x1F));\
        if (queue->bitmap[key >> 5] == 0) queue->topmap &= ~(1 << (key >> 5));\
    }\
    if (x == queue->top) {\
        if (queue->topmap != 0) {\
            int n = findFirstBitSet(queue);\
            queue->top = queue->lists[n].next;\
        } else {\
            queue->top = NULL;\
        }\
    }\
}
