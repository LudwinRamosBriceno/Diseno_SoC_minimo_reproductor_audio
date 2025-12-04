
#include <stdio.h>
#include <stdint.h>
#include "sys/alt_stdio.h"
#include "altera_up_avalon_audio_regs.h"
#include "altera_up_avalon_audio.h"
#include "system.h" 									// Para las direcciones base

// Variables de estado controladas por las ISR de los botones
extern volatile int play_flag;
extern volatile int pause_flag;
extern volatile int next_flag;
extern volatile int prev_flag;

#define AUDIO_BASE 		0x00004160
#define RAM2_BASE  		0x00040000
#define BLOCK_SAMPLES 	(1024*16)
#define FLAG_READY   	0   							// byte 0: flags buf0/buf1
#define BUF0_OFF     	4   							// offset para buf0
#define BUF1_OFF     	(BUF0_OFF + BLOCK_SAMPLES*2) 	// offset para buf1 (int16_t)
#define DELAY_CYCLES 	44

// Se declaran (pero se definen en seg_botones.c)
void start_button_isr (void * context);
void pause_button_isr (void * context);
void next_button_isr (void * context);
void prev_button_isr (void * context);
void reiniciar_timer ();


// Delay simple
void delay_loop(int count){
	for(volatile int i = 0; i < count; i++);
}

// Logica principal de control de audio
void audio_control_loop() {

    volatile int* audio = (volatile int*) AUDIO_BASE;
    volatile uint8_t* ram2_base = (volatile uint8_t*) RAM2_BASE;
    volatile uint8_t* ready = ram2_base + FLAG_READY;
    int16_t* buf0 = (int16_t*)(ram2_base + BUF0_OFF);
    int16_t* buf1 = (int16_t*)(ram2_base + BUF1_OFF);

    while (1) {
        if (play_flag) {
            // Doble buffer: reproduce buf0 o buf1 seg�n bandera ready
            if (*ready & 0x01) {
                // buf0 listo
                for (int i = 0; i < BLOCK_SAMPLES; i++) {
                    // Espera a que haya espacio en ambos canales
                    while (((audio[1] & 0x00FF0000) == 0) || ((audio[1] & 0xFF000000) == 0)) {}
                    int16_t muestra = buf0[i];
                    audio[2] = muestra; // canal izquierdo
                    audio[3] = muestra; // canal derecho
                    delay_loop(DELAY_CYCLES);
                }
                *ready &= ~0x01; // Limpia bandera buf0 para decirle al HPS que puede llenar de nuevo
            } else if (*ready & 0x02) {
                // buf1 listo
                for (int i = 0; i < BLOCK_SAMPLES; i++) {
                	// Espera a que haya espacio en ambos canales
                    while (((audio[1] & 0x00FF0000) == 0) || ((audio[1] & 0xFF000000) == 0)) {}
                    int16_t muestra = buf1[i];
                    audio[2] = muestra;
                    audio[3] = muestra;
                    delay_loop(DELAY_CYCLES);
                }
                *ready &= ~0x02; // Limpia bandera buf1 para decirle al HPS que puede llenar de nuevo
                
            } else {
                // No hay buffer listo, espera
                delay_loop(DELAY_CYCLES+100);
            }

        }/* else if (pause_flag) {
            // Pausa: no hacer nada
        	delay_loop(DELAY_CYCLES+100);
            continue;

        } else if (next_flag) {
            // Pausa y detiene el timer
            pause_button_isr(NULL);
            // Se le indica al HPS que cambie de canci�n
            next_button_isr(NULL);
            // Se limpian los flags
            play_flag = 0;
            pause_flag = 0;
            next_flag = 0;
            prev_flag = 0;

        } else if (prev_flag) {
            // Pausa y detiene el timer
            pause_button_isr(NULL);
            // Se le indica al HPS que cambie de canci�n anterior
            prev_button_isr(NULL);
            // Se limpian los flags
            play_flag = 0;
            pause_flag = 0;
            next_flag = 0;
            prev_flag = 0;
        }*/
    }
}
