/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del scheduler
*/

#include "sched.h"
#include "defines.h"
#include "gdt.h"
#include "mmu.h"
#include "tss.h"
#include "screen.h"
#include "game.h"
#include "i386.h"

void init_tss_desc() {
	/* definir descriptores tss de las tareas */
	// solicitar pagina del area libre donde se almacenaran los tss
	paddr_t tss_start = mmu_next_free_kernel_page();

	// definir descriptor tss de la tarea inicial en la gdt
	gdt[GDT_IDX_TSS_INICIAL].base_15_0 = tss_start & 0xFFFF;
	gdt[GDT_IDX_TSS_INICIAL].base_23_16 = (tss_start >> 16) & 0xFF;
	gdt[GDT_IDX_TSS_INICIAL].base_31_24 = (tss_start >> 24) & 0xFF;

	// definir descriptores tss de las demás tareas en la gdt
	for (int i=1; i<24; ++i) {
		// copiar entrada tarea inicial (mismos flags y limite)
		gdt[GDT_IDX_TSS_INICIAL + i] = gdt[GDT_IDX_TSS_INICIAL];

		// cambiar la base de la tss
		gdt[GDT_IDX_TSS_INICIAL + i].base_15_0 += TSS_SIZE*i;
	
		// invalidar entrada
		gdt[GDT_IDX_TSS_INICIAL+i].p = 0;
	}
}

void sched_init(void) {

	init_tss_desc();

	curr_pl = Rick;

	/* Pedir 2 paginas del kernel para el Backup de la pantalla (Modo DEBUG) */
	screen_backup[0] = mmu_next_free_kernel_page();
	screen_backup[1] = mmu_next_free_kernel_page();
}

selec_t sched_next_task(void) {
	/* Cambiar de jugador */
	curr_pl = next_player(curr_pl);

	/* Avanzar a la proxima tarea del jugador */

	player_next_task(curr_pl);

	uint32_t gdt_idx_tss = get_gdt_idx_tss(curr_pl, player_curr_task(curr_pl));

	return (gdt_idx_tss << 3);
}

void sched_dealloc_current_task() {
	sched_dealloc_task(curr_pl, player_curr_task(curr_pl));
}

void sched_dealloc_task(pl_id_t pl, uint8_t tsk_id) {
	/* Desmapear paginas de la tarea*/
	// calcular direccion tss de la tareas
	uint32_t gdt_idx_tss_current = get_gdt_idx_tss(pl, tsk_id);
	tss_t* pl_tss = get_tss_base(gdt_idx_tss_current);

	if (current_tsk_is_player(pl)) {
		// la tarea actual es principal
		for (int i = 0; i < 4; ++i) {
			mmu_unmap_page(pl_tss->cr3, TASK_CODE_VIRTUAL + i*0x1000);
		}

		end_game();

	} else {
		// inhabilitar tarea en el vector de tareas vivas
		player_kill_task(pl, tsk_id);
		
		// la tarea actual es de tipo meeseek
		vaddr_t vir_start = task_vir_start(tsk_id);
		for (int i = 0; i < 2; ++i) {
			mmu_unmap_page(pl_tss->cr3, vir_start);
			mmu_unmap_page(pl_tss->cr3, vir_start+PAGE_SIZE);
		}
		
		// La quitamos del mapa
		rm_meeseek_from_map(pl, tsk_id);
	}

	// deshabilitar descriptor tss en la gdt
	gdt[gdt_idx_tss_current].p = 0;
}