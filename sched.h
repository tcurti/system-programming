/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del scheduler.
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "types.h"
#include "player.h"

/* Estructuras Scheduler */
pl_id_t curr_pl; // Indicador del jugador que se haya en ejecucion actualmente

/* Funciones Scheduler */
void sched_init();
selec_t sched_next_task();
void sched_dealloc_task(pl_id_t pl, uint8_t tsk_id);
void sched_dealloc_current_task(void);

#endif //  __SCHED_H__
