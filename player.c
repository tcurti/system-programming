#include "player.h"
#include "defines.h"

player player_info[PLAYER_COUNT];

void init_players(void) {
	player_info[Rick] = (player){
		.score = 0,
		.mee_count = 0,
		.curr_task = 10
	};
	for (int i = 0; i < 10; ++i) {
		player_info[Rick].mee_info[i] = (meeseek){0};
	}

	player_info[Morty] = player_info[Rick];
}

bool current_tsk_is_player(pl_id_t pl) {
	return player_info[pl].curr_task == 10;
}

bool is_rick(pl_id_t pl) {
	return pl == Rick;
}

bool player_task_is_alive(pl_id_t pl, uint8_t tsk_id) {
	bool res;
	if (tsk_id == 10) {
		res = true;
	} else {
		res = player_info[pl].mee_info[tsk_id].p == 1;
	}

	return res;
}

bool player_task_available(pl_id_t pl) {
	return player_info[pl].mee_count < 10;
}

bool player_has_tasks(pl_id_t pl) {
	return player_info[pl].mee_count > 0;
}

bool player_task_used_p_g(pl_id_t pl, uint8_t tsk_id) {
	return player_info[pl].mee_info[tsk_id].used_p_g == 1;
}




uint8_t player_curr_task(pl_id_t pl) {
	return player_info[pl].curr_task;
}

const map_pos* player_task_pos(pl_id_t pl, uint8_t tsk_id) {
	return &(player_info[pl].mee_info[tsk_id].pos);
}

uint32_t player_score(pl_id_t pl) {
	return player_info[pl].score;
}

uint32_t player_task_age(pl_id_t pl, uint8_t tsk_id) {
	return player_info[pl].mee_info[tsk_id].age;
}




pl_id_t next_player(pl_id_t pl) {
	return (pl+1)%PLAYER_COUNT;
}

void player_next_task(pl_id_t pl) {	
	player* info = &(player_info[pl]);

	do {
		info->curr_task = (info->curr_task+1) % 11;
	} while (!player_task_is_alive(pl, info->curr_task));
}

void player_kill_task(pl_id_t pl, uint8_t tsk_id) {
	player_info[pl].mee_info[tsk_id].p = 0;
	player_info[pl].mee_count--;
}

void player_add_task(pl_id_t pl, uint8_t tsk_id, uint32_t x, uint32_t y) {
	player_info[pl].mee_info[tsk_id] = (meeseek){
		.pos = (map_pos){x, y}, 
		.p = 1,
		.used_p_g = 0,
		.age = 0
	};

	player_info[pl].mee_count++;
}

void player_task_move(pl_id_t pl, uint8_t tsk_id, int x, int y) {
	player_info[pl].mee_info[tsk_id].pos = (map_pos){x, y};
}

void player_add_score(pl_id_t pl) {
	player_info[pl].score += REWARD;
}

void player_task_use_p_g(pl_id_t pl, uint8_t tsk_id) {
	player_info[pl].mee_info[tsk_id].used_p_g = 1;
}

void player_task_update_age(pl_id_t pl) {
	if (player_info[pl].curr_task != 10)
		player_info[pl].mee_info[player_info[pl].curr_task].age++;
}




uint32_t get_gdt_idx_tss(pl_id_t pl, uint8_t tsk_id) {
	return GDT_IDX_TSS_RICK + 11*pl + ((tsk_id+1) % 11);
}