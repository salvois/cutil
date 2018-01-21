/*
Test code for the intrusive binary min-heap container.
Copyright 2012-2018 Salvatore ISAJA. All rights reserved.

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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include "IntrusiveBinaryHeap.h"
#include "tscStopwatch.h"

IntrusiveBinaryHeap_header(TestHeap);

typedef struct Value {
    uint64_t key;
    TestHeap_Node node;
    char dummy[64 - sizeof(TestHeap_Node) - sizeof(uint64_t)];
} Value;

static inline Value *Value_fromNode(TestHeap_Node *node) {
    return (Value *) ((uint8_t *) node - offsetof(Value, node));
}

static bool Value_isLess(TestHeap_Node *node, TestHeap_Node *other) {
    Value *v1 = Value_fromNode(node);
    Value *v2 = Value_fromNode(other);
    return v1->key < v2->key;
}

IntrusiveBinaryHeap_implementation(TestHeap, Value_isLess);
#ifndef NDEBUG
IntrusiveBinaryHeap_debugImplementation(TestHeap, Value_isLess);
#endif

static uint64_t nextKey = 0;

static void randomizeKey(Value *value) {
    value->key = ((uint64_t) lrand48() << 32) | lrand48();
    //value->key = nextKey++;
}

static Value *createValues(size_t nodeCount) {
    Value *values = malloc(nodeCount * sizeof(Value));
    memset(values, 0, nodeCount * sizeof(Value));
    srand48(time(NULL));
    for (size_t i = 0; i < nodeCount; ++i) {
        randomizeKey(&values[i]);
    }
    values[lrand48() % nodeCount].key = 0;
    values[lrand48() % nodeCount].key = 1;
    values[lrand48() % nodeCount].key = UINT64_MAX - 1;
    values[lrand48() % nodeCount].key = UINT64_MAX;
    return values;
}

#ifndef NDEBUG
static bool isPresent(Value **arr, size_t size, Value *node) {
    for (size_t i = 0; i < size; ++i) {
        if (arr[i] == node) return true;
    }
    return false;
}

static void testConsistency(size_t nodeCount) {
    Value **seenValues = malloc(nodeCount * sizeof(Value *));
    size_t seenValuesSize;
    Value *values = createValues(nodeCount);
    TestHeap heap;
    TestHeap_initialize(&heap);
    // Test minimum element removal
    for (size_t i = 0; i < nodeCount; ++i) {
        TestHeap_insert(&heap, &values[i].node);
        TestHeap_check(&heap);
    }
    seenValuesSize = 0;
    for (size_t i = 0; i < nodeCount; ++i) {
        assert(!TestHeap_isEmpty(&heap));
        Value *value = Value_fromNode(TestHeap_poll(&heap));
        TestHeap_check(&heap);
        assert(!isPresent(seenValues, seenValuesSize, value));
        seenValues[seenValuesSize] = value;
        seenValuesSize++;
        printf("Polled %zu: %016" PRIX64 "\n", i, value->key);
        assert(i == 0 || seenValues[i - 1]->key <= seenValues[i]->key);
    }
    assert(TestHeap_isEmpty(&heap));
    // Test random removal
    for (size_t i = 0; i < nodeCount; ++i) {
        TestHeap_insert(&heap, &values[i].node);
        TestHeap_check(&heap);
    }
    seenValuesSize = 0;
    for (size_t i = 0; i < nodeCount; ++i) {
        assert(!TestHeap_isEmpty(&heap));
        TestHeap_remove(&heap, &values[i].node);
        TestHeap_check(&heap);
        assert(!isPresent(seenValues, seenValuesSize, &values[i]));
        seenValues[seenValuesSize] = &values[i];
        seenValuesSize++;
        printf("Removed %zu: %016" PRIX64 "\n", i, values[i].key);
    }
    assert(TestHeap_isEmpty(&heap));
    // Test combined poll and insert
    for (size_t i = 0; i < nodeCount / 2; ++i) {
        TestHeap_insert(&heap, &values[i].node);
        TestHeap_check(&heap);
    }
    seenValuesSize = 0;
    for (size_t i = 0; i < nodeCount / 2; ++i) {
        assert(!TestHeap_isEmpty(&heap));
        Value *value = Value_fromNode(TestHeap_pollAndInsert(&heap, &values[i + nodeCount / 2].node));
        TestHeap_check(&heap);
        assert(!isPresent(seenValues, seenValuesSize, &values[i]));
        seenValues[seenValuesSize] = &values[i];
        seenValuesSize++;
        printf("Removed %zu: %016" PRIX64 "\tInserted %016" PRIX64 "\n", i, value->key, values[i].key);
    }
    for (size_t i = 0; i < nodeCount / 2; ++i) {
        TestHeap_poll(&heap);
    }
    assert(TestHeap_isEmpty(&heap));
    free(seenValues);
    free(values);
}
#endif

static void testRandomRemovalPerformance(size_t nodeCount, size_t roundCount) {
    Value *values = createValues(nodeCount);
    TestHeap heap;
    TestHeap_initialize(&heap);
    for (size_t i = 0; i < nodeCount - 1; ++i) {
        TestHeap_insert(&heap, &values[i].node);
    }
    double insertMean = 0;
    double removeMean = 0;
    double insertVar = 0;
    double removeVar = 0;
    for (size_t r = 0; r < roundCount; ++r) {
        // Insert
        size_t i = nodeCount - 1;
        randomizeKey(&values[i]);
        uint64_t tb = tscStopwatchBegin();
        TestHeap_insert(&heap, &values[i].node);
        uint64_t te = tscStopwatchEnd();
        double delta = (double) (te - tb) - insertMean;
        insertMean += delta / (double) (r + 1);
        double delta2 = (double) (te - tb) - insertMean;
        insertVar += delta * delta2;
        // Remove node just inserted
        tb = tscStopwatchBegin();
        TestHeap_remove(&heap, &values[i].node);
        te = tscStopwatchEnd();
        delta = (double) (te - tb) - removeMean;
        removeMean += delta / (double) (r + 1);
        delta2 = (double) (te - tb) - removeMean;
        removeVar += delta * delta2;
    }
    printf("%zu,%g,%g,%g,%g\n", nodeCount, insertMean, removeMean, sqrt(insertVar / (roundCount - 1)), sqrt(removeVar / (roundCount - 1)));
    free(values);
}

static void testMinimumRemovalPerformance(size_t nodeCount, size_t roundCount) {
    Value *values = createValues(nodeCount);
    TestHeap heap;
    TestHeap_initialize(&heap);
    for (size_t i = 0; i < nodeCount - 1; ++i) {
        TestHeap_insert(&heap, &values[i].node);
    }
    Value *value = &values[nodeCount - 1];
    double insertMean = 0;
    double removeMean = 0;
    double insertVar = 0;
    double removeVar = 0;
    for (size_t r = 0; r < roundCount; ++r) {
        // Insert
        randomizeKey(value);
        uint64_t tb = tscStopwatchBegin();
        TestHeap_insert(&heap, &value->node);
        uint64_t te = tscStopwatchEnd();
        double delta = (double) (te - tb) - insertMean;
        insertMean += delta / (double) (r + 1);
        double delta2 = (double) (te - tb) - insertMean;
        insertVar += delta * delta2;
        // Remove minimum
        tb = tscStopwatchBegin();
        value = Value_fromNode(TestHeap_poll(&heap));
        te = tscStopwatchEnd();
        delta = (double) (te - tb) - removeMean;
        removeMean += delta / (double) (r + 1);
        delta2 = (double) (te - tb) - removeMean;
        removeVar += delta * delta2;
    }
    printf("%zu,%g,%g,%g,%g\n", nodeCount, insertMean, removeMean, sqrt(insertVar / (roundCount - 1)), sqrt(removeVar / (roundCount - 1)));
    free(values);
}

static void testPollAndInsertPerformance(size_t nodeCount, size_t roundCount) {
    Value *values = createValues(nodeCount);
    TestHeap heap;
    TestHeap_initialize(&heap);
    for (size_t i = 0; i < nodeCount - 1; ++i) {
        TestHeap_insert(&heap, &values[i].node);
    }
    Value *value = &values[nodeCount - 1];
    double insertMean = 0;
    double insertVar = 0;
    for (size_t r = 0; r < roundCount; ++r) {
        // Insert
        randomizeKey(value);
        uint64_t tb = tscStopwatchBegin();
        value = Value_fromNode(TestHeap_pollAndInsert(&heap, &value->node));
        uint64_t te = tscStopwatchEnd();
        double delta = (double) (te - tb) - insertMean;
        insertMean += delta / (double) (r + 1);
        double delta2 = (double) (te - tb) - insertMean;
        insertVar += delta * delta2;
    }
    printf("%zu,%g,%g\n", nodeCount, insertMean, sqrt(insertVar / (roundCount - 1)));
    free(values);
}

static void testFullCyclePerformance(size_t nodeCount, size_t roundCount) {
    Value *nodes = createValues(nodeCount);
    TestHeap heap;
    TestHeap_initialize(&heap);
    uint64_t tb = tscStopwatchBegin();
    for (size_t r = 0; r < roundCount; ++r) {
        for (size_t i = 0; i < nodeCount; i++) {
            TestHeap_insert(&heap, &nodes[i].node);
        }
        for (size_t i = 0; i < nodeCount; i++) {
            TestHeap_poll(&heap);
        }
    }
    uint64_t te = tscStopwatchEnd();
    printf("%zu,%g\n", nodeCount, (double) (te - tb) / roundCount / nodeCount);
    free(nodes);
}

static void burstRandomRemovalPerformance(size_t roundCount) {
    testRandomRemovalPerformance(1, roundCount);
    testRandomRemovalPerformance(3, roundCount);
    testRandomRemovalPerformance(5, roundCount);
    testRandomRemovalPerformance(10, roundCount);
    testRandomRemovalPerformance(30, roundCount);
    testRandomRemovalPerformance(50, roundCount);
    testRandomRemovalPerformance(100, roundCount);
    testRandomRemovalPerformance(300, roundCount);
    testRandomRemovalPerformance(500, roundCount);
    testRandomRemovalPerformance(1000, roundCount);
    testRandomRemovalPerformance(3000, roundCount);
    testRandomRemovalPerformance(5000, roundCount);
    testRandomRemovalPerformance(10000, roundCount);
    testRandomRemovalPerformance(30000, roundCount);
    testRandomRemovalPerformance(50000, roundCount);
    testRandomRemovalPerformance(100000, roundCount);
    testRandomRemovalPerformance(300000, roundCount);
    testRandomRemovalPerformance(1000000, roundCount);
    testRandomRemovalPerformance(3000000, roundCount);
    testRandomRemovalPerformance(5000000, roundCount);
    testRandomRemovalPerformance(10000000, roundCount);
}

static void burstMinimumRemovalPerformance(size_t roundCount) {
    testMinimumRemovalPerformance(1, roundCount);
    testMinimumRemovalPerformance(3, roundCount);
    testMinimumRemovalPerformance(5, roundCount);
    testMinimumRemovalPerformance(10, roundCount);
    testMinimumRemovalPerformance(30, roundCount);
    testMinimumRemovalPerformance(50, roundCount);
    testMinimumRemovalPerformance(100, roundCount);
    testMinimumRemovalPerformance(300, roundCount);
    testMinimumRemovalPerformance(500, roundCount);
    testMinimumRemovalPerformance(1000, roundCount);
    testMinimumRemovalPerformance(3000, roundCount);
    testMinimumRemovalPerformance(5000, roundCount);
    testMinimumRemovalPerformance(10000, roundCount);
    testMinimumRemovalPerformance(30000, roundCount);
    testMinimumRemovalPerformance(50000, roundCount);
    testMinimumRemovalPerformance(100000, roundCount);
    testMinimumRemovalPerformance(300000, roundCount);
    testMinimumRemovalPerformance(1000000, roundCount);
    testMinimumRemovalPerformance(3000000, roundCount);
    testMinimumRemovalPerformance(5000000, roundCount);
    testMinimumRemovalPerformance(10000000, roundCount);
}

static void burstPollAndInsertPerformance(size_t roundCount) {
    testPollAndInsertPerformance(2, roundCount);
    testPollAndInsertPerformance(3, roundCount);
    testPollAndInsertPerformance(5, roundCount);
    testPollAndInsertPerformance(10, roundCount);
    testPollAndInsertPerformance(30, roundCount);
    testPollAndInsertPerformance(50, roundCount);
    testPollAndInsertPerformance(100, roundCount);
    testPollAndInsertPerformance(300, roundCount);
    testPollAndInsertPerformance(500, roundCount);
    testPollAndInsertPerformance(1000, roundCount);
    testPollAndInsertPerformance(3000, roundCount);
    testPollAndInsertPerformance(5000, roundCount);
    testPollAndInsertPerformance(10000, roundCount);
    testPollAndInsertPerformance(30000, roundCount);
    testPollAndInsertPerformance(50000, roundCount);
    testPollAndInsertPerformance(100000, roundCount);
    testPollAndInsertPerformance(300000, roundCount);
    testPollAndInsertPerformance(1000000, roundCount);
    testPollAndInsertPerformance(3000000, roundCount);
    testPollAndInsertPerformance(5000000, roundCount);
    testPollAndInsertPerformance(10000000, roundCount);
}

static void burstFullCyclePerformance(size_t roundCount) {
    testFullCyclePerformance(1, roundCount);
    testFullCyclePerformance(3, roundCount);
    testFullCyclePerformance(5, roundCount);
    testFullCyclePerformance(10, roundCount);
    testFullCyclePerformance(30, roundCount);
    testFullCyclePerformance(50, roundCount);
    testFullCyclePerformance(100, roundCount);
    testFullCyclePerformance(300, roundCount);
    testFullCyclePerformance(500, roundCount);
    testFullCyclePerformance(1000, roundCount);
    testFullCyclePerformance(3000, roundCount);
    testFullCyclePerformance(5000, roundCount);
    testFullCyclePerformance(10000, roundCount);
    if (roundCount < 100) {
        testFullCyclePerformance(30000, roundCount);
        testFullCyclePerformance(50000, roundCount);
        testFullCyclePerformance(100000, roundCount);
        testFullCyclePerformance(300000, roundCount);
        testFullCyclePerformance(1000000, roundCount);
        testFullCyclePerformance(3000000, roundCount);
        testFullCyclePerformance(5000000, roundCount);
        testFullCyclePerformance(10000000, roundCount);
    }
}

int main() {
    printf("Value size: %zu\n", sizeof(Value));
    #ifndef NDEBUG
    for (size_t i = 0; i < 10; ++i) {
        printf("Round %zu\n", i);
        testConsistency(5000);
    }
    #else
    printf("Random removal benchmark\n");
    printf("Node count,Ins. mean,Rem. mean,Ins. stddev,Rem. stddev\n");
    burstRandomRemovalPerformance(1000000);
    printf("Minimum removal benchmark\n");
    printf("Node count,Ins. mean,Rem. mean,Ins. stddev,Rem. stddev\n");
    burstMinimumRemovalPerformance(1000000);
    printf("Poll and insert benchmark\n");
    printf("Node count,Ins. mean,Ins. stddev\n");
    burstPollAndInsertPerformance(1000000);
    printf("Full cycle benchmark\n");
    burstFullCyclePerformance(1000);
    #endif
}
