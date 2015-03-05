/*
 * mempool_test.c
 *
 *  Created on: Jan 18, 2015
 *      Author: jason
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../cmocka/cmocka.h"
#include "../mempool.h"

typedef struct test {
	uint8_t data[100];
} test_t;

DECLARE_MEMPOOL(test_t, 10, rxdesc)

static void test_MEMPOOL_alloc(void **state)
{
	int i;
	struct test *ptr;
	struct test *ptr_array[10];
	MEMPOOL(rxdesc) pool;

	MEMPOOL_Init(rxdesc, &pool);

	for (i=0;i<10;i++) {
		ptr = MEMPOOL_Alloc(rxdesc, &pool);
		assert_true(ptr != NULL);

		/* Write some data to the allocated buffer to
		 * make sure we don't crash.
		 */
		memset(ptr, 0, sizeof(test_t));

		ptr_array[i] = ptr;
	}

	ptr = MEMPOOL_Alloc(rxdesc, &pool);

	assert_true(ptr == NULL);

	for (i=0; i<10; i++) {
		MEMPOOL_Free(rxdesc, &pool, ptr_array[i]);
	}

	/* Do it again to make sure that the buffers were actually freed */
	for (i=0;i<10;i++) {
		ptr = MEMPOOL_Alloc(rxdesc, &pool);
		assert_true(ptr != NULL);

		/* Write some data to the allocated buffer to
		 * make sure we don't crash.
		 */
		memset(ptr, 0, sizeof(test_t));
	}

}

static void test_MEMPOOL_free(void **state)
{
	int i;
	MEMPOOL(rxdesc) pool;

	MEMPOOL_Init(rxdesc, &pool);

	for (i=0;i<10;i++) {
		struct test *ptr = MEMPOOL_Alloc(rxdesc, &pool);
		assert_true(ptr != NULL);
	}
}

void run_MEMPOOL_tests(void)
{
	UnitTest mempool_tests[] = {
			unit_test(test_MEMPOOL_alloc),
			unit_test(test_MEMPOOL_free)
	};

	run_group_tests(mempool_tests);
}
