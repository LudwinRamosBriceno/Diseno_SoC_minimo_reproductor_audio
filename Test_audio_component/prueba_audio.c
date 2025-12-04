
#include <stdint.h>
#include "sys/alt_stdio.h"
#include "altera_up_avalon_audio_regs.h"
#include "altera_up_avalon_audio.h"
#include <stdint.h>
#include "sys/alt_stdio.h"

#define NUM_SAMPLES 1000
#define AUDIO_BASE 0x3000
#define DELAY_CYCLES 1000


void delay_loop(int count){
    for(volatile int i = 0; i < count; i++);
}

int main() {
    // Ejemplo: señal de onda cuadrada simple
    int16_t samples[NUM_SAMPLES];
    for (int i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = (i % 2 == 0) ? 0x7FFF : 0x8000; // Alterna entre valores altos y bajos
    }

    volatile int* audio = (volatile int*) AUDIO_BASE;

    alt_putstr("Nios audio test iniciado...\n");

    while (1) {
        for (int i = 0; i < NUM_SAMPLES; i++) {
            // Espera a que haya espacio en ambos canales
            while (((audio[1] & 0x00FF0000) == 0) || ((audio[1] & 0xFF000000) == 0)){
            	alt_putstr("lleno...\n");
            }

            int16_t s = samples[i];
            audio[2] = s; // canal izquierdo
            audio[3] = s; // canal derecho

            // Retardo para evitar saturar el buffer
            delay_loop(DELAY_CYCLES);
            alt_putstr("vacio...\n");
        }
    }
    return 0;
}


// Tutorial para ejecutar un programa C en el NIOS
//   https://youtu.be/HpiEGok8vGU?si=YX64mpgCTHng8wtq