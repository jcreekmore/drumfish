/*
 * drumfish.c
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
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sim_avr.h>

#include "drumfish.h"
#include "flash.h"

#define MAX_FLASH_FILES 1024

/* We have 1 emulated board and here's the handle to
 * the AVR core.
 */
avr_t *avr = NULL;

static void
handler(int sig)
{
    switch (sig) {
        case SIGINT:
        case SIGTERM:
            avr_terminate(avr);
            exit(EXIT_FAILURE);
            break;

        case SIGHUP:
            avr_reset(avr);
            break;
    }
}

void
usage(const char *argv0)
{
    fprintf(stderr,
"Usage: %s [-p data_path] [-f firmware.hex] [-v] [-m MAC]\n"
"\n"
"  -p data_path - Override default path for storing the EEPROM and flash data\n"
"  -f firmware  - Load the requested 'firmware' file into the device's flash\n"
"  -v           - Increase verbosity of messages\n"
"  -m           - Radio MAC address\n"
"\n"
"Examples:\n"
"  %s -m 00:11:22:00:9E:35\n"
"  %s -f bootloader.hex\n"
"  %s -f bootloader.hex -f payload.hex\n",
argv0, argv0, argv0, argv0);

}

int
main(int argc, char *argv[])
{
    const char *argv0 = argv[0];
    struct drumfish_cfg config;
    struct sigaction act;
    int state = cpu_Limbo;
    int opt;
    char **flash_file = NULL;
    size_t flash_file_len = 0;

    config.mac = NULL;
    config.data_path = NULL;
    config.foreground = 1;
    config.verbose = 0;

    while ((opt = getopt(argc, argv, "f:p:m:vh")) != -1) {
        switch (opt) {
            case 'f':
                /* Increment how many file names we need to keep track of */
                flash_file_len++;

                /* Set a reasonable max which is less than 2^30 to avoid
                 * overflowing realloc().
                 */
                if (flash_file_len > MAX_FLASH_FILES) {
                    fprintf(stderr, "Unable to load more than %d "
                            "firmware images at once.\n", MAX_FLASH_FILES);
                    exit(EXIT_FAILURE);
                }

                /* Make room for our next pointer */
                flash_file = realloc(flash_file,
                        sizeof(void *) * flash_file_len);
                if (!flash_file) {
                    fprintf(stderr, "Failed to allocate memory for "
                            "flash file list.\n");
                    exit(EXIT_FAILURE);
                }

                /* Now let's store our file name */
                flash_file[flash_file_len - 1] = strdup(optarg);
                if (!flash_file[flash_file_len - 1]) {
                    fprintf(stderr, "Failed to allocate memory for "
                            "flash file.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'p':
                config.data_path = strdup(optarg);
                if (!config.data_path) {
                    fprintf(stderr, "Failed to allocate memory for "
                            "data path.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'm':
                config.mac = strdup(optarg);
                if (!config.mac) {
                    fprintf(stderr, "Failed to allocate memory for MAC.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
               config.verbose++;
               break;
            case 'V':
               /* print version */
               break;
            case 'h':
               usage(argv0);
               exit(EXIT_SUCCESS);
               break;
            default: /* '?' */
                usage(argv0);
                exit(EXIT_FAILURE);
        }
    }

    /* Handle the bare minimum signals */
    /* Yes I should use sigset_t here and use sigemptyset() */
    memset(&act, 0, sizeof(act));

    act.sa_handler = handler;
    if (sigaction(SIGHUP, &act, NULL) < 0) {
        fprintf(stderr, "Failed to install SIGHUP handler\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGINT, &act, NULL) < 0) {
        fprintf(stderr, "Failed to install SIGINT handler\n");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGTERM, &act, NULL) < 0) {
        fprintf(stderr, "Failed to install SIGTERM handler\n");
        exit(EXIT_FAILURE);
    }

    avr = m128rfa1_start(&config);
    if (!avr) {
        fprintf(stderr, "Unable to initialize requested board.\n");
        exit(EXIT_FAILURE);
    }

    /* Flash in any requested firmware, while cleaning up our memory */
    for (int i = 0; i < flash_file_len; i++) {
        if (flash_load(flash_file[i], avr->flash, avr->flashend + 1)) {
            fprintf(stderr, "Failed to load '%s' into flash.\n", flash_file[i]);
            exit(EXIT_FAILURE);
        }
        /* Don't need this memory anymore */
        free(flash_file[i]);
    }
    free(flash_file);

    /* Our main event loop */
    for (;;) {
        state = avr_run(avr);
        if (state == cpu_Done || state == cpu_Crashed)
            break;
    }

    avr_terminate(avr);
}
