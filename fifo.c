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

#include <errno.h>
#include <stdlib.h>

#include "fifo.h"

int _FIFO_IsFull(FIFO_Generic_t *fifo)
{
	if (fifo == NULL)
		return 0;

	return (fifo->count >= fifo->numDataElems);
}

int _FIFO_IsEmpty(FIFO_Generic_t *fifo)
{
	return (fifo->count == 0);
}

int _FIFO_Init(FIFO_Generic_t *fifo, size_t numEntries)
{
	if ((fifo == NULL) || (numEntries == 0))
		return -EINVAL;

	fifo->numDataElems = numEntries;
	fifo->writeIndex = 0;
	fifo->readIndex = 0;
	fifo->count = 0;

	return 0;
}

void _FIFO_Pop(FIFO_Generic_t *fifo, size_t numEntries)
{
	fifo->readIndex = (fifo->readIndex + numEntries) % fifo->numDataElems;
}

size_t _FIFO_Size(FIFO_Generic_t *fifo)
{
	return fifo->count;
}

size_t _FIFO_Space(FIFO_Generic_t *fifo)
{
	return (fifo->numDataElems - fifo->count);
}
