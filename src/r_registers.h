/**
 * r_registers.h
 * @shawngettler
 *
 * Hardware registers.
 */

#ifndef R_REGISTERS_H
#define R_REGISTERS_H



typedef struct {
    unsigned char bgrmode; // $2105 BGMODE

    unsigned char bgr1map; // $2107-0a BGnSC
    unsigned char bgr2map;
    unsigned char bgr3map;
    unsigned char bgr4map;

    unsigned char bgr12chr; // $210b-0c BGnmNBA
    unsigned char bgr34chr;

    unsigned short bgr1off_x; // $210d-14 BGnHOFS/BGnVOFS
    unsigned short bgr1off_y;
    unsigned short bgr2off_x;
    unsigned short bgr2off_y;
    unsigned short bgr3off_x;
    unsigned short bgr3off_y;
    unsigned short bgr4off_x;
    unsigned short bgr4off_y;
} registers_t;



#endif
