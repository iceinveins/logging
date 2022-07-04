#include "shm.h"
#include <stdlib.h>
#include <string.h>

#define CAS(ptr, old, new) __sync_bool_compare_and_swap(ptr, old, new)

int ring_queue_push(ring_queue_t* pqueue, void* pitem) {
    // try to set write flag
    while (1) {
        if (ring_queue_is_full(pqueue)) {
            return -1;
        }

        if (CAS(&pqueue->write_flag, 0, 1)) {   // set write flag successfully
            break;
        }
    }

    // push data
    memcpy(pqueue->pbuf + pqueue->tail * RING_QUEUE_ITEM_SIZE, pitem, RING_QUEUE_ITEM_SIZE);
    pqueue->tail = (pqueue->tail + 1) % RING_QUEUE_CAPACITY;
    if (0 == pqueue->tail) {    // a new cycle
        pqueue->diff_cycle = 1;     // tail is not the same cycle with head
    }

    // reset write flag
    CAS(&pqueue->write_flag, 1, 0);

    return 0;
}

int ring_queue_pop(ring_queue_t* pqueue, void* pitem) {
    // try to set read flag
    while (1) {
        if (ring_queue_is_empty(pqueue)) {
            return -1;
        }

        if (CAS(&pqueue->read_flag, 0, 1)) {    // set read flag successfully
            break;
        }
    }

    // read data
    memcpy(pitem, pqueue->pbuf + pqueue->head * RING_QUEUE_ITEM_SIZE, RING_QUEUE_ITEM_SIZE);
    pqueue->head = (pqueue->head + 1) % RING_QUEUE_CAPACITY;
    if (0 == pqueue->head) {
        pqueue->diff_cycle = 0;     // head is now the same cycle with tail
    }

    // reset read flag
    CAS(&pqueue->read_flag, 1, 0);

    return 0;
}

int ring_queue_is_empty(ring_queue_t* pqueue) {
    return (pqueue->head == pqueue->tail) && !pqueue->diff_cycle;
}

int ring_queue_is_full(ring_queue_t* pqueue) {
    return (pqueue->head == pqueue->tail) && pqueue->diff_cycle;
}