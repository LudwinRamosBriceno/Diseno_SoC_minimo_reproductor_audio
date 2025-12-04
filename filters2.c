#include "sys/alt_stdio.h"
#include <system.h>
#include <sys/alt_irq.h>
#include <stdint.h>

#define FIFO_DEPTH 4096

//---------------------------------
// DEFINICION DE PUNTEROS GLOBALES
//---------------------------------

// definir el puntero al canal de OUT del FIFO
volatile unsigned int * fifo_base_ptr = (unsigned int *) FIFO_OUT_BASE;
// definir el puntero al control status register (CSR)
volatile unsigned int * fifo_csr_ptr = (unsigned int *) FIFO_OUT_CSR_BASE;

// definir punteros a los switch BASE
volatile unsigned int * sw_lwp_filter_ptr = (unsigned int *) SW_LWP_FILTER_BASE;
volatile unsigned int * sw_hgp_filter_ptr = (unsigned int *) SW_HP_FILTER_BASE;
volatile unsigned int * sw_bdp_filter_ptr = (unsigned int *) SW_BP_FILTER_BASE;

// definir punteros al interrupt mask del switch
volatile unsigned int * sw_lwp_filter_intr_mask_ptr = (unsigned int *) SW_LWP_FILTER_BASE + 2;
volatile unsigned int * sw_hgp_filter_intr_mask_ptr = (unsigned int *) SW_HP_FILTER_BASE + 2;
volatile unsigned int * sw_bdp_filter_intr_mask_ptr = (unsigned int *) SW_BP_FILTER_BASE + 2;


//------------------------------------------------
// DECLARACION DE FUNCIONES Y VARIABLES GLOBALES
//------------------------------------------------

// declarar la funcion para verificar si el contenido del FIFO esta disponible
static inline int fifo_data_available();

// declarar funciones que inician y registran las interrupciones
void init_lowp_filter_intr(void);
void init_hgp_filter_intr(void);
void init_bdp_filter_intr(void);

// declarar la funcion que se va a ejecutar cuando ocurran las interrupciones del switch de cada filtro
void low_pass_filter_isr(void * context);
void high_pass_filter_isr(void * context);
void band_pass_filter_isr(void * context);

void fill_fifo(void);
void lwp_filter(void);
void hgp_filter(void);
void bdp_filter(void);

// buffer donde se guardan los datos leidos del FIFO
int16_t fifo_data_buff[FIFO_DEPTH];
// buffer donde se guardan los datos de salida de cada filtro
int16_t filter_out_buff[FIFO_DEPTH];

// bandera para indicar que se ha solicitado el filtro
volatile int lwp_filter_requested = 0;
volatile int hgp_filter_requested = 0;
volatile int bdp_filter_requested = 0;


int main()
{
	alt_putstr("Initializing program! ...\n");
    fill_fifo();
    alt_putstr("FIFO has been successfully filled!\n");
    alt_putstr("Waiting for filter switch on...\n");
	// inicializar (registrar) la interrupcion del switch del low pass filter, high pass filter y band pass filter
	init_lowp_filter_intr();
    init_hgp_filter_intr();
    init_bdp_filter_intr();

    //-------------------------------
    // BUCLE PRINCIPAL DEL PROGRAMA
    //-------------------------------

	while (1)
    {
        if (lwp_filter_requested == 1)
        {
            alt_putstr("Low pass filter interruption has been detected!\n");
            // verificar si el fifo esta vacio
            if (fifo_data_available() == 0)
            {
                alt_putstr("FIFO is empty, no data to filter!\n");
            }
            else
            {
                // leer todo el contenido del FIFO
                for (int i = 0; i < FIFO_DEPTH; ++i) {
                    fifo_data_buff[i] = *fifo_base_ptr; // lee cada muestra del FIFO y guarda en el buffer
                }
                // aplicar el filtro pasa bajas
                lwp_filter();
                // resetear la bandera de solicitud del filtro pasa bajas
                lwp_filter_requested = 0;
            }
        }
        if (hgp_filter_requested == 1)
        {
            alt_putstr("High pass filter interruption has been detected!\n");
            // verificar si el fifo esta vacio
            if (fifo_data_available() == 0)
            {
                alt_putstr("FIFO is empty, no data to filter!\n");
            }
            else
            {
                // leer todo el contenido del FIFO
                for (int i = 0; i < FIFO_DEPTH; ++i) {
                    fifo_data_buff[i] = *fifo_base_ptr; // lee cada muestra del FIFO y guarda en el buffer
                }
                // aplicar el filtro pasa altas
                hgp_filter();
                // resetear la bandera de solicitud del filtro pasa altas
                hgp_filter_requested = 0;
            }
        }
        if (bdp_filter_requested == 1)
        {
            alt_putstr("Band pass filter interruption has been detected!\n");
            // verificar si el fifo esta vacio
            if (fifo_data_available() == 0)
            {
                alt_putstr("FIFO is empty, no data to filter!\n");
            }
            else
            {
                // leer todo el contenido del FIFO
                for (int i = 0; i < FIFO_DEPTH; ++i) {
                    fifo_data_buff[i] = *fifo_base_ptr; // lee cada muestra del FIFO y guarda en el buffer
                }
                // aplicar el filtro pasa banda
                bdp_filter();
                // resetear la bandera de solicitud del filtro pasa banda
                bdp_filter_requested = 0;
            }
        }
    }

	return 0;
}


// funcion para escribir datos en el FIFO
void fill_fifo(void) {
    for (int i = 0; i < FIFO_DEPTH; ++i) {
        *fifo_base_ptr = (i % 512) - 256;; // escribe cada muestra en el FIFO
    }
}

// funcion para verificar si el FIFO tiene datos disponibles
static inline int fifo_data_available() {
  unsigned int level = *fifo_csr_ptr & 0xFFFF;
  return level;
}


//------------------------------------------------------
// FUNCIONES PARA INICIAR Y REGISTRAR LAS INTERRUPCIONES
//------------------------------------------------------

// funcion para iniciar y registrar la interrupcion del switch del low pass filter
void init_lowp_filter_intr(void)
{
    // enable the interrupt for the switch
    *sw_lwp_filter_intr_mask_ptr = 0x1;
	// register the interrupt for the start button
	alt_ic_isr_register(SW_LWP_FILTER_IRQ_INTERRUPT_CONTROLLER_ID, SW_LWP_FILTER_IRQ, low_pass_filter_isr, 0x0, 0x0);
	alt_putstr("Interruption for switch del lowp filter has been registered!\n");
}


//funcion para iniciar y registrar la interrupcion del switch del high pass filter
void init_hgp_filter_intr(void)
{
    // enable the interrupt for the switch
    *sw_hgp_filter_intr_mask_ptr = 0x1;
	// register the interrupt for the start button
	alt_ic_isr_register(SW_HP_FILTER_IRQ_INTERRUPT_CONTROLLER_ID, SW_HP_FILTER_IRQ, high_pass_filter_isr, 0x0, 0x0);
	alt_putstr("Interruption for switch del hgp filter has been registered!\n");
}

//funcion para iniciar y registrar la interrupcion del switch del band pass filter
void init_bdp_filter_intr(void)
{
    // enable the interrupt for the switch
    *sw_bdp_filter_intr_mask_ptr = 0x1;
    // register the interrupt for the start button
    alt_ic_isr_register(SW_BP_FILTER_IRQ_INTERRUPT_CONTROLLER_ID, SW_BP_FILTER_IRQ, band_pass_filter_isr, 0x0, 0x0);
    alt_putstr("Interruption for switch del bdp filter has been registered!\n");
}

//------------------------------------------------------
// FUNCIONES PARA HABILITAR LAS BANDERAS DE CADA FILTRO
//------------------------------------------------------

// funcion para habilitar bandera de filtro pasa bajas
void low_pass_filter_isr(void * context) {
    // verificar si el switch esta activado (tiene que estar en 1 si la interrupcion se genero por el switch)
    if (*sw_lwp_filter_ptr & 0x1)
    {
        // levantar la bandera de solicitud del filtro pasa bajas
        lwp_filter_requested = 1;
    }
}

// funcion para habilitar bandera de filtro pasa altas
void high_pass_filter_isr(void * context) {
    // verificar si el switch esta activado (tiene que estar en 1 si la interrupcion se genero por el switch)
    if (*sw_hgp_filter_ptr & 0x1)
    {
        // levantar la bandera de solicitud del filtro pasa altas
        hgp_filter_requested = 1;
    }
}

// funcion para habilitar bandera de filtro pasa banda
void band_pass_filter_isr(void * context) {
    // verificar si el switch esta activado (tiene que estar en 1 si la interrupcion se genero por el switch)
    if (*sw_bdp_filter_ptr & 0x1)
    {
        // levantar la bandera de solicitud del filtro pasa banda
        bdp_filter_requested = 1;
    }
}



//-----------------------
// FUNCIONES DE FILTROS
//----------------------

// funcion que aplica el filtro pasa bajas
void lwp_filter(void)
{
    const int window = 10;
    for (int i = 0; i < FIFO_DEPTH; i++) {
        int sum = 0, count = 0;
        for (int j = -window; j <= window; j++) {
            int idx = i + j;
            if (idx >= 0 && idx < FIFO_DEPTH) {
                sum += fifo_data_buff[idx];
                count++;
            }
        }
        filter_out_buff[i] = sum / count;
        alt_printf("Value filtered: %x", filter_out_buff[i]);
    }
}


//funcion que aplica el filtro pasa altas
void hgp_filter(void)
{
    const int window = 10;
    for (int i = 0; i < FIFO_DEPTH; i++) {
        int avg = 0, count = 0;
        for (int j = -window; j <= window; j++) {
            int idx = i + j;
            if (idx >= 0 && idx < FIFO_DEPTH) {
                avg += fifo_data_buff[idx];
                count++;
            }
        }
        avg = avg / count;
        filter_out_buff[i] = fifo_data_buff[i] - avg;
        alt_printf("Value filtered: %x", filter_out_buff[i]);
    }
}


//funcion que aplica el filtro pasa banda
void bdp_filter(void)
{
    int16_t temp[FIFO_DEPTH];
    const int lp_window = 3;
    for (int i = 0; i < FIFO_DEPTH; i++) {
        int sum = 0, count = 0;
        for (int j = -lp_window; j <= lp_window; j++) {
            int idx = i + j;
            if (idx >= 0 && idx < FIFO_DEPTH) {
                sum += fifo_data_buff[idx];
                count++;
            }
        }
        temp[i] = sum / count;
    }
    const int hp_window = 12;
    for (int i = 0; i < FIFO_DEPTH; i++) {
        int avg = 0, count = 0;
        for (int j = -hp_window; j <= hp_window; j++) {
            int idx = i + j;
            if (idx >= 0 && idx < FIFO_DEPTH) {
                avg += temp[idx];
                count++;
            }
        }
        avg = avg / count;
        filter_out_buff[i] = temp[i] - avg;
        alt_printf("Value filtered: %x", filter_out_buff[i]);
    }
}
