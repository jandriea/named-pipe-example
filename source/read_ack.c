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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "pipe_handler.h"

int main(int argc, char *argv[]) {
    char buf[BLOCK_SIZE];
    int buflen;

    buflen = read_data(PIPE_SET_NAME, buf, 10);
    if (buflen > 0) {
        fprintf(stdout, "Received : %.*s\n", buflen, buf);
        fprintf(stdout, "Sending ACK\n");
        char ack[] = "ACK";
        buflen = send_data(PIPE_GET_NAME, ack, strlen(ack), 0.1);
        if (buflen == strlen(ack)) {
            fprintf(stdout, "Successfully written %d bytes\n", buflen);
        }
        else {
            return EXIT_FAILURE;
        }
    }
    else {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}