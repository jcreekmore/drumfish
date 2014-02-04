/*
 * m128rfa1.c
 *
 *  Copyright (c) 2014 Doug Goldstein <cardoe@cardoe.com>
 *
 *  This file is part of drumfish.
 *
 *  drumfish is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  drumfish is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with drumfish.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sim_avr.h>
#include "uart_pty.h"

#include "flash.h"

#define PC_START 0x1f800

uart_pty_t uart_pty_0;
uart_pty_t uart_pty_1;

static void
m128rfa1_init(avr_t *avr)
{
    avr->flash = flash_open_or_create("test.flash", avr->flashend + 1);
}

static void
m128rfa1_deinit(avr_t *avr)
{
    uart_pty_stop(&uart_pty_0);
    uart_pty_stop(&uart_pty_1);

    flash_close(avr->flash, avr->flashend + 1);
    avr->flash = NULL;
}

avr_t *
m128rfa1_start(void)
{
    avr_t *avr;

    avr = avr_make_mcu_by_name("atmega128rfa1");
    if (!avr) {
        fprintf(stderr, "Failed to create AVR core 'atmega128rfa1'\n");
        return NULL;
    }

    /* Setup any additional init/deinit routines */
    avr->special_init = m128rfa1_init;
    avr->special_deinit = m128rfa1_deinit;

    /* Initialize our AVR */
    avr_init(avr);

    /* Our chips always run at 16mhz */
    avr->frequency = 16000000;

    /* Set our fuses */
    avr->fuse[0] = 0xEF;
    avr->fuse[1] = 0xE6;
    avr->fuse[2] = 0x1C;
    //avr->fuse[3] = 0xFE;

    /* Check to see if we initialized our flash */
    if (!avr->flash) {
        fprintf(stderr, "Failed to initialize flash correctly.\n");
        return NULL;
    }

    /* Based on fuse values, we'll always want to boot from the bootloader
     * which will always start at 0x1f800.
     */
    avr->pc = PC_START;
    avr->codeend = avr->flashend;

    /* Setup our UARTs */
    uart_pty_init(avr, &uart_pty_0);
    uart_pty_connect(&uart_pty_0, '0');

    uart_pty_init(avr, &uart_pty_1);
    uart_pty_connect(&uart_pty_1, '1');

    printf("Booting from 0x%04x.\n", PC_START);

    return avr;
}
