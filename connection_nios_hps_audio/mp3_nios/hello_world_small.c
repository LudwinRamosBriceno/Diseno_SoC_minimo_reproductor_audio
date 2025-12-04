#include <stdio.h>
#include "sys/alt_stdio.h"

// Variables globales para flags de control de audio (declaradas extern)
volatile int play_flag;
volatile int pause_flag;
volatile int next_flag;
volatile int prev_flag;

// Incializacion de funciones
int init_start_button_intr(void);
int init_pause_button_intr(void);
int init_next_button_intr(void);
int init_prev_button_intr(void);

int main(){
	printf("Nios iniciado, esperando bloques...\n");

    // Se inicializan las interrupciones de los botones
    init_start_button_intr();
    init_pause_button_intr();
    init_next_button_intr();
    init_prev_button_intr();

    // Llama a la logica de control de audio
    audio_control_loop();

    return 0;
}
