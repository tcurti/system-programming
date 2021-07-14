/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de la tabla de descriptores globales
*/

#include "gdt.h"

gdt_entry_t gdt[GDT_COUNT] = {
    /* Descriptor nulo*/
    /* Offset = 0x00 */
    [GDT_IDX_NULL_DESC] = {
        .limit_15_0 = 0x0000,
        .base_15_0 = 0x0000,
        .base_23_16 = 0x00,
        .type = 0x0,
        .s = 0x00,
        .dpl = 0x00,
        .p = 0x00,
        .limit_19_16 = 0x00,
        .avl = 0x0,
        .l = 0x0,
        .db = 0x0,
        .g = 0x00,
        .base_31_24 = 0x00,
    },
    [GDT_IDX_CS_0_DESC] = {
        .base_15_0 = 0x0000,
        .base_23_16 = 0x00,
        .base_31_24 = 0x00,
        .limit_15_0 = 0xC8FF,   // 201MB
        .limit_19_16 = 0x00,  
        .type = 10,             // 10 = execute/read
        .s = 1,                 // 1 = codigo/datos
        .dpl = 0,               // privilegio = 0
        .p = 1,                 // presente en memoria fisica
        .avl = 0,
        .l = 0,
        .db = 1,                // modo protegido (32bits)
        .g = 1,                 // El segmento va a medir 201MB
    },
    [GDT_IDX_CS_3_DESC] = {
        .base_15_0 = 0x0000,
        .base_23_16 = 0x00,
        .base_31_24 = 0x00,
        .limit_15_0 = 0xC8FF,   // 201MB
        .limit_19_16 = 0x00,  
        .type = 10,             // 10 = execute/read
        .s = 1,                 // 1 = codigo/datos
        .dpl = 3,               // privilegio = 3
        .p = 1,                 // presente en memoria
        .avl = 0,
        .l = 0,
        .db = 1,                // modo protegido (32bits)
        .g = 1,                 // El segmento va a medir 201MB
    },
    [GDT_IDX_DS_0_DESC] = {
        .base_15_0 = 0x0000,
        .base_23_16 = 0x00,
        .base_31_24 = 0x00,
        .limit_15_0 = 0xC8FF,   // 201MB
        .limit_19_16 = 0x00,  
        .type = 2,              // 2 = read/write
        .s = 1,                 // 1 = codigo/datos
        .dpl = 0,               // privilegio = 0
        .p = 1,                 // presente en memoria
        .avl = 0,
        .l = 0,
        .db = 1,                // modo protegido (32bits)
        .g = 1,                 // El segmento va a medir 201MB   
    },
    [GDT_IDX_DS_3_DESC] = {
        .base_15_0 = 0x0000,
        .base_23_16 = 0x00,
        .base_31_24 = 0x00,
        .limit_15_0 = 0xC8FF,   // 201MB
        .limit_19_16 = 0x00,  
        .type = 2,              // 2 = read/write
        .s = 1,                 // 1 = codigo/datos
        .dpl = 3,               // privilegio = 3
        .p = 1,                 // presente en memoria
        .avl = 0,
        .l = 0,
        .db = 1,                // modo protegido (32bits)
        .g = 1,                 // El segmento va a medir 201MB
    },
    [GDT_IDX_PANT_0_DESC] = {
        .base_15_0 = 0x8000,
        .base_23_16 = 0x0B,
        .base_31_24 = 0x00,
        .limit_15_0 = 0x1F3F,   // 80 x 50 x 2 B - 1
        .limit_19_16 = 0x00,  
        .type = 2,              // 2 = read/write
        .s = 1,                 // 1 = codigo/datos
        .dpl = 0,               // privilegio = 0
        .p = 1,                 // presente en memoria
        .avl = 0,
        .l = 0,
        .db = 1,                // modo protegido (32bits)
        .g = 0,                 // El segmento ocupa menos de 1MB
    },
    [GDT_IDX_TSS_INICIAL] = {
        .base_15_0 = 0,
        .base_23_16 = 0,
        .base_31_24 = 0,
        .limit_15_0 = TSS_SIZE - 1,   
        .limit_19_16 = 0x00,  
        .type = 9,              // TSS Available
        .s = 0,                 // 0 = System Descriptor
        .dpl = 0,               // privilegio = 0
        .p = 1,                 // presente en memoria
        .avl = 0,
        .l = 0,
        .db = 0,                // porque es TSS descriptor
        .g = 0,                 // Ocupa menos de 1 MB
    }
};

gdt_descriptor_t GDT_DESC = {sizeof(gdt) - 1, (uint32_t)&gdt};