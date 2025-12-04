/*
 * system.h - SOPC Builder system and BSP software package information
 *
 * Machine generated for CPU 'NIOSII' in SOPC Builder design 'timer_display'
 * SOPC Builder design path: ../../timer_display.sopcinfo
 *
 * Generated: Wed Nov 26 21:23:26 CST 2025
 */

/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */

/*
 * License Agreement
 *
 * Copyright (c) 2008
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/* Include definitions from linker script generator */
#include "linker.h"


/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "altera_nios2_gen2"
#define ALT_CPU_BIG_ENDIAN 0
#define ALT_CPU_BREAK_ADDR 0x00020820
#define ALT_CPU_CPU_ARCH_NIOS2_R1
#define ALT_CPU_CPU_FREQ 50000000u
#define ALT_CPU_CPU_ID_SIZE 1
#define ALT_CPU_CPU_ID_VALUE 0x00000000
#define ALT_CPU_CPU_IMPLEMENTATION "tiny"
#define ALT_CPU_DATA_ADDR_WIDTH 0x12
#define ALT_CPU_DCACHE_LINE_SIZE 0
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 0
#define ALT_CPU_DCACHE_SIZE 0
#define ALT_CPU_EXCEPTION_ADDR 0x00010020
#define ALT_CPU_FLASH_ACCELERATOR_LINES 0
#define ALT_CPU_FLASH_ACCELERATOR_LINE_SIZE 0
#define ALT_CPU_FLUSHDA_SUPPORTED
#define ALT_CPU_FREQ 50000000
#define ALT_CPU_HARDWARE_DIVIDE_PRESENT 0
#define ALT_CPU_HARDWARE_MULTIPLY_PRESENT 0
#define ALT_CPU_HARDWARE_MULX_PRESENT 0
#define ALT_CPU_HAS_DEBUG_CORE 1
#define ALT_CPU_HAS_DEBUG_STUB
#define ALT_CPU_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define ALT_CPU_HAS_JMPI_INSTRUCTION
#define ALT_CPU_ICACHE_LINE_SIZE 0
#define ALT_CPU_ICACHE_LINE_SIZE_LOG2 0
#define ALT_CPU_ICACHE_SIZE 0
#define ALT_CPU_INST_ADDR_WIDTH 0x12
#define ALT_CPU_NAME "NIOSII"
#define ALT_CPU_OCI_VERSION 1
#define ALT_CPU_RESET_ADDR 0x00010000


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define NIOS2_BIG_ENDIAN 0
#define NIOS2_BREAK_ADDR 0x00020820
#define NIOS2_CPU_ARCH_NIOS2_R1
#define NIOS2_CPU_FREQ 50000000u
#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0x00000000
#define NIOS2_CPU_IMPLEMENTATION "tiny"
#define NIOS2_DATA_ADDR_WIDTH 0x12
#define NIOS2_DCACHE_LINE_SIZE 0
#define NIOS2_DCACHE_LINE_SIZE_LOG2 0
#define NIOS2_DCACHE_SIZE 0
#define NIOS2_EXCEPTION_ADDR 0x00010020
#define NIOS2_FLASH_ACCELERATOR_LINES 0
#define NIOS2_FLASH_ACCELERATOR_LINE_SIZE 0
#define NIOS2_FLUSHDA_SUPPORTED
#define NIOS2_HARDWARE_DIVIDE_PRESENT 0
#define NIOS2_HARDWARE_MULTIPLY_PRESENT 0
#define NIOS2_HARDWARE_MULX_PRESENT 0
#define NIOS2_HAS_DEBUG_CORE 1
#define NIOS2_HAS_DEBUG_STUB
#define NIOS2_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define NIOS2_HAS_JMPI_INSTRUCTION
#define NIOS2_ICACHE_LINE_SIZE 0
#define NIOS2_ICACHE_LINE_SIZE_LOG2 0
#define NIOS2_ICACHE_SIZE 0
#define NIOS2_INST_ADDR_WIDTH 0x12
#define NIOS2_OCI_VERSION 1
#define NIOS2_RESET_ADDR 0x00010000


/*
 * DISPLAY configuration
 *
 */

#define ALT_MODULE_CLASS_DISPLAY altera_avalon_pio
#define DISPLAY_BASE 0x21030
#define DISPLAY_BIT_CLEARING_EDGE_REGISTER 0
#define DISPLAY_BIT_MODIFYING_OUTPUT_REGISTER 0
#define DISPLAY_CAPTURE 0
#define DISPLAY_DATA_WIDTH 28
#define DISPLAY_DO_TEST_BENCH_WIRING 0
#define DISPLAY_DRIVEN_SIM_VALUE 0
#define DISPLAY_EDGE_TYPE "NONE"
#define DISPLAY_FREQ 50000000
#define DISPLAY_HAS_IN 0
#define DISPLAY_HAS_OUT 1
#define DISPLAY_HAS_TRI 0
#define DISPLAY_IRQ -1
#define DISPLAY_IRQ_INTERRUPT_CONTROLLER_ID -1
#define DISPLAY_IRQ_TYPE "NONE"
#define DISPLAY_NAME "/dev/DISPLAY"
#define DISPLAY_RESET_VALUE 0
#define DISPLAY_SPAN 16
#define DISPLAY_TYPE "altera_avalon_pio"


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __ALTERA_AVALON_PIO
#define __ALTERA_AVALON_TIMER
#define __ALTERA_NIOS2_GEN2


/*
 * PAUSE_BUTTON configuration
 *
 */

#define ALT_MODULE_CLASS_PAUSE_BUTTON altera_avalon_pio
#define PAUSE_BUTTON_BASE 0x21020
#define PAUSE_BUTTON_BIT_CLEARING_EDGE_REGISTER 1
#define PAUSE_BUTTON_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PAUSE_BUTTON_CAPTURE 1
#define PAUSE_BUTTON_DATA_WIDTH 1
#define PAUSE_BUTTON_DO_TEST_BENCH_WIRING 1
#define PAUSE_BUTTON_DRIVEN_SIM_VALUE 0
#define PAUSE_BUTTON_EDGE_TYPE "FALLING"
#define PAUSE_BUTTON_FREQ 50000000
#define PAUSE_BUTTON_HAS_IN 1
#define PAUSE_BUTTON_HAS_OUT 0
#define PAUSE_BUTTON_HAS_TRI 0
#define PAUSE_BUTTON_IRQ 4
#define PAUSE_BUTTON_IRQ_INTERRUPT_CONTROLLER_ID 0
#define PAUSE_BUTTON_IRQ_TYPE "EDGE"
#define PAUSE_BUTTON_NAME "/dev/PAUSE_BUTTON"
#define PAUSE_BUTTON_RESET_VALUE 0
#define PAUSE_BUTTON_SPAN 16
#define PAUSE_BUTTON_TYPE "altera_avalon_pio"


/*
 * RAM configuration
 *
 */

#define ALT_MODULE_CLASS_RAM altera_avalon_onchip_memory2
#define RAM_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define RAM_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define RAM_BASE 0x10000
#define RAM_CONTENTS_INFO ""
#define RAM_DUAL_PORT 0
#define RAM_GUI_RAM_BLOCK_TYPE "AUTO"
#define RAM_INIT_CONTENTS_FILE "timer_display_RAM"
#define RAM_INIT_MEM_CONTENT 1
#define RAM_INSTANCE_ID "NONE"
#define RAM_IRQ -1
#define RAM_IRQ_INTERRUPT_CONTROLLER_ID -1
#define RAM_NAME "/dev/RAM"
#define RAM_NON_DEFAULT_INIT_FILE_ENABLED 0
#define RAM_RAM_BLOCK_TYPE "AUTO"
#define RAM_READ_DURING_WRITE_MODE "DONT_CARE"
#define RAM_SINGLE_CLOCK_OP 0
#define RAM_SIZE_MULTIPLE 1
#define RAM_SIZE_VALUE 65536
#define RAM_SPAN 65536
#define RAM_TYPE "altera_avalon_onchip_memory2"
#define RAM_WRITABLE 1


/*
 * RESET_BUTTON configuration
 *
 */

#define ALT_MODULE_CLASS_RESET_BUTTON altera_avalon_pio
#define RESET_BUTTON_BASE 0x21040
#define RESET_BUTTON_BIT_CLEARING_EDGE_REGISTER 1
#define RESET_BUTTON_BIT_MODIFYING_OUTPUT_REGISTER 0
#define RESET_BUTTON_CAPTURE 1
#define RESET_BUTTON_DATA_WIDTH 1
#define RESET_BUTTON_DO_TEST_BENCH_WIRING 1
#define RESET_BUTTON_DRIVEN_SIM_VALUE 0
#define RESET_BUTTON_EDGE_TYPE "FALLING"
#define RESET_BUTTON_FREQ 50000000
#define RESET_BUTTON_HAS_IN 1
#define RESET_BUTTON_HAS_OUT 0
#define RESET_BUTTON_HAS_TRI 0
#define RESET_BUTTON_IRQ 2
#define RESET_BUTTON_IRQ_INTERRUPT_CONTROLLER_ID 0
#define RESET_BUTTON_IRQ_TYPE "EDGE"
#define RESET_BUTTON_NAME "/dev/RESET_BUTTON"
#define RESET_BUTTON_RESET_VALUE 0
#define RESET_BUTTON_SPAN 16
#define RESET_BUTTON_TYPE "altera_avalon_pio"


/*
 * START_BUTTON configuration
 *
 */

#define ALT_MODULE_CLASS_START_BUTTON altera_avalon_pio
#define START_BUTTON_BASE 0x21050
#define START_BUTTON_BIT_CLEARING_EDGE_REGISTER 1
#define START_BUTTON_BIT_MODIFYING_OUTPUT_REGISTER 0
#define START_BUTTON_CAPTURE 1
#define START_BUTTON_DATA_WIDTH 1
#define START_BUTTON_DO_TEST_BENCH_WIRING 1
#define START_BUTTON_DRIVEN_SIM_VALUE 0
#define START_BUTTON_EDGE_TYPE "FALLING"
#define START_BUTTON_FREQ 50000000
#define START_BUTTON_HAS_IN 1
#define START_BUTTON_HAS_OUT 0
#define START_BUTTON_HAS_TRI 0
#define START_BUTTON_IRQ 1
#define START_BUTTON_IRQ_INTERRUPT_CONTROLLER_ID 0
#define START_BUTTON_IRQ_TYPE "EDGE"
#define START_BUTTON_NAME "/dev/START_BUTTON"
#define START_BUTTON_RESET_VALUE 0
#define START_BUTTON_SPAN 16
#define START_BUTTON_TYPE "altera_avalon_pio"


/*
 * System configuration
 *
 */

#define ALT_DEVICE_FAMILY "Cyclone V"
#define ALT_ENHANCED_INTERRUPT_API_PRESENT
#define ALT_IRQ_BASE NULL
#define ALT_LOG_PORT "/dev/null"
#define ALT_LOG_PORT_BASE 0x0
#define ALT_LOG_PORT_DEV null
#define ALT_LOG_PORT_TYPE ""
#define ALT_NUM_EXTERNAL_INTERRUPT_CONTROLLERS 0
#define ALT_NUM_INTERNAL_INTERRUPT_CONTROLLERS 1
#define ALT_NUM_INTERRUPT_CONTROLLERS 1
#define ALT_STDERR "/dev/UART"
#define ALT_STDERR_BASE 0x21060
#define ALT_STDERR_DEV UART
#define ALT_STDERR_IS_JTAG_UART
#define ALT_STDERR_PRESENT
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN "/dev/UART"
#define ALT_STDIN_BASE 0x21060
#define ALT_STDIN_DEV UART
#define ALT_STDIN_IS_JTAG_UART
#define ALT_STDIN_PRESENT
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT "/dev/UART"
#define ALT_STDOUT_BASE 0x21060
#define ALT_STDOUT_DEV UART
#define ALT_STDOUT_IS_JTAG_UART
#define ALT_STDOUT_PRESENT
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_SYSTEM_NAME "timer_display"


/*
 * TIMER configuration
 *
 */

#define ALT_MODULE_CLASS_TIMER altera_avalon_timer
#define TIMER_ALWAYS_RUN 0
#define TIMER_BASE 0x21000
#define TIMER_COUNTER_SIZE 32
#define TIMER_FIXED_PERIOD 1
#define TIMER_FREQ 50000000
#define TIMER_IRQ 0
#define TIMER_IRQ_INTERRUPT_CONTROLLER_ID 0
#define TIMER_LOAD_VALUE 49999999
#define TIMER_MULT 1.0
#define TIMER_NAME "/dev/TIMER"
#define TIMER_PERIOD 1
#define TIMER_PERIOD_UNITS "s"
#define TIMER_RESET_OUTPUT 0
#define TIMER_SNAPSHOT 1
#define TIMER_SPAN 32
#define TIMER_TICKS_PER_SEC 1
#define TIMER_TIMEOUT_PULSE_OUTPUT 0
#define TIMER_TYPE "altera_avalon_timer"


/*
 * UART configuration
 *
 */

#define ALT_MODULE_CLASS_UART altera_avalon_jtag_uart
#define UART_BASE 0x21060
#define UART_IRQ 3
#define UART_IRQ_INTERRUPT_CONTROLLER_ID 0
#define UART_NAME "/dev/UART"
#define UART_READ_DEPTH 64
#define UART_READ_THRESHOLD 8
#define UART_SPAN 8
#define UART_TYPE "altera_avalon_jtag_uart"
#define UART_WRITE_DEPTH 64
#define UART_WRITE_THRESHOLD 8


/*
 * hal configuration
 *
 */

#define ALT_INCLUDE_INSTRUCTION_RELATED_EXCEPTION_API
#define ALT_MAX_FD 4
#define ALT_SYS_CLK none
#define ALT_TIMESTAMP_CLK none

#endif /* __SYSTEM_H_ */
