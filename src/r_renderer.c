/**
 * r_renderer.c
 * @shawngettler
 *
 * 2D background and sprite renderer intended to be compiled to WebAssembly.
 * Generally based on the SNES PPU, but not all background modes or color
 * modes are implemented and various quirks have not been recreated (this is
 * not an emulator).
 */

#include "m_memory.h"
#include "r_registers.h"
#include "r_backgrounds.h"

#include "r_renderer.h"



rendermem_t mem;

pixel_t pix[SCREEN_WIDTH];



/**
 * Draw the screen based on the contents of the renderer memory.
 *
 * @param img pointer to output buffer
 * @param vram pointer to VRAM buffer
 * @param oaram pointer to OAM buffer
 * @param cgram pointer to CG buffer
 * @param regs pointer of registers
 */
void R_DrawScreen(color32_t *img, unsigned char *vram, unsigned char *oaram, unsigned char *cgram, registers_t *regs) {

    mem.vram = vram;
    mem.oaram = oaram;
    mem.cgram = cgram;

    // precompute 32-bit colors for current palette
    color32_t truecol[256];
    for(int i = 0; i < 256; i++) {
        unsigned short bgr = mem.cgram[i*2+0] | (mem.cgram[i*2+1] << 8);
        truecol[i].r = ((bgr << 3) & 0xf8) | ((bgr >> 2) & 0x07);
        truecol[i].g = ((bgr >> 2) & 0xf8) | ((bgr >> 7) & 0x07);
        truecol[i].b = ((bgr >> 7) & 0xf8) | ((bgr >> 12) & 0x07);
        truecol[i].a = 255;
    }

    for(int sy = 0; sy < SCREEN_HEIGHT; sy++) {
        R_ReadBgrRegisters(regs[0]);

        R_DrawBgrScanline(bgr1, sy);
        R_DrawBgrScanline(bgr2, sy);
        R_DrawBgrScanline(bgr3, sy);
        R_DrawBgrScanline(bgr4, sy);

        for(int sx = 0; sx < SCREEN_WIDTH; sx++) {
            img[sy*256+sx] = truecol[pix[sx].col];

            pix[sx].col = 0;
            pix[sx].pri = 0;
        }
    }

}
