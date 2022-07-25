/**
 * r_renderer.h
 * @shawngettler
 *
 * 2D background and sprite renderer.
 */

#ifndef R_RENDERER_H
#define R_RENDERER_H

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 224



typedef struct {
    unsigned char *vram;
    unsigned char *oaram;
    unsigned char *cgram;
} rendermem_t;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} color32_t;

typedef struct {
    int col;
    int pri;
} pixel_t;



extern rendermem_t mem;
extern pixel_t pix[];

extern void R_DrawScreen(color32_t *, unsigned char *, unsigned char *, unsigned char *, registers_t *);



#endif
