#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "types.h"

/* Maquina de estados Modo Debug 
 * STATE:   NORMAL -> DEBUG
 * 			    DEBUG -> NORMAL 
 *			    EXCEPCION -> DEBUG */
extern uint8_t STATE;

typedef struct str_register_states {
  /* registro de flags del procesador */
  uint32_t eflags;

  /* registros de control */
  uint32_t cr0;
  uint32_t cr2;
  uint32_t cr3;
  uint32_t cr4;

  /* selectores de segmento */
  uint32_t cs;
  uint32_t ds;
  uint32_t es;
  uint32_t fs;
  uint32_t gs;
  uint32_t ss;

  /* registros proposito general */
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  uint32_t esi;
  uint32_t edi;
  uint32_t ebp;

  uint32_t esp;
  uint32_t eip;

  /* esp al comienzo de la rutina de atencion de la excepcion */
  uint32_t esp0;

  //uint32_t backtrace;
} __attribute__((__packed__, aligned(8))) reg_states;


reg_states excep_state;

void load_excep_state(uint32_t* reg_vector);
void change_state(void);
void debug_exception(uint32_t);


#endif /* __DEBUG_H__ */