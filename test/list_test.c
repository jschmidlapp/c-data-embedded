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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cmocka/cmocka.h"
#include "../list.h"

typedef struct test_entry {
	LIST_node_t node;
	int data;
} test_entry_t;

LIST_node_t head;

static void test_LIST_empty(void **state)
{
	int result;
	test_entry_t entry;

	LIST_Init(&head);

	result = LIST_Empty(&head);

	assert_true(result == 1);

	LIST_Add(&head, &entry);

	result = LIST_Empty(&head);

	assert_true(result == 0);

	LIST_Del(&entry);

	result = LIST_Empty(&head);

	assert_true(result == 1);
}

static void test_LIST_add(void **state)
{
	int i;
	test_entry_t entries[5];
	LIST_node_t head;
	test_entry_t *entry;

	LIST_Init(&head);

	for (i=0; i<5; i++) {
		entries[i].data = i;
	}

	for (i=0; i<5; i++) {
		LIST_Add(&head, &entries[i]);
	}

	i = 4;
	LIST_foreach(entry, &head, test_entry_t) {
		assert_true(entry->data == i);
		i--;
	}
}

static void test_LIST_del(void **state)
{
	int i;
	test_entry_t entries[5];
	LIST_node_t head;
	test_entry_t *entry;

	LIST_Init(&head);

	for (i=0; i<5; i++) {
		entries[i].data = i;
	}

	for (i=0; i<5; i++) {
		LIST_Add(&head, &entries[i]);
	}

	LIST_Del(&entries[2]);

	i=4;
	LIST_foreach(entry, &head, test_entry_t) {
		assert_true(entry->data == i);
		i--;
		if (i==2)
			i--;
	}
}

void run_LIST_tests(void)
{
	UnitTest list_tests[] = {
			unit_test(test_LIST_empty),
			unit_test(test_LIST_add),
			unit_test(test_LIST_del)
	};

	run_tests(list_tests);
}

