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
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "pipe_handler.h"

int main(int argc, char *argv[]) {
    char buf[BLOCK_SIZE];
    int fd = -1, buflen;

    if (argc != 2) {
        printf("usage - %s [stuff to write]\n", argv[0]);
        return -1;
    }
    printf("writing: \"%s\"\n", argv[1]);

    snprintf(buf, BLOCK_SIZE, "%s", argv[1]);
    buflen = send_data(PIPE_SET_NAME, buf, strlen(buf), 0.1);
    if (buflen == strlen(buf)) {
        fprintf(stdout, "Successfully written %d bytes\n", buflen);
        buflen = read_data(PIPE_GET_NAME, buf, 10);
        if ((buflen > 0) && (memcmp(buf, "ACK", buflen) == 0)) {
            fprintf(stdout, "Received : %.*s\n", buflen, buf);
        } else {
            return EXIT_FAILURE;
        }
    } else {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
