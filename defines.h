/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__
/* MMU */
/* -------------------------------------------------------------------------- */

#define MMU_P (1 << 0)
#define MMU_W (1 << 1)
#define MMU_U (1 << 2)

#define PAGE_SIZE 4096

/* Informacion del juego */
/* -------------------------------------------------------------------------- */
#define PLAYER_COUNT 2

// Dimensiones del Mapa
#define SIZE_N 40	// # de filas		(y)
#define SIZE_M 80	// # de columnas	(x)

// Puntos por semilla
#define REWARD 425

/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_IDX_NULL_DESC 0
#define GDT_COUNT         39



/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC (GDT_IDX_NULL_DESC << 3)

#define GDT_IDX_CS_0_DESC 10  // Necesario para entrar a modo protegido
#define GDT_IDX_CS_3_DESC 11
#define GDT_IDX_DS_0_DESC 12  // Necesario para entrar a modo protegido
#define GDT_IDX_DS_3_DESC 13

#define GDT_IDX_PANT_0_DESC 14

#define GDT_IDX_TSS_INICIAL 15
#define GDT_IDX_TSS_IDLE    16

#define GDT_IDX_TSS_RICK 17
#define GDT_IDX_TSS_MEE_RICK_START 18

#define GDT_IDX_TSS_MORTY 28
#define GDT_IDX_TSS_MEE_MORTY_START 29

/* Direcciones de memoria */
/* -------------------------------------------------------------------------- */

// direccion fisica de comienzo del bootsector (copiado)
#define BOOTSECTOR 0x00001000
// direccion fisica de comienzo del kernel
#define KERNEL 0x00001200
// direccion fisica del buffer de video
#define VIDEO 0x000B8000
// direcciones fisicas del codigo de los jugadores
#define RICK_CODE_PHY 0x10000
#define MORTY_CODE_PHY 0x14000

/* Direcciones virtuales de código, pila y datos */
/* -------------------------------------------------------------------------- */

// direccion virtual del codigo
#define TASK_CODE_VIRTUAL 0x01D00000
#define TASK_PAGES        4

#define MEESEEK_VIRTUAL_AREA	0x8000000
#define MEESEEK_PAGES			2

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<X>_PHY_START.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
#define KERNEL_PAGE_DIR     (0x00025000)
#define KERNEL_PAGE_TABLE_0 (0x00026000)
#define KERNEL_STACK        (0x00025000)
#define KERNEL_FIRST_PAGE	(0x00100000)

/* TSS */
/* -------------------------------------------------------------------------- */
#define TSS_SIZE 0x68

#define EFLAGS_DEFAULT 0x202 			// Interrupt enable
//#define TSS_IOMAP_ALL_PORTS_DISABLED
#define TSS_IDLE_SEL (0x10 << 3)



/* Modo Debug */
/* -------------------------------------------------------------------------- */
// Estados
#define NORMAL		0x00
#define DEBUG 		0x0D
#define EXCEPCION 	0x0E


#endif //  __DEFINES_H__