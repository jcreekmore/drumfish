/*
 * flash.h
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

#ifndef __FLASH_H__
#define __FLASH_H__

struct drumfish_cfg;

uint8_t * flash_open_or_create(const struct drumfish_cfg *config, off_t len);

int flash_load(const char *file, uint8_t *start, size_t len);

int flash_close(uint8_t *flash, size_t len);

#endif /* __FLASH_H__ */
