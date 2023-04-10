/*
 * MIT License
 * 
 * Copyright (c) 2023 Jatty Andriean
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "pipe_handler.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage - %s [stuff to write]\n", argv[0]);
        return -1;
    }
    printf("writing: \"%s\"\n", argv[1]);

    char buf[BLOCK_SIZE];
    int buflen;
    int fd;

    snprintf(buf, BLOCK_SIZE, "%s", argv[1]);
    buflen = strlen(buf);

    if (access(PIPE_SET_NAME, F_OK) == -1) {
        if (mkfifo(PIPE_SET_NAME, 0666) == -1) {
            fprintf(stderr, "Error creating named pipe: %s\n", strerror(errno));
            return -1;
        }
    }

    if ((fd = open(PIPE_SET_NAME, O_WRONLY | O_NONBLOCK)) == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            fprintf(stderr, "Error opening named pipe for writing: %s\n", strerror(errno));
            return -1;
        }
    }
    int bytes_written = write(fd, buf, buflen);
    if (bytes_written == -1) {
        fprintf(stderr, "Error writing to named pipe: %s\n", strerror(errno));
        return -1;
    }

    if (bytes_written != buflen) {
        fprintf(stderr, "Failed to write all data: %d != %d\n", bytes_written, buflen);
        return -1;
    }

    close(fd);
    return EXIT_SUCCESS;
}
