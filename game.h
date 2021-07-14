/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de las rutinas asociadas al juego.
*/

#include "prng.h"
#include "types.h"
#include "screen.h"
#include "player.h"

#ifndef __GAME_H__
#define __GAME_H__

typedef struct str_seed {
	map_pos pos;
	uint8_t p:1;
}  __attribute__((__packed__, aligned(8))) seed;

/* Funciones del juego */
void game_init(void);
uint32_t init_meeseek(uint32_t code_start, uint32_t x, uint32_t y);
void move_current_task(int dx, int dy);
void look(int* x_y);
void teleport_meeseek(void);
void end_game(void);

void rm_meeseek_from_map(pl_id_t pl, uint8_t mee);
void update_task_age(void);



#endif //  __GAME_H__
