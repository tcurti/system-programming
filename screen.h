/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler
*/

#ifndef __SCREEN_H__
#define __SCREEN_H__

/* Definicion de la pantalla */
#define VIDEO_FILS 50
#define VIDEO_COLS 80

#include "colors.h"
#include "defines.h"
#include "stdint.h"
#include "types.h"
#include "debug.h"


/* Estructura de para acceder a memoria de video */
typedef struct ca_s {
  uint8_t c;
  uint8_t a;
} ca;

/* Funciones para escribir la memoria de video */
void printScanCode(uint8_t scancode);
void print(const char* text, uint32_t x, uint32_t y, uint16_t attr);
void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr);
void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr);

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr);

void screen_draw_layout(void);


/* Pantalla de error modo Debug */
paddr_t screen_backup[2];
void screen_save(void);
void screen_restore(void);

void imprimir_excepcion(uint32_t id);

void print_gp_reg(uint32_t x, uint32_t y, uint16_t attr);
void print_seg_sel(uint32_t x, uint32_t y, uint16_t attr);
void print_eflag(uint32_t x, uint32_t y, uint16_t attr);
void print_creg(uint32_t x, uint32_t y, uint16_t attr);
void print_stack(uint32_t x, uint32_t y, uint16_t attr);

void print_error_screen(uint32_t id);

#endif //  __SCREEN_H__
