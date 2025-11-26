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
volatile unsigned int * reset_button_pio_intr_mask = (unsigned int *) RESET_BUTTON_BASE + 2;
volatile unsigned int * reset_button_pio_edge_capture = (unsigned int *) RESET_BUTTON_BASE + 3;

// ** GLOBAL VARIABLES **
unsigned int seconds_counter;

// ** FUNCTION DECLARATIONS **
// interrupt Service Routines
void timer_isr (void * context);
void start_button_isr (void * context);
void reset_button_isr (void * context);

// initialization functions
int init_timer_intr (void);
int init_start_button_intr(void);
int init_reset_button_intr(void);


int main()
{
	alt_putstr("Initializing program! ...\n");
	init_start_button_intr();
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
	// increment counter and print the value
	alt_printf("INFO: SECONDS COUNT: %x\n", seconds_counter++);
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
			alt_printf("INFO: timer is already running! %x\n", *timer_status_ptr);
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
		alt_putstr("Reset button pressed!, counter reset\n");
		alt_putstr("Waiting for start button! ...\n");
	}

}
