/**
 * r_backgrounds.c
 * @shawngettler
 *
 * Background layer rendering.
 */

#include "r_registers.h"
#include "r_renderer.h"

#include "r_backgrounds.h"



background_t bgr1, bgr2, bgr3, bgr4;



/**
 * Update background properties based on the current hardware registers.
 *
 * @param regs registers values
 */
void R_ReadBgrRegisters(registers_t regs) {
    int mode = regs.bgrmode & 0x07;
    if(mode == 0) {
        bgr1.enabled = 1;
        bgr1.chrbpp = 2;
        bgr1.pri0 = 8;
        bgr1.pri1 = 11;
        bgr2.enabled = 1;
        bgr2.chrbpp = 2;
        bgr2.pri0 = 7;
        bgr2.pri1 = 10;
        bgr3.enabled = 1;
        bgr3.chrbpp = 2;
        bgr3.pri0 = 2;
        bgr3.pri1 = 5;
        bgr4.enabled = 1;
        bgr4.chrbpp = 2;
        bgr4.pri0 = 1;
        bgr4.pri1 = 4;
    } else if(mode == 1) {
        bgr1.enabled = 1;
        bgr1.chrbpp = 4;
        bgr1.pri0 = 8;
        bgr1.pri1 = 11;
        bgr2.enabled = 1;
        bgr2.chrbpp = 4;
        bgr2.pri0 = 7;
        bgr2.pri1 = 10;
        bgr3.enabled = 1;
        bgr3.chrbpp = 2;
        bgr3.pri0 = 2;
        bgr3.pri1 = (regs.bgrmode & 0x08) ? 13 : 5;
        bgr4.enabled = 0;
    } else if(mode == 3) {
        bgr1.enabled = 1;
        bgr1.chrbpp = 8;
        bgr1.pri0 = 5;
        bgr1.pri1 = 11;
        bgr2.enabled = 1;
        bgr2.chrbpp = 4;
        bgr2.pri0 = 2;
        bgr2.pri1 = 8;
        bgr3.enabled = 0;
        bgr4.enabled = 0;
    }

    bgr1.mapadd = (regs.bgr1map & 0xfc) << 9;
    bgr1.mapsize_x = regs.bgr1map & 0x01;
    bgr1.mapsize_y = regs.bgr1map & 0x02;
    bgr1.chradd = (regs.bgr12chr & 0x0f) << 13;
    bgr1.chrsize = regs.bgrmode & 0x10;
    bgr1.offset_x = regs.bgr1off_x;
    bgr1.offset_y = regs.bgr1off_y;

    bgr2.mapadd = (regs.bgr2map & 0xfc) << 9;
    bgr2.mapsize_x = regs.bgr2map & 0x01;
    bgr2.mapsize_y = regs.bgr2map & 0x02;
    bgr2.chradd = (regs.bgr12chr & 0xf0) << 9;
    bgr2.chrsize = regs.bgrmode & 0x20;
    bgr2.offset_x = regs.bgr2off_x;
    bgr2.offset_y = regs.bgr2off_y;

    bgr3.mapadd = (regs.bgr3map & 0xfc) << 9;
    bgr3.mapsize_x = regs.bgr3map & 0x01;
    bgr3.mapsize_y = regs.bgr3map & 0x02;
    bgr3.chradd = (regs.bgr34chr & 0x0f) << 13;
    bgr3.chrsize = regs.bgrmode & 0x40;
    bgr3.offset_x = regs.bgr3off_x;
    bgr3.offset_y = regs.bgr3off_y;

    bgr4.mapadd = (regs.bgr4map & 0xfc) << 9;
    bgr4.mapsize_x = regs.bgr4map & 0x01;
    bgr4.mapsize_y = regs.bgr4map & 0x02;
    bgr4.chradd = (regs.bgr34chr & 0xf0) << 9;
    bgr4.chrsize = regs.bgrmode & 0x80;
    bgr4.offset_x = regs.bgr4off_x;
    bgr4.offset_y = regs.bgr4off_y;
}


/**
 * Draw a scanline of the specified background layer.
 *
 * @param bgr background properties
 * @param sy screen y coordinate of the scanline
 */
void R_DrawBgrScanline(background_t bgr, int sy) {
    if(!bgr.enabled) {
        return;
    }

    int tvfl, thfl, tpri, tpal, tchr;

    unsigned char cdata[bgr.chrbpp];
    int cx = bgr.offset_x & 0x07;

    int by = sy + bgr.offset_y;
    int ti = by >> (bgr.chrsize ? 4 : 3);

    for(int sx = -7; sx < SCREEN_WIDTH; sx++) {

        if(cx++ == 7) {
            cx = 0;

            int bx = sx + bgr.offset_x;
            int tj = bx >> (bgr.chrsize ? 4 : 3);

            int tn = ((ti & 0x1f) << 5) | (tj & 0x1f);
            tn |= bgr.mapsize_y ? ((ti & 0x20) << (bgr.mapsize_x ? 6 : 5)) : 0;
            tn |= bgr.mapsize_x ? ((tj & 0x20) << 5) : 0;

            int tadd = bgr.mapadd | (tn << 1);
            unsigned short tdata = mem.vram[tadd+0] | (mem.vram[tadd+1] << 8);
            tvfl = (tdata & 0x8000);
            thfl = (tdata & 0x4000);
            tpri = (tdata & 0x2000) ? bgr.pri1 : bgr.pri0;
            tpal = (tdata & 0x1c00) >> 10;
            tchr = (tdata & 0x03ff);

            int tx = bx & 0x07;
            int ty = by & 0x07;
            if(bgr.chrsize) {
                tchr += bx & 0x08 ? thfl ? 0 : 1 : thfl ? 1 : 0;
                tchr += by & 0x08 ? tvfl ? 0 : 16 : tvfl ? 16 : 0;
            }

            if(tvfl) {
                ty = 7 - ty;
            }

            int cadd = bgr.chradd + tchr*8*bgr.chrbpp + ty*2;
            for(int b = 0; b < bgr.chrbpp; b++) {
                cdata[b] = mem.vram[cadd + (b >> 1)*16 + (b & 0x01)];
            }

            if(thfl) {
                for(int b = 0; b < bgr.chrbpp; b++) {
                    cdata[b] = ((cdata[b] & 0xf0) >> 4) | ((cdata[b] & 0x0f) << 4);
                    cdata[b] = ((cdata[b] & 0xcc) >> 2) | ((cdata[b] & 0x33) << 2);
                    cdata[b] = ((cdata[b] & 0xaa) >> 1) | ((cdata[b] & 0x55) << 1);
                }
            }
        }

        unsigned char col = 0;
        for(int b = 0; b < bgr.chrbpp; b++) {
            col |= (cdata[b] & 0x80) ? (1 << b) : 0;
            cdata[b] <<= 1;
        }
        col += col ? (tpal << bgr.chrbpp) : 0;

        if(sx >= 0) {
            if(col && tpri > pix[sx].pri) {
                pix[sx].col = col;
                pix[sx].pri = tpri;
            }
        }
    }

}
