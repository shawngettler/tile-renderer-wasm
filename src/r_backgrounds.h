/**
 * r_backgrounds.h
 * @shawngettler
 *
 * Background layer rendering.
 */

#ifndef R_BACKGROUNDS_H
#define R_BACKGROUNDS_H



typedef struct {
    int enabled;
    int mapadd;
    int mapsize_x;
    int mapsize_y;
    int chradd;
    int chrsize;
    int chrbpp;
    int offset_x;
    int offset_y;
    int pri0;
    int pri1;
} background_t;



extern background_t bgr1, bgr2, bgr3, bgr4;

extern void R_ReadBgrRegisters(registers_t);
extern void R_DrawBgrScanline(background_t, int);



#endif
