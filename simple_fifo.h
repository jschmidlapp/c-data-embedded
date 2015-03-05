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

#ifndef SIMPLE_FIFO_H_
#define SIMPLE_FIFO_H_

/*
 * A very simple single consumer, single producer FIFO.  This should be lockfree,
 * though two memory barriers may be required (see notes in code below).  For
 * efficiency, two assumptions are made;
 *
 * 1. The specified size must be a power of two.
 * 2. MAXINT-1 must be an even multiple of size.
 *
 * The code doesn't do any checking that these conditions are met; bad things will
 * happen if they aren't.
 *
 */

#include <stddef.h>

#define SFIFO_Init(name, fifo)    SFIFO_Init_##name##_(fifo)
#define SFIFO_Get(name, fifo)     SFIFO_Get_##name##_(fifo)
#define SFIFO_Pop(name, fifo)     SFIFO_Pop_##name##_(fifo)
#define SFIFO_Push(name, fifo,data)    SFIFO_Push_##name##_(fifo, data)
#define SFIFO_IsEmpty(name, fifo) SFIFO_IsEmpty_##name##_(fifo)
#define SFIFO_IsFull(name, fifo)  SFIFO_IsFull_##name##_(fifo)

#define SFIFO(name) SFIFO_##name##_t

#define MOD2(a,b) (a & (b-1))

#define DECLARE_SIMPLE_FIFO(type, name, size)                                \
typedef struct {                                                             \
	size_t produce_count;                                                    \
	size_t consume_count;                                                    \
	type   buffer[size];                                                     \
} SFIFO_##name##_t;                                                          \
                                                                             \
static inline int SFIFO_Init_##name##_(SFIFO_##name##_t *fifo)               \
{                                                                            \
	if (!fifo)                                                               \
		return -1;                                                           \
	fifo->produce_count = 0;                                                 \
	fifo->consume_count = 0;                                                 \
	return 0;                                                                \
}                                                                            \
                                                                             \
static inline type SFIFO_Get_##name##_(SFIFO_##name##_t *fifo)               \
{                                                                            \
	return fifo->buffer[MOD2(fifo->consume_count, size)];                    \
}                                                                            \
                                                                             \
static inline type SFIFO_Pop_##name##_(SFIFO_##name##_t *fifo)               \
{                                                                            \
	type data = fifo->buffer[MOD2(fifo->consume_count, size)];               \
	/* Memory barrier ? */                                                   \
	++fifo->consume_count;                                                   \
	return data;                                                             \
}                                                                            \
                                                                             \
static inline void SFIFO_Push_##name##_(SFIFO_##name##_t *fifo, type data)   \
{                                                                            \
	fifo->buffer[MOD2(fifo->produce_count, size)] = data;                    \
	/* Memory barrier ? */                                                   \
	++fifo->produce_count;                                                   \
}                                                                            \
                                                                             \
static inline int SFIFO_IsFull_##name##_(SFIFO_##name##_t *fifo)             \
{                                                                            \
	return ((fifo->produce_count - fifo->consume_count) == size);            \
}                                                                            \
                                                                             \
static inline int SFIFO_IsEmpty_##name##_(SFIFO_##name##_t *fifo)            \
{                                                                            \
	return ((fifo->produce_count - fifo->consume_count) == 0);               \
}

#endif // SIMPLE_FIFO_H_
