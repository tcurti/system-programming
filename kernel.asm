; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "print.mac"


global start

extern GDT_DESC

extern IDT_DESC
extern idt_init
extern pic_reset
extern pic_enable

extern mmu_init
extern mmu_init_kernel_dir
extern mmu_map_page
extern mmu_unmap_page

extern print

extern tss_init_idle
extern tss_init_players

extern sched_init

extern game_init

extern print_scores

; segmento de codigo de nivel 0
%define CS_RING_0 (0xA << 3)
; segmento de datos de nivel 0
%define DS_RING_0 (0xC << 3)
; segmento de pantalla
%define FS_PANT_0 (0xE << 3)
; segmento tss init
%define TSS_INIT_SEL (0xF << 3)
; selector de segmento tss idle
%define TSS_IDLE_SEL (0x10 << 3)

; base de la pila
%define EBP_INIT 0x25000
; tope de la pila
%define ESP_INIT 0x25000

BITS 16
;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db     'Iniciando kernel en Modo Real'
start_rm_len equ    $ - start_rm_msg

start_pm_msg db     'Iniciando kernel en Modo Protegido'
start_pm_len equ    $ - start_pm_msg

libretas_msg db 	'160/19 - 327/19 - 374/19'
libretas_len equ 	$ - libretas_msg

;;
;; Seccion de codigo.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 0003h
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0

    ; Habilitar A20
    call A20_disable
    call A20_check
    call A20_enable
    call A20_check

    ; Cargar la GDT
    lgdt [GDT_DESC]

    ; Setear el bit PE del registro CR0
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Saltar a modo protegido
    jmp CS_RING_0:modo_protegido

BITS 32
modo_protegido:

    ; Establecer selectores de segmentos
    mov ax, DS_RING_0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Establecer la base de la pila
    mov ebp, EBP_INIT
    mov esp, ESP_INIT

    ; Imprimir mensaje de bienvenida
    print_text_pm start_pm_msg, start_pm_len, 0x07, 0, 0

    ; Inicializar pantalla
    mov ax, FS_PANT_0
    mov fs, ax
    call init_screen
    mov ax, DS_RING_0
    mov fs, ax

    ; Inicializar el manejador de memoria
 	call mmu_init

    ; Inicializar el directorio de paginas
    call mmu_init_kernel_dir

    ; Cargar directorio de paginas
    mov cr3, eax

    ; Habilitar paginacion
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; Imprimir Libretas
    ;print_text_pm libretas_msg, libretas_len, 0x07, 0, 0

    ; probamos el mapeado
    ;push 0x00000003	; Escritura/Presente
    ;push 0x00400000	; Dir fisica
    ;push 0x0050E000	; Dir virtual
    ;mov eax, cr3
    ;push eax		; cr3
    ;call mmu_map_page
    ;add esp, 4*4
    ;mov byte [0x0050E027], 0x1

    ; probamos desmapear
    ;push 0x0050E000
    ;mov eax, cr3
    ;push eax
    ;call mmu_unmap_page
    ;add esp, 4*2

    ; Inicializar la IDT
    call idt_init
    
    ; Cargar IDT
    lidt [IDT_DESC]

    ; Configurar controlador de interrupciones
    call pic_reset
    call pic_enable

    ; Habilitamos interrupciones
    ;sti

    ; probamos una excepcion
    ;xchg bx, bx
    ;xor eax, eax
    ;idiv eax ; division por 0

    ; Inicializar estructuras del scheduler
    call sched_init    

    ; Cargar tarea inicial
    mov ax, TSS_INIT_SEL
    ltr ax

    ; Cargar tarea idle
    call tss_init_idle

    ; Cargar tareas jugadores
    call tss_init_players

    ; Inicializar estructuras del juego
    call game_init

    ; Saltar a la primera tarea: Idle
    jmp TSS_IDLE_SEL:0

    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $

;;
;; Funciones auxiliares.
;; -------------------------------------------------------------------------- ;;
init_screen:
	push ebp
	mov ebp, esp
    ; imprimir mapa
    xor eax, eax
    .ciclo_mapa:
    	mov dword [fs:eax], 0xA020A020

    	add eax, 4			; recorremos de a 4 Bytes
    	cmp eax, 0x1900 	; 80 columnas x 40 filas x 2 (cada caracter ocupa 2 bytes)
    	jne .ciclo_mapa

    pop ebp
    ret


;; -------------------------------------------------------------------------- ;;

%include "a20.asm"