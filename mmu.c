/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"
#include "defines.h"
#include "kassert.h"

#define MMU_FLAG_PRESENT 1
#define MMU_FLAG_SUPERVISOR 0
#define MMU_FLAG_READWRITE 1

// EJERCICIO 4
	// Armamos Identity Mapping en el Kernel
paddr_t mmu_init_kernel_dir(void) {
	page_directory_entry* pd = (page_directory_entry *)KERNEL_PAGE_DIR;
	page_table_entry* pt_0 = (page_table_entry *)KERNEL_PAGE_TABLE_0;

	// Inicializamos en 0
	for (int i = 0; i < 1024; ++i) {
		pd[i] = (page_directory_entry){0};
		pt_0[i] = (page_table_entry){0};
	}

	// Definimos las entradas de la primer entrada del directorio
	pd[0].present = MMU_FLAG_PRESENT;
	pd[0].user_supervisor = MMU_FLAG_SUPERVISOR;
	pd[0].read_write = MMU_FLAG_READWRITE;
	pd[0].page_table_base = ((uint32_t)pt_0)>>12;

	// Definimos las entradas de la 1er tabla
	for (int i = 0; i < 1024; ++i) {
		pt_0[i].present = MMU_FLAG_PRESENT;
		pt_0[i].user_supervisor = MMU_FLAG_SUPERVISOR;
		pt_0[i].read_write = MMU_FLAG_READWRITE;
		pt_0[i].physical_address_base = i;
	}

	return (uint32_t) pd;
}

// EJERCICIO 5

page_table_entry* proxima_pagina_libre;

void mmu_init(void) {
	proxima_pagina_libre = (page_table_entry *)KERNEL_FIRST_PAGE;
}

paddr_t mmu_next_free_kernel_page(void) {
    page_table_entry* res = proxima_pagina_libre;
    proxima_pagina_libre += PAGE_SIZE;
    return (paddr_t) res;
}


void mmu_map_page(uint32_t cr3, vaddr_t virt, paddr_t phy, uint32_t attrs) {
	vaddr_t directoryIdx = virt >> 22;
	vaddr_t tableIdx = (virt >> 12) & 0x3FF;

	// conseguimos la entrada en la PDT
	page_directory_entry* pde = (page_directory_entry *)(cr3 & ~0xFFF);

	// si la pagina no está presente, creamos una
	if (pde[directoryIdx].present != 1) {
		paddr_t addr = mmu_next_free_kernel_page();
		page_table_entry* nuevaPT = (page_table_entry *) addr; 

		for (int i = 0; i < 1024; ++i) {
			nuevaPT[i] = (page_table_entry){0};
		}
		
		// asignamos la nueva pagina a su entrada en la PDT
		pde[directoryIdx].page_table_base = ((vaddr_t)addr)>>12;
		pde[directoryIdx].present = attrs & 0x0001;
		pde[directoryIdx].read_write = (attrs >> 1) & 0x0001;
		pde[directoryIdx].user_supervisor = (attrs >> 2) & 0x0001;
		pde[directoryIdx].page_write_through= (attrs >> 3) & 0x0001;
		pde[directoryIdx].page_cache_disable = (attrs >> 4) & 0x0001;
		pde[directoryIdx].accessed = (attrs >> 5) & 0x0001;
		pde[directoryIdx].x = (attrs >> 6) & 0x0001;
		pde[directoryIdx].page_size = (attrs >> 7) & 0x0001;
		pde[directoryIdx].ignore = (attrs >> 8) & 0x0001;
		pde[directoryIdx].available = (attrs >> 9) & 0x0007;
		//pde[directoryIdx] = addr | attrs;
	}

	// completamos la entrada correspondiente en la PT
	page_table_entry* PT = (page_table_entry *)(pde[directoryIdx].page_table_base << 12);
	PT[tableIdx].physical_address_base = (phy >> 12);
	PT[tableIdx].present = attrs & 0x0001;
	PT[tableIdx].read_write = (attrs >> 1) & 0x0001;
	PT[tableIdx].user_supervisor = (attrs >> 2) & 0x0001;
	PT[tableIdx].page_write_through = (attrs >> 3) & 0x0001;
	PT[tableIdx].page_cache_disable = (attrs >> 4) & 0x0001;
	PT[tableIdx].accessed = (attrs >> 5) & 0x0001;
	PT[tableIdx].dirty = (attrs >> 6) & 0x0001;
	PT[tableIdx].x = (attrs >> 7) & 0x0001;
	PT[tableIdx].global = (attrs >> 8) & 0x0001;
	PT[tableIdx].available = (attrs >> 9) & 0x0007;
	//PT[tableIdx] = (phy & ~0xFFF) | attrs;
	tlbflush();
}


void mmu_unmap_page(uint32_t cr3, vaddr_t virt) {
	vaddr_t directoryIdx = virt >> 22;
	vaddr_t tableIdx = (virt >> 12) & 0x3FF;

	// conseguimos la base de la PDT
	page_directory_entry* pde = (page_directory_entry *)(cr3 & ~0xFFF);

	// conseguimos la base de la PT
	page_table_entry* pagina_a_borrar = (page_table_entry *)(pde[directoryIdx].page_table_base << 12);

	// borramos la entrada de la pagina correspondiente
	pagina_a_borrar[tableIdx].present = 0;

	tlbflush();
}


paddr_t mmu_init_task_dir(uint32_t cr3, paddr_t phy_start, paddr_t code_start, size_t pages) {
	//breakpoint();

	size_t nada = pages;
	pages = nada;

	vaddr_t task_vir_start = TASK_CODE_VIRTUAL;
	uint32_t task_attrs = 0x7; // u/s=1, r/w=0, p=1

	// Inicializamos el directorio de paginas
	page_directory_entry* pd = (page_directory_entry *)mmu_next_free_kernel_page();


	// Inicializamos en 0
	for (int i = 0; i < 1024; ++i) {
		pd[i] = (page_directory_entry){0};
	}
	
	// Páginas del kernel
	pd[0] = *(page_directory_entry*) KERNEL_PAGE_DIR;

	// Mapear paginas para la tarea a partir de la dirección 0x1D00000
	for (int i = 0; i < (int)pages; ++i) {
		mmu_map_page((uint32_t)pd, task_vir_start + i*0x1000, phy_start + i*0x1000, task_attrs);

		// Tambien mapear para el kernel: necesario para copiar el codigo
		mmu_map_page(cr3, task_vir_start + i*0x1000, phy_start + i*0x1000, 0x3);
	}

	// Copiar el codigo de la tarea a  partir de la direccion fisica correspondiente
	vaddr_t src_dir = (vaddr_t) code_start;
	vaddr_t dst_dir = task_vir_start;

	for (int i = 0; i < (int)pages*1024; ++i) {

		*(uint32_t*) dst_dir = *(uint32_t*) src_dir;

		src_dir += 4;
		dst_dir += 4;
	}

	// Desmapear paginas del kernel
	for (int i = 0; i < (int)pages; ++i) {
		mmu_unmap_page(cr3, task_vir_start + i*0x1000);
	}

	return (paddr_t)pd; 	// pd[11:0] = 0 -> pd.pcd = pd.pwt = 0
}

void mmu_append_task(uint32_t cr3, paddr_t phy_start, paddr_t code_start, vaddr_t vir_start) {

	uint32_t task_attrs = 0x7;

	// Mapear paginas para la tarea a partir de la dirección vir_start
	for (int i = 0; i < 2; ++i) {
		mmu_map_page(cr3, vir_start + i*0x1000, phy_start + i*0x1000, task_attrs);
	}

	// Copiar el codigo de la tarea a  partir de la direccion fisica correspondiente
	vaddr_t src_dir = (vaddr_t) code_start;
	vaddr_t dst_dir = vir_start;
	vaddr_t src_lim = 0x1D04000;
	for (int i = 0; i < 2*1024 && src_dir < src_lim; ++i) {

		*(uint32_t*) dst_dir = *(uint32_t*) src_dir;

		src_dir += 4;
		dst_dir += 4;
	}
}

void mmu_move_task(uint32_t cr3, paddr_t src_start, paddr_t dst_start, vaddr_t vir_start, uint32_t pages) {
	
	uint32_t task_attrs = 0x7;
	uint32_t kernel_attrs = 0x3;

	for (uint32_t i = 0; i < pages; ++i) {
		// Mapear las paginas dst a la direccion virtual de la tarea
		mmu_map_page(cr3, vir_start + i*0x1000, dst_start + i*0x1000, task_attrs);

		// Mapear las paginas src con identity mapping
		mmu_map_page(rcr3(), (vaddr_t)(dst_start + i*0x1000), dst_start + i*0x1000, task_attrs);
		mmu_map_page(rcr3(), (vaddr_t)(src_start + i*0x1000), src_start + i*0x1000, kernel_attrs);
	}


	// Copiar el codigo de la tarea a  partir de la direccion fisica correspondiente
	vaddr_t src_dir = (vaddr_t) src_start;
	vaddr_t dst_dir = (vaddr_t) dst_start;
	for (uint32_t i = 0; i < pages*1024; ++i) {

		*(uint32_t*) dst_dir = *(uint32_t*) src_dir;

		src_dir += 4;
		dst_dir += 4;
	}

	// Desmapear paginas src
	for (uint32_t i = 0; i < pages; ++i) {
		mmu_unmap_page(rcr3(), (vaddr_t)(src_start + i*0x1000));
		mmu_unmap_page(rcr3(), (vaddr_t)(dst_start + i*0x1000));
	}
}



vaddr_t task_vir_start(uint8_t task_id) {
	vaddr_t res;
	if (task_id == 10) 
		res = 0x1D00000;
	else
		res = MEESEEK_VIRTUAL_AREA + (task_id)*0x1000*MEESEEK_PAGES;
	return res;
}
