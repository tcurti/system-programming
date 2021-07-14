/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "types.h"
#include "game.h"
#include "sched.h"
#include "defines.h"
#include "tss.h"
#include "i386.h"
#include "mmu.h"
#include "math.h"

/* Variables "privadas" del juego */
seed seed_info[40];
uint32_t seeds_num;

/* Declaracion funciones auxiliares */
void init_seeds();
void init_screen();
bool is_seed(uint32_t x, uint32_t y);
bool collide(pl_id_t pl1, uint8_t mee1, pl_id_t pl2, uint8_t mee2);
uint32_t closest_seed(int x, int y);
int manhattan_dst(int x1, int y1, int x2, int y2 );
uint32_t move_meeseek(int dx, int dy, pl_id_t pl, uint8_t mee_id);
void move_mee_code(int x, int y, pl_id_t pl, uint8_t mee_id);
void move_teleport(int x, int y, pl_id_t pl, uint8_t mee_id);
bool valid_movement(int dx, int dy, pl_id_t pl, uint8_t mee_id);

/* Declaracion de funciones de la pantalla */
void print_seeds();
void print_scores();
void print_mee_indicators();
void print_meeseek(pl_id_t pl, uint8_t mee_id);
void rm_seed_from_map(uint32_t x, uint32_t y);


/* ============================== 
   ==== Funciones del Juego  ==== 
   ============================== */

void game_init(void) {	
	init_seeds();
	init_players();
	init_screen();
}

/* Funciones para inicializar Meeseeks */
uint32_t init_meeseek(uint32_t code_start, uint32_t x, uint32_t y) {
	/* Verificar:
	 * 		- que el puntero al codigo sea valido 
	 *		- que la posicion sea valida
	 *		- que el servicio haya sido llamado por un jugador */
	bool valid_code = 0x1D00000<=code_start && code_start<0x1D04000;
	bool valid_pos = x<SIZE_M && y<SIZE_N;
	bool called_by_player = current_tsk_is_player(curr_pl);
	if (!valid_code || !valid_pos || !called_by_player)
		sched_dealloc_current_task();

	// Chequear que haya espacio para crearlo
	if (!player_task_available(curr_pl)) {
		return 0;
	}

	// Chequear si en la posicion había una megasemilla
	if (is_seed(x, y)) {
		player_add_score(curr_pl);
		// quitar la megasemilla del mapa
		rm_seed_from_map(x, y);
		// Actualizar puntajes
		print_scores();

		if (seeds_num == 0)
			end_game();

		return 0;
	}

	// Inicializar tss del meeseeks y copiar el codigo
	uint8_t mee_id = 0;
	while (player_task_is_alive(curr_pl, mee_id)) {
		mee_id++;
	}

	tss_init_meeseek((paddr_t)code_start, x, y, curr_pl, mee_id);

	// Inicializamos estructura del meeseek actual
	player_add_task(curr_pl, mee_id, x, y);

	// Imprimirlo en pantalla
	print_meeseek(curr_pl, mee_id);

	uint32_t dir_meeseek = 0x8000000 + mee_id*2000;

	return dir_meeseek;
}

/* Funciones para mover meeseeks */

void move_mee_code(int x, int y, pl_id_t pl, uint8_t mee_id) {

	tss_t* tss_owner = get_tss_base(get_gdt_idx_tss(pl, mee_id));

	const map_pos* src_pos = player_task_pos(pl, mee_id);
	paddr_t src_start = 0x400000 + (src_pos->x + src_pos->y*80)*0x2000;

	paddr_t dst_start = 0x400000 + (x + y*80)*0x2000;

	uint32_t vir_start = task_vir_start(mee_id);

	mmu_move_task(tss_owner->cr3, src_start, dst_start, vir_start, 2);
}

// PRE = { La tarea actual es un meeseek }
uint32_t move_meeseek(int dx, int dy, pl_id_t pl, uint8_t mee_id) {
	const map_pos* current_pos = player_task_pos(pl, mee_id);

	int x = mod(current_pos->x + dx, SIZE_M);
	int y = mod(current_pos->y + dy, SIZE_N);


	// Chequear movimiento válido [EJ. 8L]
	if (!valid_movement(dx, dy, pl, mee_id)) {
		return 0;
	}

	// Chequear si habia una megasemilla en la posicion final
	if (is_seed(x,y)) {
		/* asimilar semilla */
		rm_seed_from_map(x,y);

		/* resolver puntajes */
		player_add_score(pl);
		print_scores();

		/* finalizar la tarea */
		sched_dealloc_task(pl, mee_id);

		/* si no hay mas semillas, finaliza el juego */
		if (seeds_num == 0)
			end_game();

		return 1;
	}

	// Mover tarea meeseeks a la posicion correspondiente
	move_mee_code(x, y, pl, mee_id);

	// Actualizar pantalla
	rm_meeseek_from_map(pl, mee_id);
	player_task_move(pl, mee_id, x, y);
	print_meeseek(pl, mee_id);

	return 1;
}

void move_current_task(int dx, int dy) {
	// Chequear si la tarea que llamó la interrupcion fue Rick o Morty
	if (current_tsk_is_player(curr_pl)) {
		// Si un jugador intenta moverse, termina el juego
  		sched_dealloc_current_task();
	}

	move_meeseek(dx, dy, curr_pl, player_curr_task(curr_pl));
}

void move_teleport(int x, int y, pl_id_t pl, uint8_t mee_id) {
	// Chequear si habia una megasemilla en la posicion final
	if (is_seed(x,y)) {
		/* asimilar semilla */
		rm_seed_from_map(x,y);

		/* resolver puntajes */
		player_add_score(pl);
		print_scores();

		/* finalizar la tarea */
		sched_dealloc_task(pl, mee_id);

		/* si no hay mas semillas, finaliza el juego */
		if (seeds_num == 0)
			end_game();

		return;
	}
	// Mover tarea meeseeks a la posicion correspondiente
	move_mee_code(x, y, pl, mee_id);

	// Actualizar pantalla
	rm_meeseek_from_map(pl, mee_id);
	player_task_move(pl, mee_id, x, y);
	print_meeseek(pl, mee_id);
}

/* Funciones para Look */

void look(int* x_y) {
	// Chequear si la tarea que llamo fue Rick o Morty
	if (current_tsk_is_player(curr_pl)) {
		x_y[0] = -1;
		x_y[1] = -1; 
		return;
	}
	// Buscar semilla mas proxima
	const map_pos* mee_pos = player_task_pos(curr_pl, player_curr_task(curr_pl));
	uint32_t mx = mee_pos->x; uint32_t my = mee_pos->y;
	uint32_t seed_idx = closest_seed(mx, my);

	// Obtenemos desplazamiento relativo a semilla mas proxima
	int x_res = seed_info[seed_idx].pos.x - mx;
	int y_res = seed_info[seed_idx].pos.y - my;
	x_y[0] = x_res;
	x_y[1] = y_res;
}

uint32_t closest_seed(int x, int y) {
	
	uint32_t closest = 0;
	int min_dst =  121;
	for (int i = 0; i < 40; ++i) {
		if (seed_info[i].p == 1) {
			int dst_to_seed = manhattan_dst(x, y, seed_info[i].pos.x, seed_info[i].pos.y);
			if (dst_to_seed < min_dst) {
				closest = i;
				min_dst = dst_to_seed;
			}
		}
	}

	return closest;
}

int manhattan_dst(int x1, int y1, int x2, int y2 ) {
	return abs(x2-x1) + abs(y2-y1);
}


/* Funciones para use_portal_gun */
void teleport_meeseek(void) {

	if (current_tsk_is_player(curr_pl) 								|| 
		player_task_used_p_g(curr_pl, player_curr_task(curr_pl))	) 
		{ return; }

	pl_id_t enemy = next_player(curr_pl);
	if (player_has_tasks(enemy)) {
		uint8_t rand_mee_id;
		do {
			rand_mee_id = (rand() % 10);
		} while (!player_task_is_alive(enemy, rand_mee_id));
		uint32_t rand_dx = rand() % SIZE_M;
		uint32_t rand_dy = rand() % SIZE_N;

		move_teleport(rand_dx, rand_dy, enemy, rand_mee_id);
		player_task_use_p_g(curr_pl, player_curr_task(curr_pl));
	}
}




/* ============================== 
   ==== Funciones Auxiliares ==== 
   ============================== */

void init_seeds(void) {
	/* Configurar cantidad total de semillas */
	seeds_num = 40;

	/* Crear seeds en posiciones aleatorias */
	uint32_t x_rand = rand() % SIZE_M;
	uint32_t y_rand = rand() % SIZE_N;
	for (int i = 0; i < 40; ++i) {
		while(is_seed(x_rand, y_rand)) {
			x_rand = rand() % SIZE_M;
			y_rand = rand() % SIZE_N;
		}
		seed_info[i].pos.x = x_rand;
		seed_info[i].pos.y = y_rand;
		seed_info[i].p = 1;
	}

}

bool is_seed(uint32_t x, uint32_t y) {
	bool res = false;
	for (int i = 0; i < 40; ++i) {
		if (seed_info[i].pos.x == x && seed_info[i].pos.y == y && seed_info[i].p == 1) {
			res = true;
			break;
		}
	}
	return res;
}

bool valid_movement(int dx, int dy, pl_id_t pl, uint8_t mee_id) {
	// Calculamos maxima distancia permitida
	int max_mov = 7;
	uint32_t penalty =  player_task_age(pl, mee_id) / 2;
	if (penalty > 6) {
		max_mov = 1;
	} else {
		max_mov = max_mov - penalty;
	}
	// Chequeamos validez del movimiento pedido
	int req_move = abs(dx) + abs(dy);
	return max_mov >= req_move;
}

void end_game(void) {
	bool fin_by_exc = seeds_num != 0;

	char* winner;
	if (fin_by_exc) {
		if (is_rick(curr_pl)) {
			winner = "El ganador es Morty!";
		} else {
			winner = "El ganador es Rick!";
		}

	} else {
		if (player_score(Rick) > player_score(Morty)) {
			winner = "El ganador es Rick!";
		} else if (player_score(Rick) == player_score(Morty)) {
			winner = "Empate!";
		} else {
			winner = "El ganador es Morty!";
		}
	}

	print(winner, 30, 20, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
	print("GAME OVER", 35, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
	while(1);
}

/* ============================== 
   ====  Funciones Pantalla  ==== 
   ============================== */

void init_screen(void) {	
	print_seeds();
	print_scores();
	print_mee_indicators();
}

void print_seeds(void) {
	for (int i = 0; i < 40; ++i) {
		print("S", seed_info[i].pos.x, seed_info[i].pos.y, C_FG_LIGHT_BROWN | C_BG_GREEN);
	}
}

void print_scores(void) {
	print_dec(player_score(Rick), 8, 5, 45, C_FG_WHITE | C_BG_BLUE);
	print_dec(player_score(Morty), 8, 67, 45, C_FG_WHITE | C_BG_RED);
}

void print_mee_indicators() {
	for (int i = 0; i < 2; ++i) {
		int32_t attr = ((i==0)? C_FG_LIGHT_BLUE : C_FG_LIGHT_RED) | C_BG_BLACK;

		for (int j = 1; j <= 10; ++j) {
			print_dec(j, 2, 19 + 4*j, 42 + 4*i, attr);
			print("):", 19 + 4*j, 42 + 4*i + 1, C_FG_WHITE | C_BG_BLACK);
		}
	}
}

void print_meeseek(pl_id_t pl, uint8_t mee_id) {
	uint8_t mee_atr = is_rick(pl)? C_FG_BLUE : C_FG_RED;

	const map_pos* pos = player_task_pos(pl, mee_id);

	print("M", pos->x, pos->y, mee_atr | C_BG_GREEN);	

	/* actualizar indicador */
	print("(", 19 + 4*(mee_id + 1), 43 + 4*(is_rick(pl)? 0:1), C_FG_WHITE | C_BG_BLACK);
}

void rm_meeseek_from_map(pl_id_t pl, uint8_t mee) {
	/* Identificar colision con otro meeseek */
	uint8_t pl_2 = pl;
	uint8_t mee_2 = mee;

	for (uint8_t pl_i = 0; pl_i < PLAYER_COUNT; ++pl_i) {
		for (uint8_t mee_i = 0; mee_i < 10; ++mee_i) {
			bool is_different = (pl_i!=pl || mee_i!=mee);
			
			if (player_task_is_alive(pl_i, mee_i) 	&& 
				is_different 						&& 
				collide(pl, mee, pl_i, mee_i)		)
			{
				pl_2 = pl_i;
				mee_2 = mee_i;
			}
		}
	}

	/* En caso de haber colision, imprimir el otro meeseek */
	bool there_was_another = pl!=pl_2 || mee!=mee_2;

	if (there_was_another) {
		uint8_t mee_attr = is_rick(pl_2)? C_FG_BLUE : C_FG_RED;
		const map_pos* mee_pos = player_task_pos(pl_2, mee_2);

		print("M", mee_pos->x, mee_pos->y, mee_attr | C_BG_GREEN);
	} else {
		const map_pos* mee_pos = player_task_pos(pl, mee);

		//if (mee_pos->x == 1 && mee_pos->y == 0)
		//	breakpoint();

		print(" ", mee_pos->x, mee_pos->y, C_FG_GREEN | C_BG_GREEN);
	}


	/* Actualizar el indicar del meeseek */
	print(")", 19 + 4*(mee + 1), 43 + 4*(is_rick(pl)? 0:1), C_FG_WHITE | C_BG_BLACK);
}

bool collide(pl_id_t pl1, uint8_t mee1, pl_id_t pl2, uint8_t mee2) {
	const map_pos* mee1_pos = player_task_pos(pl1, mee1);
	const map_pos* mee2_pos = player_task_pos(pl2, mee2);

	return (mee1_pos->x==mee2_pos->x) && (mee1_pos->y==mee2_pos->y);
}


void rm_seed_from_map(uint32_t x, uint32_t y) {
	// Quitarla del vector de semillas
	for (int i = 0; i < 40; ++i) {
		if (seed_info[i].pos.x == x && seed_info[i].pos.y == y) {
			seed_info[i].p = 0;
			seeds_num--;
			break;
		}
	}
	// Quitarla del mapa
	print(" ", x, y, C_FG_GREEN | C_BG_GREEN);
}