/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "defines.h"
#include "kassert.h"
#include "mmu.h"
#include "gdt.h"

#define GDT_OFF_CS_RING_0 (GDT_IDX_CS_0_DESC << 3)
#define GDT_OFF_DS_RING_0 (GDT_IDX_DS_0_DESC << 3)
#define GDT_OFF_CS_RING_3 (GDT_IDX_CS_3_DESC << 3)
#define GDT_OFF_DS_RING_3 (GDT_IDX_DS_3_DESC << 3)
#define GDT_RPL_RING_0 0x0
#define GDT_RPL_RING_3 0x3

tss_t tss_initial = {0};

tss_t tss_idle = {
	.ss = GDT_OFF_DS_RING_0 | GDT_RPL_RING_0,
	.ds = GDT_OFF_DS_RING_0 | GDT_RPL_RING_0,
	.es = GDT_OFF_DS_RING_0 | GDT_RPL_RING_0,
	.fs = GDT_OFF_DS_RING_0 | GDT_RPL_RING_0,
	.gs = GDT_OFF_DS_RING_0 | GDT_RPL_RING_0,
	.cs = GDT_OFF_CS_RING_0 | GDT_RPL_RING_0,
	.cr3 = KERNEL_PAGE_DIR,
	.eip = 0x18000,
	.ebp = KERNEL_STACK,
	.esp = KERNEL_STACK,
	.eflags = EFLAGS_DEFAULT,
	.iomap = 0x0,
    .ss1 = 0,
    .esp1 = 0,
    .ss2 = 0,
    .esp2 = 0,
    .ss0 = GDT_OFF_DS_RING_0 | GDT_RPL_RING_0,
    .esp0 = KERNEL_STACK
};

tss_t* get_tss_base(uint32_t index) {

    gdt_entry_t desc_tss = gdt[index];
    tss_t* dir_tss = (tss_t*)  ((desc_tss.base_31_24 << 24) | 
                               (desc_tss.base_23_16 << 16) | 
                                desc_tss.base_15_0);

    return dir_tss;
}

void tss_init_idle(void) {
	/* calcular direccion tss idle */
	tss_t* idle_dir_tss = get_tss_base(GDT_IDX_TSS_IDLE);

	/* cargar tss idle */
	*idle_dir_tss = tss_idle;

	/* habilitar descriptor tss idle en la gdt */
	gdt[GDT_IDX_TSS_IDLE].p = 1;
}

void tss_init_players(void) {
 	/* calcular direccion tss rick y morty */
	tss_t* tss_rick = get_tss_base(GDT_IDX_TSS_RICK);
	tss_t* tss_morty = get_tss_base(GDT_IDX_TSS_MORTY);

	/* inicializar tss */
	tss_init(tss_rick, TASK_CODE_VIRTUAL, 0x1D00000, RICK_CODE_PHY, 1);
	tss_init(tss_morty, TASK_CODE_VIRTUAL, 0x1D04000, MORTY_CODE_PHY, 1);

	/* habilitar descriptores tss rick y morty en la gdt */
	gdt[GDT_IDX_TSS_RICK].p = 1;
	gdt[GDT_IDX_TSS_MORTY].p = 1;
}

void tss_init_meeseek(paddr_t code_start, uint32_t x, uint32_t y, pl_id_t pl, uint8_t mee_id) {
	uint32_t gdt_idx_mee = get_gdt_idx_tss(pl, mee_id);
	tss_t* tss_mee = get_tss_base(gdt_idx_mee);

	paddr_t offset = (x + y*80)*0x2000;
	paddr_t phy_addr = 0x400000 + offset;

	uint32_t mee_eip = 0x8000000 + (mee_id)*0x2000;

	tss_init(tss_mee, mee_eip, phy_addr, code_start, 0);

	tss_t* tss_player = get_tss_base(get_gdt_idx_tss(pl, 10));
	tss_mee->cr3 = tss_player->cr3;

	mmu_append_task(tss_player->cr3, phy_addr, code_start, mee_eip);

	// Setear presente en GDT
	gdt[gdt_idx_mee].p = 1;
}

void tss_init(tss_t* dir, uint32_t eip, paddr_t phy_start, paddr_t code_start, uint32_t task_type) {
	// Completar tss
	dir->eip = eip;
	dir->esp = eip + 0x1000*(task_type*2 + 2);	// eip + 4kb*#paginas_mapeadas
	dir->ebp = eip + 0x1000*(task_type*2 + 2);	
	dir->ss = GDT_OFF_DS_RING_3 | GDT_RPL_RING_3;
	dir->ds = GDT_OFF_DS_RING_3 | GDT_RPL_RING_3;
	dir->es = GDT_OFF_DS_RING_3 | GDT_RPL_RING_3;
	dir->fs = GDT_OFF_DS_RING_3 | GDT_RPL_RING_3;
	dir->gs = GDT_OFF_DS_RING_3 | GDT_RPL_RING_3;
	dir->cs = GDT_OFF_CS_RING_3 | GDT_RPL_RING_3;
	dir->eflags = EFLAGS_DEFAULT;
	dir->esp0 = mmu_next_free_kernel_page() + PAGE_SIZE - 1;
	dir->ss0 = GDT_OFF_DS_RING_0 | GDT_RPL_RING_0;

	if (task_type == 1) {
		dir->cr3 = mmu_init_task_dir(KERNEL_PAGE_DIR, phy_start, code_start, TASK_PAGES);
	}
}

