#include<stddef.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#include"devector.h"

#define MIN_CAPACITY 2
#define INIT_CAPACITY (MIN_CAPACITY)

#define DEVECTOR_POINTER(dvptr, index) ((char*)dvptr->buffer + (index) * dvptr->elem_size)

void* devector_at(const devector* ptr, size_t i) {
    return DEVECTOR_POINTER(ptr, (ptr->capacity - ptr->head + i) & (ptr->capacity - 1));
}

size_t devector_size(const devector* ptr) {
    return (ptr->tail + ptr->head) & (ptr->capacity - 1);
}

static devectorBufferState devector_resize(devector* ptr, size_t ncap) {

    void* tmp = malloc(ncap * ptr->elem_size);

    if (tmp) {

        if ((ptr->capacity - 1) - ptr->head >= ptr->tail) {

            if (ptr->head) {

                const void* src = DEVECTOR_POINTER(ptr, ptr->capacity - ptr->head);
                void* dst = (char*)tmp + (ncap - ptr->head) * ptr->elem_size;

                memcpy(dst, src, ptr->head * ptr->elem_size);
            }

            if (ptr->tail) {

                const void* src = ptr->buffer;
                void* dst = tmp;

                memcpy(dst, src, ptr->tail * ptr->elem_size);
            }

        }
        else {

            const void* src = DEVECTOR_POINTER(ptr, ptr->capacity - ptr->head);
            void* dst = tmp;

            ptr->tail -= (ptr->capacity - ptr->head), ptr->head = 0;

            memcpy(dst, src, ptr->tail * ptr->elem_size);

        }

        free(ptr->buffer); ptr->buffer = tmp;

        ptr->capacity = ncap;

        return VALID_BUFFER_STATE;
    }
    else {
        return INVALID_BUFFER_STATE;
    }
}

static devectorBufferState devector_advance(devector* ptr, size_t* idx) {

    *idx = (*idx + 1) & (ptr->capacity - 1);

    if ((ptr->capacity - 1) - ptr->head == ptr->tail) {

        if (ptr->capacity <= SIZE_MAX /  2) {
            return devector_resize(ptr, ptr->capacity * 2);
        }
        else {
            return INVALID_BUFFER_STATE;
        }
    }

    return VALID_BUFFER_STATE;
}

static devectorBufferState devector_retreat(devector* ptr, size_t* idx) {

    *idx = (*idx) ? *idx - 1 : ptr->capacity - 1;

    if (devector_size(ptr) + MIN_CAPACITY == ptr->capacity / 4) {
       return devector_resize(ptr, ptr->capacity / 4);
    }

    return VALID_BUFFER_STATE;
}

devectorBufferState devector_push_front(devector* ptr, const void* src) {

    void* dst = DEVECTOR_POINTER(ptr, (ptr->capacity - 1) - ptr->head);

    memcpy(dst, src, ptr->elem_size);
    
    return devector_advance(ptr, &ptr->head);
}

devectorBufferState devector_push_back(devector* ptr, const void* src) {

    void* dst = DEVECTOR_POINTER(ptr, ptr->tail);

    memcpy(dst, src, ptr->elem_size);
    
    return devector_advance(ptr, &ptr->tail);
}

devectorBufferState devector_pop_front(devector* ptr) {

    if (!devector_size(ptr)) return VALID_BUFFER_STATE;
    
    return devector_retreat(ptr, &ptr->head);
}

devectorBufferState devector_pop_back(devector* ptr) {

    if (!devector_size(ptr)) return VALID_BUFFER_STATE;
    
    return devector_retreat(ptr, &ptr->tail);
}

devectorBufferState devector_deploy(devector* ptr, size_t elem_size) {

    ptr->buffer = malloc(INIT_CAPACITY * elem_size);

    if (ptr->buffer) {

        ptr->capacity = INIT_CAPACITY;
        ptr->elem_size = elem_size;

        ptr->head =  ptr->tail = 0;

        return VALID_BUFFER_STATE;
    }
    else {
        return INVALID_BUFFER_STATE;
    }
}

void devector_destroy(devector* ptr) {

    free(ptr->buffer);
    memset(ptr, 0, sizeof(devector));
}
