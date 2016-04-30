.section .text
 
// Init ist eine Funktion aus init.c
.extern init

// _start muss global sein, damit der Linker es findet und als Einsprungspunkt
// benutzen kann (alle Labels, die nicht global sind, sind nur in dieser Datei
// sichtbar)
.global _start
_start:
    // Stack initialisieren
    mov $kernel_stack, %esp
 
    // C-Code aufrufen
    call init
 
    // Falls wir jemals aus init zurueckkommen sollten, sperren wir die Interrupts und
    // halten einfach den Prozessor an. (man braucht ihn ja nicht unnötig heißlaufen lassen.)
_stop:
    cli
    hlt
 
    // Sollte es doch weitergehen, probieren wir erneut die CPU schlafen zu lassen
    jmp _stop
 
// 8 kB Stack fuer den Kernel. Das Label steht hinter dem freien Speicher,
// weil der Stack nach unten waechst
.section .bss
.space 8192
kernel_stack: