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
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include "pipe_handler.h"

// Signal
volatile sig_atomic_t stop = 0;

void sigint_handler(int signum) {
    stop = 1;
}

static void sleep_ms(int timeout) {
    usleep(timeout * 1000);
}

int read_loop() {
    int fd, buflen;
    char buf[BLOCK_SIZE];
    ssize_t num_read;

    fd = open_pipe(PIPE_SET_NAME, true);
    if (fd == -1) {
        return -1;
    }

    while(!stop) {
        num_read = read(fd, buf, BLOCK_SIZE);
        if (num_read == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                fprintf(stderr, "Error reading from named pipe: %s\n", strerror(errno));
                close(fd);
                return -1;
            }
        } else if (num_read == 0) {
            // printf("Nothing to read, sleeping...\n");
            sleep_ms(10);
        } else {
            printf("Received data: %.*s\n", (int) num_read, buf);
            fprintf(stdout, "Sending ACK\n");
            char ack[] = "ACK";
            buflen = send_data(PIPE_GET_NAME, ack, strlen(ack), 0.1);
            if (buflen == strlen(ack)) {
                fprintf(stdout, "Successfully written %d bytes\n", buflen);
            }

            if (strcmp(buf, "quit") == 0) {
                break;
            }
            // Clean the buffer
            memset(buf, 0, BLOCK_SIZE);
        }
    }

    if (fd) {
        close(fd);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);  // setup signal handler for SIGINT
    read_loop();
    return EXIT_SUCCESS;
}
