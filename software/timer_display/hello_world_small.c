/*********************************************************
 * Timer on 7 segments display program with interrupts
**********************************************************/
#include "sys/alt_stdio.h"
#include <system.h>
#include <sys/alt_irq.h>

// ** POINTERS TO COMPONENTS MEMORY DIRECTIONS **
volatile unsigned int * timer_status_ptr = (unsigned int *) TIMER_BASE;
volatile unsigned int * timer_ctrl_ptr = (unsigned int *) TIMER_BASE + 1;
volatile unsigned int * start_button_pio_intr_mask = (unsigned int *) START_BUTTON_BASE + 2;
volatile unsigned int * start_button_pio_edge_capture = (unsigned int *) START_BUTTON_BASE + 3;
volatile unsigned int * pause_button_pio_intr_mask = (unsigned int *) PAUSE_BUTTON_BASE + 2;
volatile unsigned int * pause_button_pio_edge_capture = (unsigned int *) PAUSE_BUTTON_BASE + 3;
volatile unsigned int * reset_button_pio_intr_mask = (unsigned int *) RESET_BUTTON_BASE + 2;
volatile unsigned int * reset_button_pio_edge_capture = (unsigned int *) RESET_BUTTON_BASE + 3;
volatile unsigned int * display_ptr = (unsigned int *) DISPLAY_BASE;

// ** GLOBAL VARIABLES **
unsigned int seconds_counter;
// bit-mask for every number on display
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

// ** FUNCTION DECLARATIONS **
// interrupt Service Routines
void timer_isr (void * context);
void start_button_isr (void * context);
void pause_button_isr (void * context);
void reset_button_isr (void * context);

// initialization functions
int init_timer_intr (void);
int init_start_button_intr(void);
int init_pause_button_intr(void);
int init_reset_button_intr(void);

int main()
{
	alt_putstr("Initializing program! ...\n");
	init_start_button_intr();
	init_pause_button_intr();
	init_reset_button_intr();
	alt_putstr("Waiting for start button! ...\n");

	while (1);

	return 0;
}

int init_timer_intr (void)
{
	// register the interrupt for the timeout
	alt_ic_isr_register(TIMER_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_IRQ, timer_isr, 0x0, 0x0);

	// initialize the timer
	alt_putstr("Turning on the timer...\n");
	// write corresponding values to 'control' register of the timer
	*timer_ctrl_ptr = 0x7; // 0x7 = 0(STOP) 1(START) 1(CONT) 1(ITO)

	return 0;
}

int init_start_button_intr (void)
{
	// clean the register by writing 1
	*start_button_pio_edge_capture = 0x1;
	// enable the interrupts on the port
	*start_button_pio_intr_mask = 0x1;
	// register the interrupt for the start button
	alt_ic_isr_register(START_BUTTON_IRQ_INTERRUPT_CONTROLLER_ID, START_BUTTON_IRQ, start_button_isr, 0x0, 0x0);
	alt_putstr("Interruption for start button has been registered!\n");

	return 0;
}

int init_pause_button_intr (void)
{
	// clean the register by writing 1
	*pause_button_pio_edge_capture = 0x1;
	// enable the interrupts on the port
	*pause_button_pio_intr_mask = 0x1;
	// register the interrupt for the start button
	alt_ic_isr_register(PAUSE_BUTTON_IRQ_INTERRUPT_CONTROLLER_ID, PAUSE_BUTTON_IRQ, pause_button_isr, 0x0, 0x0);
	alt_putstr("Interruption for pause button has been registered!\n");

	return 0;
}

int init_reset_button_intr (void)
{
	// clean the register by writing 1
	*reset_button_pio_edge_capture = 0x1;
	// enable the interrupts on the port
	*reset_button_pio_intr_mask = 0x1;
	// register the interrupt for the reset button
	alt_ic_isr_register(RESET_BUTTON_IRQ_INTERRUPT_CONTROLLER_ID, RESET_BUTTON_IRQ, reset_button_isr, 0x0, 0x0);
	alt_putstr("Interruption for reset button has been registered!\n");

	return 0;
}

void timer_isr (void * context)
{
	// clean the status register so that it continue counting
	*timer_status_ptr = 0;
	// increment the seconds counter
	seconds_counter++;
	// conversion to MM.SS format
	int minutes = (seconds_counter / 60) % 100;
	int seconds = seconds_counter % 60;
	// assign every digit to different values (numbers that are going to be on each display)
	int d1 = minutes / 10;
	int d2 = minutes % 10;
	int d3 = seconds / 10;
	int d4 = seconds % 10;

	// packet the 28 bits
	unsigned int display_value =
		(seg7[d1] << 21) |
		(seg7[d2] << 14) |
		(seg7[d3] << 7 ) |
		(seg7[d4]      );

	// write to PIO
	*display_ptr = display_value;
	// print the time values
	alt_printf("Current time: %x%x:%x%x\n", d1, d2, d3, d4);
}

void start_button_isr (void * context)
{
	// read the edge-capture register to verify if the interrupt has been asserted
	unsigned int edge = *start_button_pio_edge_capture;
	// clean the register by writing 0x1 (read value)
	*start_button_pio_edge_capture = edge;

	if (edge & 0x1)
	{
		alt_putstr("edge detected\n");
		// verify that the timer isn't running and there isn't a timeout yet
		if (*timer_status_ptr == 0)
		{
			init_timer_intr();
		}
		// if not, call the timer initialization function
		else
		{
			alt_putstr("INFO: timer is already running!");
		}
	}

}

void pause_button_isr (void * context)
{
	// read the edge-capture register to verify if the interrupt has been asserted
	unsigned int edge = *pause_button_pio_edge_capture;
	// clean the register by writing 0x1 (read value)
	*pause_button_pio_edge_capture = edge;

	alt_putstr("Pause interruption captured\n");

	if (edge & 0x1)
	{
		//verify that the timer is running
		if (*timer_status_ptr == 0x2 || *timer_status_ptr == 0x3)
		{
			// stop the timer
			alt_putstr("Pausing the timer...\n");
			// write corresponding values to 'control' register of the timer
			*timer_ctrl_ptr = 0x8; // 0x7 = 1(STOP) 0(START) 0(CONT) 0(ITO)
			// write 1 in "TO" bit to clean the register
			*timer_status_ptr = 0x1;
		}
		else
		{
			alt_putstr("The timer is already stopped!\n");
		}
	}

}

void reset_button_isr (void * context)
{
	// read the edge-capture register to verify if the interrupt has been asserted
	unsigned int edge = *reset_button_pio_edge_capture;
	// clean the register by writing 0x1 (read value)
	*reset_button_pio_edge_capture = edge;

	if (edge & 0x1)
	{
		// write corresponding values to 'control' register of the timer
		*timer_ctrl_ptr = 0x8; // 0x7 = 1(STOP) 0(START) 0(CONT) 0(ITO)
		// reset the counter
		seconds_counter = 0;
		// reset the displays with 0
		unsigned int display_value =
			(seg7[0] << 21) |
			(seg7[0] << 14) |
			(seg7[0] << 7 ) |
			(seg7[0]      );

		// write to PIO the 0
		*display_ptr = display_value;
		alt_putstr("Reset button pressed!, counter reset\n");
		alt_putstr("Waiting for start button!...\n");
	}

}
