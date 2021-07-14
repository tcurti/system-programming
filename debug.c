#include "debug.h"
#include "defines.h"
#include "screen.h"
#include "game.h"
#include "sched.h"

uint8_t STATE = NORMAL;
reg_states excep_state = {0};

void load_excep_state(uint32_t* reg_vector) {
	/* reg_vector = { 	cr0, cr2, cr3, cr4, 					(4)	
	 *					ds, es, fs, gs,							(8)
	 *					eax, ebx, ecx, edx, esi, edi, ebp, 		(15)
	 *					esp0								} 	(16)	*/

	/* cargar registro de flags */
	excep_state.eflags =  *(uint32_t*)(reg_vector[15] + sizeof(uint32_t)*3);

	/* cargar registros de control */
	for (int i = 0; i < 4; ++i) {
		*(&excep_state.cr0 + i) = reg_vector[i]; 
	}

	/* cargar selectores de segmento */
	excep_state.cs = *(uint32_t*)(reg_vector[15] + sizeof(uint32_t)*2);
	for (int i = 0; i < 4; ++i) {
		*(&excep_state.ds + i) = reg_vector[4 + i]; 
	}

	excep_state.ss = *(uint32_t*)(reg_vector[15] + sizeof(uint32_t)*5);

	/* cargar registros de proposito general */
	for (int i = 0; i < 7; ++i) {
		*(&excep_state.eax + i) = reg_vector[8 + i]; 
	}
	excep_state.eip = *(uint32_t*)(reg_vector[15] + sizeof(uint32_t)*1);	
	excep_state.esp = *(uint32_t*)(reg_vector[15] + sizeof(uint32_t)*4);

	/* cargar ptro a la pila de nivel 0 */
	excep_state.esp0 = 	reg_vector[15];
}

void change_state(void) {
	if (STATE == NORMAL) {
		STATE = DEBUG;

	} else if (STATE == DEBUG) {
		STATE = NORMAL;
		
	} else if (STATE == EXCEPCION) {
		STATE = DEBUG;
		/* quitar el mensaje de error */
		screen_restore();
		/* desalojar la tarea uqe produjo la excepcion */
		sched_dealloc_current_task();
	}
}

void debug_exception(uint32_t id) {
	STATE = EXCEPCION;

	screen_save();
	print_error_screen(id);
}