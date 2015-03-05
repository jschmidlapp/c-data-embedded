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

#ifndef MEMPOOL_H_
#define MEMPOOL_H_

/*
 * Generic memory/buffer pool implementation.  Designed to be generic for any type
 * (including structures) and to work with static memory allocation.  That is, all
 * data for both the actual buffers as well as the management structure is defined in
 * whatever context the MEMPOOL() macro (see below) is called.
 *
 * Usage:
 *
 * The memory pool must first be declared in your source file.  This should be in module
 * context (i.e. with other typedefs for the particular module.  This defines the memory
 * pool management data structures and inline functions.  It however does *NOT*
 * define memory - this is done with another call.  The memory pool declaration should be
 * called as:
 *
 * ...other static variables and typedefs...
 *
 * DECLARE_MEMPOOL(RxDescriptor_t, 10, rxdesc);
 *
 * This declares the data structures for a pool of 10 entires of type RxDescriptor_t.  The
 * name is a user specified value that is needed for the other API calls below.  Due to
 * C preprocessor limitations, the type must be a single string without spaces.  For example,
 * you need to use RxDescriptor_t instead of struct RxDescriptor.
 *
 * Once the memory pool is declared, the actual memory pool should be defined.  There aren't
 * too many restrictions on where this can happen.  That is, it can be done at global
 * context, inside of a structure, etc.  It is called like:
 *
 * MEMPOOL(rxdesc) rxd_pool;
 *
 * Where rxdesc is the arbitrary name given to DECLARE_MEMPOOL.  While I've never tried it,
 * it should be possible to put other modifiers in front of MEMPOOL (e.g. static or compiler
 * specific memory location specfifiers).
 *
 * Once the memory pool is declared and defined, there are three API calls to actually use
 * the memory pool:
 *
 * MEMPOOL_Init(poolname, varname)
 *
 * Initializes the buffer pool with the given name (e.g. rxdesc) and the actual pool (e.g. rxd_pool)
 *
 *
 * MEMPOOL_Alloc(poolname, varname)
 *
 * Allocates a buffer from the pool.  For instance:
 *
 * RxDescriptor_t *ptr = MEMPOOL_Alloc(rxdesc, rxd_pool);
 *
 *
 * MEMPOOL_Free(poolname, varname, buffername)
 *
 * Frees a buffer back to the pool.  For instance:
 *
 * MEMPOOL_Free(rxdesc, rxd_pool, ptr);
 *
 *
 * Implementation note: This uses a lot of C preprocessor magic but should be portable
 * (e.g. doesn't use any additional GCC magic like typeof)
 */

#include <stddef.h>
#include "list.h"

typedef struct
{
	LIST_node_t node;
	int         data;
} BufferPoolDescriptor_t;

#define DECLARE_MEMPOOL(type, size, name)                                       			  \
typedef struct _MEMPOOL_##name {                                                			  \
	LIST_node_t freeList;                                                            			  \
	LIST_node_t storeList;                                                           			  \
	struct {                                                                    			  \
		LIST_node_t node;                                                            			  \
		type buffer;                                                            			  \
	} bufferDescs[size];                                                        			  \
} _MEMPOOL_##name;                                                              			  \
                                                                                			  \
typedef type _MEMPOOL_type_##name;                                              			  \
static inline void _MEMPOOL_Init_##name(_MEMPOOL_##name *pool) {                			  \
	int i;                                                                      			  \
	int numElems = sizeof(pool->bufferDescs)/sizeof(pool->bufferDescs[0]);      			  \
	LIST_Init(&pool->freeList);                                                 			  \
	LIST_Init(&pool->storeList);                                                			  \
	for (i=0; i<numElems; i++)                                                  			  \
		LIST_Add(&pool->freeList, &(pool->bufferDescs[i]));                     			  \
}                                                                               			  \
static inline type *_MEMPOOL_Alloc_##name(_MEMPOOL_##name *pool) {              			  \
	void *ptr;                                                                  			  \
	if (LIST_Empty(&pool->freeList)) {                                                        \
		ptr = NULL;                                                                           \
	}                                                                                         \
	else {                                                                                    \
		BufferPoolDescriptor_t *desc = (BufferPoolDescriptor_t *) pool->freeList.next;        \
		ptr = &(desc->data);                                                                  \
		LIST_Del(desc);                                                                       \
		LIST_Add(&pool->storeList, desc);                                                     \
	}                                                                                         \
                                                                                              \
	return ptr;                                                                               \
}                                                                                             \
static inline void _MEMPOOL_Free_##name(_MEMPOOL_##name *pool, _MEMPOOL_type_##name *ptr) {   \
	int i;                                                                                    \
	for (i=0; i<sizeof(pool->bufferDescs)/sizeof(pool->bufferDescs[0]); i++) {                \
		if (ptr == &pool->bufferDescs[i].buffer) {                                            \
			LIST_Add(&pool->freeList, &pool->bufferDescs[i]);                                 \
		}                                                                                     \
	}                                                                                         \
}


#define MEMPOOL(poolname) _MEMPOOL_##poolname

#define MEMPOOL_Init(name, pool) _MEMPOOL_Init_##name(pool)

#define MEMPOOL_Alloc(name, pool) _MEMPOOL_Alloc_##name((_MEMPOOL_##name *) pool);

#define MEMPOOL_Free(name, pool, ptr) _MEMPOOL_Free_##name((_MEMPOOL_##name *) pool, ptr)

#endif /* MEMPOOL_H_ */
