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
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#define PIPE_SET_NAME "/tmp/my_pipe_set"
#define PIPE_GET_NAME "/tmp/my_pipe_get"

#define BLOCK_SIZE 4096

// Signal
volatile sig_atomic_t stop = 0;

void sigint_handler(int signum) {
    stop = 1;
}

int read_from_pipe() {
    int fd;
    char buf[BLOCK_SIZE];
    ssize_t num_read;

    while (!stop) {
        if (access(PIPE_SET_NAME, F_OK) == -1) {
            // The named pipe doesn't exist yet.
            sleep(1);
            continue;
        }

        // The named pipe exists, so try to open it for reading.
        fd = open(PIPE_SET_NAME, O_RDONLY | O_NONBLOCK);
        if (fd == -1) {
            fprintf(stderr, "Error opening named pipe for reading: %s\n", strerror(errno));
            sleep(1);
            continue;
        }

        num_read = read(fd, buf, BLOCK_SIZE);
        if (num_read == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                fprintf(stderr, "Error reading from named pipe: %s\n", strerror(errno));
                close(fd);
                return -1;
            }
        } else if (num_read == 0) {
            printf("Nothing to read, sleeping...\n");
            sleep(1);
        } else {
            printf("Received data: %.*s\n", (int) num_read, buf);
            if (memcmp(buf, "quit", (int) num_read) == 0) {
                break;
            }
            // Clean the buffer
            memset(buf, 0, BLOCK_SIZE);
        }
    }
    close(fd);
    return 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);  // setup signal handler for SIGINT
    read_from_pipe();
    return EXIT_SUCCESS;
}
