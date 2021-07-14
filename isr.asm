; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
;
; Definicion de rutinas de atencion de interrupciones

%include "print.mac"

BITS 32

sched_task_offset:     dd 0
sched_task_selector:   dw 0

;; PIC
extern pic_finish1

;; Sched
extern curr_pl
extern sched_next_task
extern sched_dealloc_current_task

;; funciones externas
extern imprimir_excepcion
extern printScanCode
extern gp_group
extern cr_group
extern init_meeseek
extern player_task_update_age
extern move_current_task
extern look
extern teleport_meeseek

;; modo Debug
extern STATE
extern load_excep_state
extern change_state
extern debug_exception

;; Definicion de constantes 

; selector de segmento tss idle
%define TSS_IDLE_SEL (0x10 << 3)

rick_wins_msg db     '¡Rick gana!'
rick_wins_len equ    $ - rick_wins_msg

morty_wins_msg db     '¡Morty gana!'
morty_wins_len equ    $ - morty_wins_msg

; maquina de STATEs
%define NORMAL      0x00
%define DEBUG       0x0D
%define EXCEPCION   0x0E

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

;; Guardar estado actual de TODOS los registros en la pila
%macro push_reg_state 0
    push esp        

    push ebp
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax

    xor eax, eax

    mov ax, gs
    push eax
    mov ax, fs
    push eax
    mov ax, es
    push eax
    mov ax, ds
    push eax

    mov eax, cr4
    push eax
    mov eax, cr3
    push eax
    mov eax, cr2
    push eax
    mov eax, cr0
    push eax

%endmacro

;; Rutina de atencion de las excepciones
%macro ISR 1
global _isr%1

_isr%1:
    cmp byte [STATE], DEBUG
    jne .continue
        ; modo DEBUG + excepcion --> modo EXCEPCION

        push_reg_state          ; almacenar vector con estado de los registros en la pila
        push esp                ; pushear ptro al vector
        call load_excep_state


        mov eax, %1
        push eax
        call debug_exception
        add esp, 4 

        jmp .fin

    .continue:
    ; Desalojar la tarea actual
    call sched_dealloc_current_task

    .fin:
    ; pasar a la tarea idle
    jmp TSS_IDLE_SEL:0

    iret

%endmacro


;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21

;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32

_isr32:
    pushad

    ; avisar al pic que se recibio la interrupcion
    call pic_finish1

    cmp byte [STATE], EXCEPCION
    je .no_swap             ; mientras este impresa la excepcion, el juego se mantiene detenido (idle)

        ; obtener proxima tarea
        call sched_next_task

        push ax

        ; Actualizar el tiempo de vida de una tarea meeseeks
        push dword [curr_pl]
        call player_task_update_age
        add esp, 4

        pop ax

        ; comparar tarea actual con la proxima
        str cx
        cmp cx, ax
        je .no_swap

            ; permutar tareas
            mov [sched_task_selector], ax
            ;xchg bx,bx
            jmp far [sched_task_offset]

    .no_swap:

    ; imprimir el reloj del sistema
    call next_clock

    popad
    iret

;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
global _isr33

_isr33:
    pushad

    ; mueve a AL un byte del puerto I/O
    in al, 0x60 

    cmp al, 0x15
    jne .continue
        ; se presiono la tecla 'y'
        call change_state

    .continue:

    call pic_finish1
    
    popad
    iret

;; Rutinas de atención de Syscalls
;; -------------------------------------------------------------------------- ;;
global _isr88
global _isr89
global _isr100
global _isr123

;; Syscall Meeseeks
_isr88:
    push ebp
    mov ebp, esp
    sub esp, 4
    pushad

    push ecx
    push ebx
    push eax
    call init_meeseek
    add esp, 3*4
    mov [ebp - 4], eax

    ; pasar a la tarea idle
    jmp TSS_IDLE_SEL:0


    popad
    pop eax
    pop ebp
    iret

;; Syscall Use_Portal_Gun
_isr89:
    pushad

    call teleport_meeseek
    ; pasar a la tarea idle
    jmp TSS_IDLE_SEL:0

    popad
    iret

;; Syscall Look
_isr100:
    push ebp
    mov ebp, esp
    sub esp, 2*4
    pushad

    lea eax, [ebp - 8]
    push eax
    call look
    add esp, 4

    ; pasar a la tarea idle
    jmp TSS_IDLE_SEL:0

    popad
    pop eax
    pop ebx
    pop ebp
    iret

;; Syscall Move
_isr123:
    push ebp
    mov ebp, esp
    sub esp, 4
    pushad
    
    push ebx
    push eax
    call move_current_task
    add esp, 2*4
    mov [ebp - 4], eax

    ; pasar a la tarea idle
    jmp TSS_IDLE_SEL:0

    popad
    pop eax
    pop ebp
    iret

;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:           dd 0x00000000
isrClock:            db '|/-\'
next_clock:
        pushad

        inc DWORD [isrNumber]
        mov ebx, [isrNumber]
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrNumber], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock
                print_text_pm ebx, 1, 0x0f, 49, 79
                popad
        ret