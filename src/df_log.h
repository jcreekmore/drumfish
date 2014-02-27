/*
 * df_log.h
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


#ifndef __DF_LOG_H__
#define __DF_LOG_H__

/* Possible logging levels */
enum df_log_lvl {
    DF_LOG_ERR = 0,
    DF_LOG_WARN,
    DF_LOG_INFO,
    DF_LOG_DEBUG
};

/* Forward declaration */
struct drumfish_cfg;

void df_log_init(struct drumfish_cfg *config);

void df_log_start_time(void);

void df_log_msg(enum df_log_lvl level, const char *format, ...)
    __attribute__ ((format (printf, 2, 3)));

#endif /* __DF_LOG_H__ */
