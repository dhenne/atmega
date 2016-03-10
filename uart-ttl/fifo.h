/*
 * File:   fifo-function.h
 * Author: Student
 *
 * Created on 7. Januar 2016, 13:17
 */

#ifndef FIFO_FUNCTION_H
#define	FIFO_FUNCTION_H

#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE_DEF 32

typedef struct __fifo
{
    uint8_t* buffer;
    const uint8_t max;
    uint8_t end;
    uint8_t start;
} fifo;

// main functions
bool fifo_add(fifo* _fifo, uint8_t data);
bool fifo_rem(fifo* _fifo, uint8_t* data);

// STATUS Functions
bool fifo_empty(fifo* _fifo);

#endif	/* FIFO_FUNCTION_H */

