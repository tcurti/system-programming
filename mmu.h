/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "types.h"

typedef struct str_page_directory_entry {
	uint8_t    present:1;
	uint8_t    read_write:1;
	uint8_t    user_supervisor:1;
	uint8_t    page_write_through:1;
	uint8_t    page_cache_disable:1;
	uint8_t    accessed:1;
	uint8_t    x:1;
	uint8_t    page_size:1;
	uint8_t    ignore:1;
	uint8_t    available:3;
	uint32_t   page_table_base:20;
} __attribute__((__packed__)) page_directory_entry;

typedef struct str_page_table_entry {
	uint8_t    present:1;
	uint8_t    read_write:1;
	uint8_t    user_supervisor:1;
	uint8_t    page_write_through:1;
	uint8_t    page_cache_disable:1;
	uint8_t    accessed:1;
	uint8_t    dirty:1;
	uint8_t    x:1;
	uint8_t    global:1;
	uint8_t    available:3;
	uint32_t   physical_address_base:20;
} __attribute__((__packed__)) page_table_entry;

void mmu_init(void);

paddr_t mmu_next_free_kernel_page(void);

void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs);

void mmu_unmap_page(uint32_t cr3, vaddr_t virt);

paddr_t mmu_init_kernel_dir(void);

paddr_t mmu_init_task_dir(uint32_t cr3, paddr_t phy_start, paddr_t code_start, size_t pages);

void mmu_append_task(uint32_t cr3, paddr_t phy_start, paddr_t code_start, vaddr_t vir_start);

void mmu_move_task(uint32_t cr3, paddr_t src_start, paddr_t dst_start, vaddr_t vir_start, uint32_t pages);


vaddr_t task_vir_start(uint8_t task_id);


#endif //  __MMU_H__
