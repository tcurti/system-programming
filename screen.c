/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del scheduler
*/

#include "screen.h"
#include "sched.h"
#include "player.h"

void printScanCode(uint8_t scancode) {
  if(!(scancode & 0x80)) {
    if ((0x02 <= scancode) & (scancode <= 0x0A)) {
      uint8_t prt = scancode - 1;
      print_hex(prt, 1, 80-2, 2, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
    } else if (scancode == 0x0B) {
      print_hex(0x00, 1, 80-2, 2, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
    } 
  }
}

void print(const char* text, uint32_t x, uint32_t y, uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  for (i = 0; text[i] != 0; i++) {
    p[y][x].c = (uint8_t)text[i];
    p[y][x].a = (uint8_t)attr;
    x++;
    if (x == VIDEO_COLS) {
      x = 0;
      y++;
    }
  }
}

void print_dec(uint32_t numero, uint32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  uint32_t i;
  uint8_t letras[16] = "0123456789";

  for (i = 0; i < size; i++) {
    uint32_t resto = numero % 10;
    numero = numero / 10;
    p[y][x + size - i - 1].c = letras[resto];
    p[y][x + size - i - 1].a = attr;
  }
}

void print_hex(uint32_t numero, int32_t size, uint32_t x, uint32_t y,
               uint16_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO; // magia
  int32_t i;
  uint8_t hexa[8];
  uint8_t letras[16] = "0123456789ABCDEF";
  hexa[0] = letras[(numero & 0x0000000F) >> 0];
  hexa[1] = letras[(numero & 0x000000F0) >> 4];
  hexa[2] = letras[(numero & 0x00000F00) >> 8];
  hexa[3] = letras[(numero & 0x0000F000) >> 12];
  hexa[4] = letras[(numero & 0x000F0000) >> 16];
  hexa[5] = letras[(numero & 0x00F00000) >> 20];
  hexa[6] = letras[(numero & 0x0F000000) >> 24];
  hexa[7] = letras[(numero & 0xF0000000) >> 28];
  for (i = 0; i < size; i++) {
    p[y][x + size - i - 1].c = hexa[i];
    p[y][x + size - i - 1].a = attr;
  }
}

void screen_draw_box(uint32_t fInit, uint32_t cInit, uint32_t fSize,
                     uint32_t cSize, uint8_t character, uint8_t attr) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  uint32_t f;
  uint32_t c;
  for (f = fInit; f < fInit + fSize; f++) {
    for (c = cInit; c < cInit + cSize; c++) {
      p[f][c].c = character;
      p[f][c].a = attr;
    }
  }
}

/* Pantalla de error modo Debug */
void screen_save(void) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  ca* iter = (ca*)screen_backup[0];

  for (int i = 0; i < VIDEO_FILS; ++i) {
    for (int j = 0; j < VIDEO_COLS; ++j) {
      *iter = p[i][j];

      iter += sizeof(ca);
    }
  }
}

void screen_restore(void) {
  ca(*p)[VIDEO_COLS] = (ca(*)[VIDEO_COLS])VIDEO;
  ca* iter = (ca*)screen_backup[0];

  for (int i = 0; i < VIDEO_FILS; ++i) {
    for (int j = 0; j < VIDEO_COLS; ++j) {
      p[i][j] = *iter;

      iter += sizeof(ca);
    }
  }
}

void print_gp_reg(uint32_t x, uint32_t y, uint16_t attr){
  char* gp_reg[9] = {"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp", "eip"};

  for(int i = 0; i < 9; ++i) {
    print(gp_reg[i], x , y + i*2, attr);
    
    uint32_t reg_val = *(&excep_state.eax + i);
    print_hex(reg_val, 8, x + 4, y + i*2, attr);
  }
}

void print_seg_sel(uint32_t x, uint32_t y, uint16_t attr){
  char* seg_reg[6] = {"cs", "ds", "es", "fs", "gs", "ss"};

  for(int i = 0; i < 6; ++i) {
    print(seg_reg[i], x , y + i*2, attr);
    
    uint32_t reg_val = *(&excep_state.cs + i);
    print_hex(reg_val, 8, x + 4, y + i*2, attr);  
  }
}

void print_eflag(uint32_t x, uint32_t y, uint16_t attr){
  print("eflags", x, y, attr);
  print_hex(excep_state.eflags, 8, x + 8, y, attr);
}

void print_creg(uint32_t x, uint32_t y, uint16_t attr){
  char* creg_reg[5] = {"cr0", "cr2", "cr3", "cr4", "err"};
  for (int i = 0; i < 4; ++i) {
    print(creg_reg[i], x, y + i*2, attr);

    uint32_t reg_val = *(&excep_state.cr0 + i);
    print_hex(reg_val, 8, x + 4, y + i*2, attr);
  }

  /* codigo de error de la excepcion*/
  print(creg_reg[4], x, y + 4*2, attr);
  uint32_t error_code = *(uint32_t*)(excep_state.esp0);
  print_hex(error_code, 8, x + 4, y + 4*2, attr);
}

void print_stack(uint32_t x, uint32_t y, uint16_t attr){
  print("stack", x, y, attr);
  
  uint32_t* esp = (uint32_t*)(excep_state.esp);
  uint32_t* ebp = (uint32_t*)(excep_state.ebp);

  for (int i = 0; i<3 && (esp + i < ebp); ++i) {
    uint32_t stack_val = *(esp + i);
    print_hex(stack_val, 8, x, y + (i+1)*2, attr);
  }  
}


void print_backtrace(uint32_t x, uint32_t y, uint16_t attr) {
  print("backtrace", x, y, attr);
  
  /* ebp actual */
  uint32_t* ebp = (uint32_t*)(excep_state.ebp);

  /* valor inicial del ebp al alojar la tarea por primera vez */
  uint32_t* ebp_limit;
  if (current_tsk_is_player(curr_pl))
    /* la tarea actual es de tipo jugador */
    ebp_limit = (uint32_t*) 0x1D04000;
  else 
    //la tarea actual es de tipo meeseek 
    ebp_limit = (uint32_t*) (0x8002000 + player_curr_task(curr_pl)*2000);

  /* == STACK == 
   * [ebp + 0] : ebp (anterior)
   * [ebp + 4] : direccion de retorno   */
  for (int i = 1; i<=3 && (ebp != ebp_limit); ++i) {
    uint32_t ret_p = *(ebp + 1);
    print_hex(ret_p, 8, x, y + i*2, attr);

    ebp = (uint32_t*)(*ebp);
  }
}



void print_error_screen(uint32_t id) {
  imprimir_excepcion(id);

  print_gp_reg(2, 2, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
  print_seg_sel(3, 20, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
  print_eflag(2, 32, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
  print_creg(42, 2, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
  print_stack(40, 12, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
  print_backtrace(40, 24, C_FG_DARK_GREY | C_BG_LIGHT_GREY);
}

void imprimir_excepcion(uint32_t codigo) {
    if(codigo == 0) {
    print("EXCEPCION NRO 0: Error de division (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 1) {
    print("EXCEPCION NRO 1: Excepcion de debugging (fault/trap)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 2) {
    print("EXCEPCION NRO 2: Interrupción externa no enmascarable NMI (interrupt)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 3) {
    print("EXCEPCION NRO 3: Breakpoint (trap)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 4) {
    print("EXCEPCION NRO 4: Overflow (trap)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 5) {
    print("EXCEPCION NRO 5: Rango excedido (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 6) {
    print("EXCEPCION NRO 6: Codigo de operación invalido (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 7) {
    print("EXCEPCION NRO 7: Dispositivo no disponible (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 8) {
    print("EXCEPCION NRO 8: Double fault (abort)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 9) {
    print("EXCEPCION NRO 9: Segmento de coprocesador invadido (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 10) {
    print("EXCEPCION NRO 10: TSS invalido (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 11) {
    print("EXCEPCION NRO 11: Segmento no presente (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 12) {
    print("EXCEPCION NRO 12: Falta en el segmento del stack (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 13) {
    print("EXCEPCION NRO 13: General Protection (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 14) {
    print("EXCEPCION NRO 14: Falta de paginacion (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 15) {
    print("EXCEPCION NRO 15: Reservada de Intel (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 16) {
    print("EXCEPCION NRO 16: Error en la FPU (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 17) {
    print("EXCEPCION NRO 17: Alignment Check (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 18) {
    print("EXCEPCION NRO 18: Machine check (abort)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 19) {
    print("EXCEPCION NRO 19: Excepcion de SIMD FP (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 20) {
    print("EXCEPCION NRO 20: Excepcion de virtualizacion (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
    if(codigo == 21) {
    print("EXCEPCION NRO 21: Excepcion de proteccion de control (fault)", 0, 0, C_FG_DARK_GREY | C_BG_LIGHT_GREY); 
    }
}