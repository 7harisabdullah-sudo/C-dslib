#ifndef DEVECTOR_H
#define DEVECTOR_H

#include<stddef.h>

typedef enum {

    INVALID_BUFFER_STATE = 0,
    VALID_BUFFER_STATE = 1

} devectorBufferState;

typedef struct devector {

    void* buffer;

    size_t elem_size, capacity;

    size_t head, tail;

} devector;

size_t devector_size(const devector* ptr);
void* devector_get( const devector* ptr, size_t i);

devectorBufferState devector_push_front(devector* ptr, const void* src);
devectorBufferState devector_push_back(devector* ptr, const void* src);

devectorBufferState devector_pop_front(devector* ptr);
devectorBufferState devector_pop_back(devector* ptr);

devectorBufferState devector_deploy(devector* ptr, size_t elem_size);
void devector_destroy(devector* ptr);

#endif
