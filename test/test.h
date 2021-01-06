/*
Simple C unit testing utility (compatible with NetBeans).
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
#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED

extern int exitCode;

#define ASSERT(condition) \
    if (!(condition)) { \
        __builtin_printf("%%TEST_FAILED%% time=0 testname=%s (%s) message=Assertion %s failed in %s in %s:%u\n", __func__, __FILE__, #condition, __func__, __FILE__, __LINE__); \
        exitCode = -1; \
    }

#define ASSERTN(testName, line, condition) \
    if (!(condition)) { \
        __builtin_printf("%%TEST_FAILED%% time=0 testname=%s (%s) message=Assertion %s failed in %s in %s:%u\n", testName, __FILE__, #condition, __func__, __FILE__, line); \
        exitCode = -1; \
    }

#define RUN_TEST(name) \
    __builtin_printf("%%TEST_STARTED%%  %s (%s)\n", #name, __FILE__); \
    name(); \
    __builtin_printf("%%TEST_FINISHED%% time=0 %s (%s)\n", #name, __FILE__);

#define RUN_SUITE(name) \
    __builtin_printf("%%SUITE_STARTING%% %s\n", #name); \
    __builtin_printf("%%SUITE_STARTED%%\n"); \
    name(); \
    __builtin_printf("%%SUITE_FINISHED%% time=0\n");

#endif
