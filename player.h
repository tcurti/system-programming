#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "types.h"

typedef enum e_pl_id {
  Rick,
  Morty
} pl_id_t;

typedef struct str_map_pos {
	uint32_t x;
	uint32_t y;
}  __attribute__((__packed__, aligned(8))) map_pos;

typedef struct str_meeseek {
	map_pos pos;
	uint8_t p:1;
	uint8_t used_p_g:1;
	uint32_t age;
}  __attribute__((aligned(8))) meeseek;

typedef struct str_player{
	uint32_t score;			// Puntaje del jugador
	meeseek mee_info[10];	// Información acerca de los 10 Meeseeks asociados
	uint8_t mee_count;		// Cantidad de meeseeks vivos	
	uint8_t curr_task;		// Indicador de la tarea actual (10: jugador, 0-9: Meeseek)
} __attribute__((aligned(8))) player;



void init_players(void);

bool current_tsk_is_player(pl_id_t pl);
bool is_rick(pl_id_t pl);
bool player_task_is_alive(pl_id_t pl, uint8_t tsk_id);
bool player_task_available(pl_id_t pl);
bool player_has_tasks(pl_id_t pl);
bool player_task_used_p_g(pl_id_t pl, uint8_t tsk_id);

uint8_t player_curr_task(pl_id_t pl);
const map_pos* player_task_pos(pl_id_t pl, uint8_t tsk_id);
void player_add_task(pl_id_t pl, uint8_t tsk_id, uint32_t x, uint32_t y);
uint32_t player_score(pl_id_t pl);
uint32_t player_task_age(pl_id_t pl, uint8_t tsk_id);

pl_id_t next_player(pl_id_t pl);
void player_next_task(pl_id_t pl);
void player_kill_task(pl_id_t pl, uint8_t tsk_id);
void player_task_move(pl_id_t pl, uint8_t tsk_id, int x, int y);
void player_add_score(pl_id_t pl);
void player_task_use_p_g(pl_id_t pl, uint8_t tsk_id);
void player_task_update_age(pl_id_t pl);

uint32_t get_gdt_idx_tss(pl_id_t pl, uint8_t tsk_id);


#endif /*__PLAYER_H__*/