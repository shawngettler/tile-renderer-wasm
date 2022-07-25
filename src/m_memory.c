/**
 * m_memory.c
 * @shawngettler
 *
 * WebAssembly memory allocation tools to avoid standard libraries.
 */

#include "m_memory.h"



void M_Init(void);
void M_MergeFreeBlocks(void);



// beginning of allocated memory
memblock_t *head = (memblock_t *)&__heap_base;



/**
 * Initialize block list.
 */
void M_Init() {
    int pages = __builtin_wasm_memory_size(0);

    memblock_t *empty = (memblock_t *)&__heap_base;
    empty->size = pages*PAGE_SIZE - ((int)&__heap_base + sizeof(memblock_t));
    empty->free = 1;
    empty->next = NULL;

    head = empty;
}

/**
 * Iterate through block list and merge adjacent free blocks.
 */
void M_MergeFreeBlocks() {
    memblock_t *curr = head;
    memblock_t *next = curr->next;
    while(curr != NULL && next != NULL) {
        if(curr->free && next->free) {
            curr->size += next->size + sizeof(memblock_t);
            curr->next = next->next;
        } else {
            curr = next;
            next = curr->next;
        }
    }
}



/**
 * Allocate the requested block of memory. If there is not a large enough
 * block of memory, increase the size of the WebAssembly linear memory.
 *
 * @param bytes_size number of bytes to allocate
 *
 * @return pointer to the allocated memory
 */
void *M_AllocateBytes(int bytes_size) {
    int block_size = bytes_size + sizeof(memblock_t);

    if(head->size == 0) {
        M_Init();
    }

    memblock_t *curr = head;
    memblock_t *prev = curr;
    while(curr != NULL) {
        if(curr->free) {
            if(curr->size == bytes_size) {
                curr->free = 0;
                return curr + 1;
            } else if(curr->size > bytes_size) {
                memblock_t *empty = (memblock_t *)((unsigned int)curr + block_size);
                empty->size = curr->size - block_size;
                empty->free = 1;
                empty->next = curr->next;

                curr->size = bytes_size;
                curr->free = 0;
                curr->next = empty;
                return curr + 1;
            }
        }
        prev = curr;
        curr = curr->next;
    }

    // no blocks large enough
    __builtin_wasm_memory_grow(0, block_size/PAGE_SIZE + 1);
    int pages = __builtin_wasm_memory_size(0);

    if(prev->free) {
        prev->size = pages*PAGE_SIZE - ((int)prev + sizeof(memblock_t));
    } else {
        memblock_t *empty = (memblock_t *)((int)prev + sizeof(memblock_t) + prev->size);
        empty->size = pages*PAGE_SIZE - sizeof(memblock_t);
        empty->free = 1;
        empty->next = NULL;

        prev->next = empty;
    }

    return M_AllocateBytes(bytes_size);
}

/**
 * Free the memory at the pointer location.
 *
 * @param ptr pointer to the allocated memory
 */
void M_FreeBytes(void *ptr) {
    if(ptr == NULL) {
        return;
    }

    memblock_t *curr = ptr - 1;
    curr->free = 1;
    M_MergeFreeBlocks();
}
