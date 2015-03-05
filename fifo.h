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

#ifndef LIFO_H_
#define LIFO_H_

#include <stdint.h>
#include <errno.h>

typedef struct {
	size_t numDataElems;
	size_t readIndex;
	size_t writeIndex;
	size_t count;
} FIFO_Generic_t;

int    _FIFO_Init(FIFO_Generic_t *fifo, size_t numEntries);
int    _FIFO_IsEmpty(FIFO_Generic_t *fifo);
int    _FIFO_IsFull(FIFO_Generic_t *fifo);
size_t _FIFO_Size(FIFO_Generic_t *fifo);
void   _FIFO_Pop(FIFO_Generic_t *fifo, size_t numEntries);
size_t _FIFO_Space(FIFO_Generic_t *fifo);

// TODO: Why isn't the FIFO memory just part of the structure (like mempool?)

#define FIFO_IsFull(name, fifo)       FIFO_IsFull_##name##_(fifo)
#define FIFO_IsEmpty(name, fifo)      FIFO_IsEmpty_##name##_(fifo)
#define FIFO_Size(name, fifo)         FIFO_Size_##name##_(fifo)
#define FIFO_Space(name, fifo)        FIFO_Space_##name##_(fifo)
#define FIFO_Pop(name, fifo, numEntries) FIFO_Pop_##name##_(fifo, numEntries)
#define FIFO_Write(name, fifo, data)  FIFO_Write_##name##_(fifo, data)
#define FIFO_Read(name, fifo)         FIFO_Read_##name##_(fifo)
#define FIFO_Remove(name, fifo, num)  FIFO_Remove_##name##_(fifo, num)
#define FIFO_Init(name, fifo, numEntries, workingBuffer) FIFO_Init_##name##_(fifo, numEntries, workingBuffer)
#define FIFO_GetPointer(name, fifo, pptr, plen)          FIFO_GetPointer_##name##_(fifo, pptr, plen)

#define DECLARE_FIFO(type, name)                                                                                        \
typedef struct {                                                                                                        \
	size_t numDataElems;                                                                                                \
	size_t readIndex;                                                                                                   \
	size_t writeIndex;                                                                                                  \
	size_t count;                                                                                                       \
	type *queue;                                                                                                        \
} FIFO_##name##_t;                                                                                                      \
                                                                                                                        \
static inline int FIFO_IsFull_##name##_(FIFO_##name##_t * fifo)  { return _FIFO_IsFull((FIFO_Generic_t *) fifo);  }     \
static inline int FIFO_IsEmpty_##name##_(FIFO_##name##_t *fifo)  { return _FIFO_IsEmpty((FIFO_Generic_t *) fifo); }     \
static inline size_t FIFO_Size_##name##_(FIFO_##name##_t *fifo)  { return _FIFO_Size((FIFO_Generic_t *) fifo);    }     \
static inline size_t FIFO_Space_##name##_(FIFO_##name##_t *fifo) { return _FIFO_Space((FIFO_Generic_t *) fifo);   }     \
                                                                                                                        \
static inline void FIFO_Pop_##name##_(FIFO_##name##_t *fifo, size_t numEntries)                                         \
{                                                                                                                       \
	_FIFO_Pop((FIFO_Generic_t *) fifo, numEntries);                                                                     \
}                                                                                                                       \
                                                                                                                        \
static inline int FIFO_Init_##name##_(FIFO_##name##_t *fifo, size_t numEntries, type *workingBuffer)                    \
{                                                                                                                       \
	if ((workingBuffer == NULL) || (fifo == NULL))                                                                                          \
		return -EINVAL;                                                                                                 \
                                                                                                                        \
	fifo->queue = workingBuffer;                                                                                        \
	return _FIFO_Init((FIFO_Generic_t *) fifo, numEntries);                                                             \
}                                                                                                                       \
																														\
static inline void FIFO_Write_##name##_(FIFO_##name##_t *fifo, type data)                                               \
{                                                                                                                       \
	fifo->queue[fifo->writeIndex] = data;                                                                               \
	fifo->writeIndex = (fifo->writeIndex + 1) % fifo->numDataElems;                                                     \
	fifo->count++; 																										\
}                                                                                                                       \
																														\
static inline type FIFO_Read_##name##_(FIFO_##name##_t *fifo)                                                           \
{                                                                                                                       \
	type val = fifo->queue[fifo->readIndex];                                                                            \
	fifo->readIndex = (fifo->readIndex + 1) % fifo->numDataElems;                                                       \
	fifo->count--;                                                                                                      \
	return val;                                                                                                         \
}                                                                                                                       \
																														\
static inline void FIFO_GetPointer_##name##_(FIFO_##name##_t *fifo, type **ptr, size_t *len)                            \
{                                                                                                                       \
	*ptr = &(fifo->queue[fifo->readIndex]);                                                                             \
                                                                                                                        \
	if (fifo->writeIndex > fifo->readIndex)                                                                             \
		*len = fifo->writeIndex - fifo->readIndex;                                                                      \
	else                                                                                                                \
		*len = fifo->numDataElems - fifo->readIndex;                                                                    \
}                                                                                                                       \
																														\
static inline void FIFO_Remove_##name##_(FIFO_##name##_t *fifo, size_t numEntries)                                      \
{                                                                                                                       \
	while (numEntries--)                                                                                                \
		FIFO_Read_##name##_(fifo);                                                                                      \
}

#define FIFO(name) FIFO_##name##_t

#endif // FIFO_H_
