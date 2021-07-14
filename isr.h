/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Declaracion de las rutinas de atencion de interrupciones
*/

#ifndef __ISR_H__
#define __ISR_H__

// Excepciones del procesador
void _isr0();
void _isr1();
void _isr2();
void _isr3();
void _isr4();
void _isr5();
void _isr6();
void _isr7();
void _isr8();
void _isr9();
void _isr10();
void _isr11();
void _isr12();
void _isr13();
void _isr14();
void _isr15();
void _isr16();
void _isr17();
void _isr18();
void _isr19();
void _isr20();
void _isr21();

// Interrupcion del reloj
void _isr32();

// Interrupcion del teclado
void _isr33();

// Interrupciones adicionales Software
void _isr88();
void _isr89();
void _isr100();
void _isr123();

#endif //  __ISR_H__
