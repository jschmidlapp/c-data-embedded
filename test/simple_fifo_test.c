/*
 * Copyright (c) 2015 Jason Schmidlapp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "../simple_fifo.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include "cmocka.h"
#include <errno.h>

#define FIFO_SIZE 8

DECLARE_SIMPLE_FIFO(int, test, FIFO_SIZE);

SFIFO(test) fifo;

void test_SFIFO_readWrite(void **state)
{
	int i;
	int result = SFIFO_Init(test, &fifo);

	assert_true(result == 0);

	for (i=0; i<FIFO_SIZE; i++) {
		SFIFO_Push(test, &fifo, i);
	}

	for (i=0; i<FIFO_SIZE; i++) {
		int val = SFIFO_Get(test, &fifo);
		assert_true(val == i);
		val = SFIFO_Pop(test, &fifo);
		assert_true(val == i);
	}
}

void test_SFIFO_full(void **state)
{
	int i;
	int result = SFIFO_Init(test, &fifo);

	assert_true(result == 0);

	result = SFIFO_IsFull(test, &fifo);

	assert_true(result == 0); // We just started, so queue should be empty

	for (i=0; i<FIFO_SIZE; i++) {
		SFIFO_Push(test, &fifo, i);
	}

	result = SFIFO_IsFull(test, &fifo);

	assert_true(result == 1);
}

void test_SFIFO_empty(void **state)
{
	int i;
	int result = SFIFO_Init(test, &fifo);

	assert_true(result == 0);

	result = SFIFO_IsEmpty(test, &fifo);

	assert_true(result == 1); // We just started, so queue should be empty

	for (i=0; i<10; i++) {
		SFIFO_Push(test, &fifo, 10);
	}

	result = SFIFO_IsEmpty(test, &fifo);

	assert_true(result == 0);
}

void test_SFIFO_init(void **state)
{
	int result;

	result = SFIFO_Init(test, NULL);

	assert_true(result < 0);

	result = SFIFO_Init(test, &fifo);

	assert_true(result == 0);
}

void test_SFIFO_wrap(void **state)
{
	int i;

	for (i=0; i<FIFO_SIZE; i++) {
		SFIFO_Push(test, &fifo, i);
	}

	for (i=0; i<5; i++) {
		int val = SFIFO_Pop(test, &fifo);
		assert_true(val == i);
	}

	for (i=0; i<3; i++) {
		SFIFO_Push(test, &fifo, FIFO_SIZE+i);
	}

	for (i=0; i<FIFO_SIZE-5; i++) {
		int val = SFIFO_Pop(test, &fifo);
		assert_true(val = 5+i);
	}

	for (i=0; i<3; i++) {
		int val = SFIFO_Pop(test, &fifo);
		assert_true(val == FIFO_SIZE+i);
	}
}


void run_SFIFO_tests(void)
{
	UnitTest sfifo_tests[] = {
			unit_test(test_SFIFO_init),
			unit_test(test_SFIFO_empty),
			unit_test(test_SFIFO_full),
			unit_test(test_SFIFO_readWrite),
			unit_test(test_SFIFO_wrap),
	};

	run_group_tests(sfifo_tests);
}


