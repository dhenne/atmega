#include "fifo.h"
#include <stdbool.h>

const uint8_t BUFFER_SIZE = BUFFER_SIZE_DEF;

bool fifo_add(fifo* _fifo, uint8_t data) {
    if ( (_fifo->end + 1  == _fifo->start ) || (( _fifo->end + 1 == BUFFER_SIZE) && (_fifo->start == 0)) ) {
        return false;
    } else {
        _fifo->buffer[_fifo->end] = data;
        _fifo->end++;
        if (_fifo->end == BUFFER_SIZE ) {
            _fifo->end = 0;
        }
        return true;
    }
}

bool fifo_rem(fifo* _fifo, uint8_t* data) {
    if (  _fifo->start == _fifo->end ) {
        return false;
    } else {
        *data = _fifo->buffer[_fifo->start];
        _fifo->start++;
        if (_fifo->start == BUFFER_SIZE ) {
            _fifo->start = 0;
        }
        return true;
    }
}

bool fifo_empty(fifo* _fifo) {
    return _fifo->start == _fifo->end;
}

bool fifo_full(fifo* _fifo) {
	return (_fifo->end + 1  == _fifo->start ) || (( _fifo->end + 1 == BUFFER_SIZE) && (_fifo->start == 0));
}
