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

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "cmocka/cmocka.h"
#include "../fifo.h"

#define WORKING_BUFFER_SIZE 100
uint8_t workingBuffer[WORKING_BUFFER_SIZE];

DECLARE_FIFO(uint8_t, unittest)

// TODO: why can't this be called fifo?
FIFO(unittest) testfifo;

void test_FIFO_readWrite(void **state)
{
	int i;
	int result = FIFO_Init(unittest, &testfifo, WORKING_BUFFER_SIZE, workingBuffer);

	assert_true(result == 0);

	for (i=0; i<WORKING_BUFFER_SIZE; i++) {
		FIFO_Write(unittest, &testfifo, i);
	}

	for (i=0; i<WORKING_BUFFER_SIZE; i++) {
		uint32_t val = FIFO_Read(unittest, &testfifo);

		assert_true(val == i);
	}
}

void test_FIFO_full(void **state)
{
	int i;
	int result = FIFO_Init(unittest, &testfifo, WORKING_BUFFER_SIZE, workingBuffer);

	assert_true(result == 0);

	result = FIFO_IsFull(unittest, &testfifo);

	assert_true(result == 0); // We just started, so queue should be empty

	for (i=0; i<WORKING_BUFFER_SIZE; i++) {
		FIFO_Write(unittest, &testfifo, 10);
	}

	result = FIFO_IsFull(unittest, &testfifo);

	assert_true(result == 1);
}

void test_FIFO_empty(void **state)
{
	int i;
	int result = FIFO_Init(unittest, &testfifo, WORKING_BUFFER_SIZE, workingBuffer);

	assert_true(result == 0);

	result = FIFO_IsEmpty(unittest, &testfifo);

	assert_true(result == 1); // We just started, so queue should be empty

	for (i=0; i<10; i++) {
		FIFO_Write(unittest, &testfifo, 10);
	}

	result = FIFO_IsEmpty(unittest, &testfifo);

	assert_true(result == 0);
}

void test_FIFO_init(void **state)
{
	int result;

	result = FIFO_Init(unittest, NULL, WORKING_BUFFER_SIZE, workingBuffer);

	assert_true(result == -EINVAL);

	result = FIFO_Init(unittest, &testfifo, 0, workingBuffer);

	assert_true(result == -EINVAL);

	result = FIFO_Init(unittest, &testfifo, WORKING_BUFFER_SIZE, NULL);

	assert_true(result == -EINVAL);

	result = FIFO_Init(unittest, &testfifo, WORKING_BUFFER_SIZE, workingBuffer);

	assert_true(result == 0);
}

void test_FIFO_size(void **state)
{
	int i;
	int size;
	int result = FIFO_Init(unittest, &testfifo, WORKING_BUFFER_SIZE, workingBuffer);

	assert_true(result == 0);

	size = FIFO_Size(unittest, &testfifo);

	assert_true(size == 0);

	for (i=0; i<20; i++) {
		FIFO_Write(unittest, &testfifo, 0);
	}

	size = FIFO_Size(unittest, &testfifo);

	assert_true(size == 20);

	for (i=0; i<20; i++) {
		FIFO_Read(unittest, &testfifo);
	}

	size = FIFO_Size(unittest, &testfifo);

	assert_true(size == 0);

	/* Write/read enough data to cause pointers to wrap around */
	for (i=0; i<80; i++) {
		FIFO_Write(unittest, &testfifo, 0);
	}

	for (i=0; i<40; i++) {
		FIFO_Read(unittest, &testfifo);
	}

	for (i=0; i<40; i++) {
		FIFO_Write(unittest, &testfifo, 0);
	}

	size = FIFO_Size(unittest, &testfifo);

	assert_true(size == 80);

}

void test_FIFO_remove(void **state)
{
	int i;
	int size;
	int result = FIFO_Init(unittest, &testfifo, WORKING_BUFFER_SIZE, workingBuffer);

	assert_true(result == 0);

	for (i=0; i<20; i++) {
		FIFO_Write(unittest, &testfifo, 0);
	}

	size = FIFO_Size(unittest, &testfifo);

	assert_true(size == 20);

	FIFO_Remove(unittest, &testfifo, 10);

	size = FIFO_Size(unittest, &testfifo);

	assert_true(size == 10);
}

void test_FIFO_getPointer(void **state)
{
	int i;
	uint8_t *ptr;
	size_t len;

	int result = FIFO_Init(unittest, &testfifo, WORKING_BUFFER_SIZE, workingBuffer);

	assert_true(result == 0);

	FIFO_GetPointer(unittest, &testfifo, &ptr, &len);

	assert_true(len == 0);

	for (i=0; i<10; i++) {
		FIFO_Write(unittest, &testfifo, i);
	}

	FIFO_GetPointer(unittest, &testfifo, &ptr, &len);

	assert_true(len == 10);

	for (i=0; i<10; i++) {
		assert_true(ptr[i] == i);
	}

	/* Write/read enough data to cause pointers to wrap around */
	result = FIFO_Init(unittest, &testfifo, WORKING_BUFFER_SIZE, workingBuffer);

	assert_true(result == 0);

	for (i=0; i<80; i++) {
		FIFO_Write(unittest, &testfifo, 0);
	}

	for (i=0; i<40; i++) {
		FIFO_Read(unittest,&testfifo);
	}

	for (i=0; i<40; i++) {
		FIFO_Write(unittest, &testfifo, 0);
	}

	FIFO_GetPointer(unittest, &testfifo, &ptr, &len);

	assert_true(len == 60);
}

void run_FIFO_tests(void)
{
	UnitTest fifo_tests[] = {
			unit_test(test_FIFO_init),
			unit_test(test_FIFO_empty),
			unit_test(test_FIFO_full),
			unit_test(test_FIFO_readWrite),
			unit_test(test_FIFO_size),
			unit_test(test_FIFO_getPointer),
			unit_test(test_FIFO_remove)
	};

	run_group_tests(fifo_tests);
}
