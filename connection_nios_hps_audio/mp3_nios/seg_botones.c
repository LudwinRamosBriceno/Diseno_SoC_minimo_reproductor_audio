
#include "sys/alt_stdio.h"
#include <system.h>
#include <sys/alt_irq.h>

#define RAM_BASE				0x00020000
#define RAM_OFFSET_FPGA_TO_HPS 	0x3

// Puntero para escritura de banderas de control para el HPS
volatile unsigned int * ram_ptr = (unsigned int *) RAM_BASE;
volatile unsigned int * ram_offset_fpga_to_hps_ptr = (unsigned int *)(RAM_BASE + RAM_OFFSET_FPGA_TO_HPS);

// Punteros para control del timer
volatile unsigned int * timer_status_ptr = (unsigned int *) TIMER_BASE;
volatile unsigned int * timer_ctrl_ptr = (unsigned int *) TIMER_BASE + 1;

// Punteros para el manejo de las interrupciones de start, pause, next y prev
volatile unsigned int * start_button_pio_intr_mask = (unsigned int *) BUTTON_START_BASE + 2;
volatile unsigned int * start_button_pio_edge_capture = (unsigned int *) BUTTON_START_BASE + 3;
volatile unsigned int * pause_button_pio_intr_mask = (unsigned int *) BUTTON_PAUSE_BASE + 2;
volatile unsigned int * pause_button_pio_edge_capture = (unsigned int *) BUTTON_PAUSE_BASE + 3;
volatile unsigned int * next_button_pio_intr_mask = (unsigned int *) BUTTON_NEXT_BASE + 2;
volatile unsigned int * next_button_pio_edge_capture = (unsigned int *) BUTTON_NEXT_BASE + 3;
volatile unsigned int * prev_button_pio_intr_mask = (unsigned int *) BUTTON_PREV_BASE + 2;
volatile unsigned int * prev_button_pio_edge_capture = (unsigned int *) BUTTON_PREV_BASE + 3;

// Puntero para el display 7 segmentos
volatile unsigned int * display_ptr = (unsigned int *) REG_SEGMENTS_BASE;

// Variables globales
unsigned int seconds_counter;

// Mascara de bits para el display de 7 segmentos
unsigned char seg7[10] = {
    0x40, // 0 -> 0b1000000
    0x79, // 1 -> 0b1111001
    0x24, // 2 -> 0b0100100
    0x30, // 3 -> 0b0110000
    0x19, // 4 -> 0b0011001
    0x12, // 5 -> 0b0010010
    0x02, // 6 -> 0b0000010
    0x78, // 7 -> 0b1111000
    0x00, // 8 -> 0b0000000
    0x10  // 9 -> 0b0010000
};

// Rutinas de servicio de interrupcion
void timer_isr (void * context, unsigned long id);
void start_button_isr (void * context, unsigned long id);
void pause_button_isr (void * context, unsigned long id);
void next_button_isr (void * context, unsigned long id);
void prev_button_isr (void * context, unsigned long id);
void reiniciar_timer ();

// Inicializacion de la interrupcion del timer
int init_timer_intr (void);

// Variables globales
extern volatile int play_flag;
extern volatile int pause_flag;
extern volatile int next_flag;
extern volatile int prev_flag;

// Funcion de reiniciar el timer
int init_timer_intr (void){
	// Registrar la interrupcion para el timeout
	alt_irq_register(TIMER_IRQ, 0x0, timer_isr);
	// Escribir los valores correspondientes al registro de control del timer
	*timer_ctrl_ptr = 0x7; // 0x7 = 0(STOP) 1(START) 1(CONT) 1(ITO)
	return 0;
}


// Funciones de inicializacion y registro de interrupciones
int init_pause_button_intr (void){
	// Limpiar el registro escribiendo 1
	*pause_button_pio_edge_capture = 0x1;
	// Habilitar las interrupciones en el puerto
	*pause_button_pio_intr_mask = 0x1;
	// Registrar la interrupción para el botón de pausa
	alt_irq_register(BUTTON_PAUSE_IRQ, 0x0, pause_button_isr);
	return 0;
}

int init_start_button_intr (void){
	// Limpiar el registro escribiendo 1
	*start_button_pio_edge_capture = 0x1;
	// Habilitar las interrupciones en el puerto
	*start_button_pio_intr_mask = 0x1;
	// Registrar la interrupción para el botón de inicio
	alt_irq_register(BUTTON_START_IRQ, 0x0, start_button_isr);
	return 0;
}

int init_next_button_intr (void){
	// Limpiar el registro escribiendo 1
	*next_button_pio_edge_capture = 0x1;
	// Habilitar las interrupciones en el puerto
	*next_button_pio_intr_mask = 0x1;
	// Registrar la interrupción para el botón siguiente
	alt_irq_register(BUTTON_NEXT_IRQ, 0x0, next_button_isr);
	return 0;
}

int init_prev_button_intr (void){
	// Limpiar el registro escribiendo 1
	*prev_button_pio_edge_capture = 0x1;
	// Habilitar las interrupciones en el puerto
	*prev_button_pio_intr_mask = 0x1;
	// Registrar la interrupción para el botón previo
	alt_irq_register(BUTTON_PREV_IRQ, 0x0, prev_button_isr);
	return 0;
}


// Funciones objetivo de las interrupciones
void timer_isr (void * context, unsigned long id){
	// Limpia el registro de estado para que continue contando
	*timer_status_ptr = 0;
	// Incrementa el contador de segundos
	seconds_counter++;
	// Conversion a formato MM.SS
	int minutes = (seconds_counter / 60) % 100;
	int seconds = seconds_counter % 60;
	// Asignar cada digito a diferentes valores (numeros que van a estar en cada display)
	int d1 = minutes / 10;
	int d2 = minutes % 10;
	int d3 = seconds / 10;
	int d4 = seconds % 10;

	// Empaquetar los 28 bits
	unsigned int display_value =
		(seg7[d1] << 21) |
		(seg7[d2] << 14) |
		(seg7[d3] << 7 ) |
		(seg7[d4]      );

	// Escribir en el PIO
	*display_ptr = display_value;
}

void start_button_isr (void * context, unsigned long id){
	// Lee el registro edge-capture para verificar si la interrupcion ha sido activada
	unsigned int edge = *start_button_pio_edge_capture;
	// Limpia el registro escribiendo 0x1 (valor leido)
	*start_button_pio_edge_capture = edge;
	// Escribe 1 en la direccion vista por el HPS para el control para activar
	*ram_offset_fpga_to_hps_ptr = 1;
	// Retardo
	for(volatile int i = 0; i < 1000; i++);
	// Escribe 0 en la direccion vista por el HPS para el control para limpiar
	*ram_offset_fpga_to_hps_ptr = 0;

	/*// Condicion para iniciar la interrupcion del timer
	if (edge & 0x1){
		// Verificar que el timer no esté corriendo y que no haya timeout aún
		if (*timer_status_ptr == 0){
			// Inicializa el timer
			init_timer_intr();
		} else{
			// No hace nada
		}
	}
	*/

	// Cambio de las flags de control de audio (variables globales)
	play_flag = 1;
	pause_flag = 0;
	next_flag = 0;
	prev_flag = 0;

}

void pause_button_isr (void * context, unsigned long id){
	// Lee el registro edge-capture para verificar si la interrupcion ha sido activada
	unsigned int edge = *pause_button_pio_edge_capture;
	// Limpia el registro escribiendo 0x1 (valor leido)
	*pause_button_pio_edge_capture = edge;
	// Escribe 1 en la direccion vista por el HPS para el control para activar
	*ram_offset_fpga_to_hps_ptr = 2;
	// Retardo
	for(volatile int i = 0; i < 1000; i++);
	// Escribe 1 en la direccion vista por el HPS para el control para limpiar
	*ram_offset_fpga_to_hps_ptr = 0;

	// Condicion para iniciar la interrupcion del timer
	/*
	if (edge & 0x1){
		// Verificar que el timer esta corriendo
		if (*timer_status_ptr == 0x2 || *timer_status_ptr == 0x3){
			// Detener el timer al escribir los valores correspondientes al registro de control del timer
			*timer_ctrl_ptr = 0x8; // 0x7 = 1(STOP) 0(START) 0(CONT) 0(ITO)
			// Escribir 1 en el bit "TO" para limpiar el registro
			*timer_status_ptr = 0x1;
		}
		else{
			// No hace nada
		}
	}
	*/

	// Cambio de las flags de control de audio (variables globales)
	play_flag = 0;
	pause_flag = 1;
	next_flag = 0;
	prev_flag = 0;
}

void next_button_isr (void * context, unsigned long id){
	// Lee el registro edge-capture para verificar si la interrupcion ha sido activada
	unsigned int edge = *next_button_pio_edge_capture;
	// Limpia el registro escribiendo 0x1 (valor leido)
	*next_button_pio_edge_capture = edge;
	// Escribe 1 en la direccion vista por el HPS para el control para activar
	*ram_offset_fpga_to_hps_ptr = 4;
	// Retardo
	for(volatile int i = 0; i < 1000; i++);
	// Escribe 1 en la direccion vista por el HPS para el control para limpiar
	*ram_offset_fpga_to_hps_ptr = 0;

	// Cambio de las flags de control de audio (variables globales)
	play_flag = 0;
	pause_flag = 0;
	next_flag = 1;
	prev_flag = 0;
}

void prev_button_isr (void * context, unsigned long id){
	// Lee el registro edge-capture para verificar si la interrupcion ha sido activada
	unsigned int edge = *prev_button_pio_edge_capture;
	// Limpia el registro escribiendo 0x1 (valor leido)
	*prev_button_pio_edge_capture = edge;
	// Escribe 1 en la direccion vista por el HPS para el control para activar
	*ram_offset_fpga_to_hps_ptr = 3;
	// Retardo
	for(volatile int i = 0; i < 1000; i++);
	// Escribe 1 en la direccion vista por el HPS para el control para limpiar
	*ram_offset_fpga_to_hps_ptr = 0;

	// Cambio de las flags de control de audio (variables globales)
	play_flag = 0;
	pause_flag = 0;
	next_flag = 0;
	prev_flag = 1;
}

void reiniciar_timer (){
	// Escribe los valores correspondientes al registro de control del timer
	*timer_ctrl_ptr = 0x8; // 0x7 = 1(STOP) 0(START) 0(CONT) 0(ITO)
	// Reinicia el contador de segundos
	seconds_counter = 0;
	// Reinicia el display a 00:00
	unsigned int display_value =
		(seg7[0] << 21) |
		(seg7[0] << 14) |
		(seg7[0] << 7 ) |
		(seg7[0]      );
	// Escribe al 7 segmentos
	*display_ptr = display_value;
}

