/**
 * m_memory.h
 * @shawngettler
 *
 * WebAssembly memory allocation tools to avoid standard libraries.
 */

#ifndef M_MEMORY_H
#define M_MEMORY_H

#define NULL 0
#define PAGE_SIZE 65536



typedef struct memblock_s {
    int size;
    int free;
    struct memblock_s *next;
} memblock_t;



extern unsigned char __heap_base;

extern void *M_AllocateBytes(int);
extern void M_FreeBytes(void *);



#endif
