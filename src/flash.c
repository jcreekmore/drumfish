/*
 * flash.c
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
#include <sys/mman.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sim_hex.h>

#include "drumfish.h"
#include "flash.h"

static int
flash_create_dir(const char *path)
{
    char *dir;
    char *path_copy = NULL;

    /* Make a local copy so we can modify it */
    path_copy = strdup(path);

    /* Now let's stuff a NULL byte at the last component */
    dir = dirname(path_copy);

try_again:
    /* Create the directory */
    if (mkdir(dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP)) {
        /* if ENOENT is set then a path component is missing and
         * we need to create the parent
         */
        if (errno == ENOENT) {
            /* If it worked, try this directory again */
            if (!flash_create_dir(dir))
                goto try_again;
        }

        fprintf(stderr, "Failed to create directory '%s': %s\n",
                dir, strerror(errno));

        free(path_copy);
        return -1;
    }

    free(path_copy);
    return 0;
}

uint8_t *
flash_open_or_create(const struct drumfish_cfg *config, off_t len)
{
    int fd = -1;
    struct stat st;
    int ret;
    int must_ff = 0;
    uint8_t *buf;
    const char *file = config->pflash;

try_again:
    fd = open(file, O_RDWR | O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP);
    if (fd == -1) {
        /* if errno is ENOENT, then a part of the path doesn't exist
         * or a symbolic link in the path is busted so we'll try to
         * make the path
         */
        if (errno == ENOENT) {
            /* if the path was mad successfully, try the whole process again */
            if (!flash_create_dir(file))
                goto try_again;
        }

        fprintf(stderr, "Unable to open or create '%s' %d: %s\n",
                file, errno, strerror(errno));
        goto err;
    }

    if (fstat(fd, &st)) {
        fprintf(stderr, "Unable to get file info for '%s': %s\n",
                file, strerror(errno));
        goto err;
    }

    /* If the existing flash file is less than the size we need,
     * allocate the space/extend the file. We don't use ftruncate() since
     * on some file systems its unable to grow files.
     */
    if (st.st_size < len) {
        ret = posix_fallocate(fd, 0, len);
        if (ret) {
            fprintf(stderr, "Unable to create '%s' with %zu bytes: %s\n",
                    file, len, strerror(ret));
            goto err;
        }

        /* We need to clear out the current flash with 0xFF */
        must_ff = 1;
    } else if (st.st_size > len) {
        fprintf(stderr, "The flash file '%s' supplied is larger than "
                "the supported size of %zu. Your code might french fry "
                "when it should pizza.\n", file, len);
    }

    buf = mmap(0, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
        fprintf(stderr, "Failed to map '%s': %s\n", file, strerror(errno));
        goto err;
    }

    /* Once the file is mapped we can drop the fd */
    close(fd);
    fd = -1;

    /* Now we set the 0xFF if needed */
    if (must_ff)
        memset(buf, 0xFF, len);

    return buf;

err:
    if (fd != -1)
        close(fd);

    return NULL;
}

int
flash_load(const char *file, uint8_t *start, size_t len)
{
    int items;
    ihex_chunk_p chunks;
    int i;
    int retval = -1;

    items = read_ihex_chunks(file, &chunks);

    for (i = 0; i < items; i++) {
        if (chunks[i].baseaddr + chunks[i].size > len) {
            fprintf(stderr, "Firmware file would exceed max size of flash. "
                    "Max size: %zu. Firmware baseaddr: %04x, size: %d\n",
                    len, chunks[i].baseaddr, chunks[i].size);
            goto cleanup;
        }
        memcpy(start + chunks[i].baseaddr, chunks[i].data, chunks[i].size);
        printf("Loading '%s' into flash at %04x, size %d\n",
                file, chunks[i].baseaddr, chunks[i].size);

    }

    retval = 0;

cleanup:
    for (i = 0; i < items; i++) {
        free(chunks[i].data);
    }
    free(chunks);

    return retval;
}


int
flash_close(uint8_t *flash, size_t len)
{
    if (!flash)
        return -1;

    if (munmap(flash, len)) {
        fprintf(stderr, "Unable to cleanly close flash memory.\n");
        return -1;
    }

    return 0;
}

