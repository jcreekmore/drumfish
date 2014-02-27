/*
 * df_log.c
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

#define _GNU_SOURCE

#include <sys/time.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drumfish.h"
#include "df_log.h"

static enum df_log_lvl verbosity = 0;
static struct timeval start_time;

void
df_log_init(struct drumfish_cfg *config)
{
    verbosity = (enum df_log_lvl) config->verbose;
    timerclear(&start_time);
}

void
df_log_start_time(void)
{
    if (gettimeofday(&start_time, NULL))
        timerclear(&start_time);
}

void
df_log_msg(enum df_log_lvl level, const char *format, ...)
{
    va_list ap;
    struct timeval now;
    struct timeval offset;
    char *msg;

    va_start(ap, format);

    /* If its a message we will not print, skip it */
    if (level <= verbosity) {
        /* Get our current time for stamping the message */
        if (gettimeofday(&now, NULL)) {
            fprintf(stderr, "Failed to get current time: %s\n",
                    strerror(errno));
            timerclear(&now);
        }

        /* Compute time since the CPU started */
        if (timerisset(&start_time))
            timersub(&now, &start_time, &offset);
        else
            timerclear(&offset);

        /* Build the message and print it with the time stamp */
        vasprintf(&msg, format, ap);
        fprintf(stderr, "[%5ld.%06ld] %s", offset.tv_sec, offset.tv_usec,
                msg);
        free(msg);
    }

    va_end(ap);

}
