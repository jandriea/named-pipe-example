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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#include "pipe_handler.h"

/**
 * Opens a named pipe with the given name and returns a file descriptor.
 *
 * @param name The name of the named pipe.
 * @param isRead A flag indicating whether the pipe should be opened for reading (if true) or writing (if false).
 * @return The file descriptor of the opened named pipe, or -1 on error.
 * @throws If an error occurs while creating or opening the named pipe, an appropriate error message will be printed to stderr.
 *
 * Example usage:
 * int pipe_fd = open_pipe("/tmp/my_pipe", true);
 * char buffer[256];
 * int num_bytes_read = read(pipe_fd, buffer, 256);
 */
int open_pipe(const char *name, bool isRead) {
    int fd;
    if (access(name, F_OK) == -1) {
        if (mkfifo(name, 0666) == -1) {
            fprintf(stderr, "Error creating named pipe: %s\n", strerror(errno));
            return -1;
        }
    }

    if (isRead) {
        if ((fd = open(name, O_RDONLY | O_NONBLOCK)) == -1) {
            fprintf(stderr, "Error opening named pipe for reading: %s\n", strerror(errno));
            return -1;
        }
    } else {
        if ((fd = open(name, O_WRONLY | O_NONBLOCK)) == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                fprintf(stderr, "Error opening named pipe for writing: %s\n", strerror(errno));
                return -1;
            }
        }
    }
    
    return fd;
}

/**
 * Writes data to a named pipe with the given file descriptor.
 *
 * @param fd The file descriptor of the named pipe.
 * @param data A pointer to the data to be written.
 * @param datalen The length of the data to be written.
 * @return The number of bytes written to the named pipe, or -1 on error.
 * @throws If an error occurs while writing to the named pipe, an appropriate error message will be printed to stderr.
 */
int write_to_pipe(int fd, char *data, size_t datalen) {
    int bytes_written = write(fd, data, datalen);
    if (bytes_written == -1) {
        fprintf(stderr, "Error writing to named pipe: %s\n", strerror(errno));
        return -1;
    }
    return bytes_written;
}

/**
 * Reads data from a named pipe with the given file descriptor, waiting up to `timeout` seconds for data to arrive.
 *
 * @param fd The file descriptor of the named pipe.
 * @param buf A buffer to store the data read from the named pipe.
 * @param timeout The maximum number of seconds to wait for data to arrive.
 * @return The number of bytes read from the named pipe, or -1 on error.
 * @throws If an error occurs while reading from the named pipe, an appropriate error message will be printed to stderr.
 */
int read_from_pipe(int fd, char* buf, double timeout) {
    int bytes_read, retval;
    double i, f;
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    // parse the timeout
    f = modf(timeout, &i);

    tv.tv_sec = (int)i;
    tv.tv_usec = (int)(f * 1000000);

    retval = select(fd + 1, &rfds, NULL, NULL, &tv);

    if (retval == -1) {
        fprintf(stderr, "Error in select(): %s\n", strerror(errno));
        return -1;
    }
    else if (retval == 0) {
        fprintf(stderr, "Timeout waiting for data on named pipe\n");
        return -1;
    }
    else {
        if ((bytes_read = read(fd, buf, BLOCK_SIZE)) == -1) {
            fprintf(stderr, "Error reading from named pipe: %s\n", strerror(errno));
            return -1;
        }
        return bytes_read;
    }
}

/**
 * Writes data to a named pipe with the given name, waiting up to `timeout` seconds for the pipe to become available.
 *
 * @param pipe_name The name of the named pipe to write to.
 * @param buf A pointer to the data to be written.
 * @param buflen The length of the data to be written.
 * @param timeout The maximum number of seconds to wait for the named pipe to become available.
 * @return The number of bytes written to the named pipe, or -1 on error.
 * @throws If an error occurs while opening the named pipe, waiting for the pipe to become available, or writing to the named pipe, an appropriate error message will be printed to stderr.
 */
int send_data(const char *pipe_name, char *buf, int buflen, double timeout) {
    int fd, bytes_written;
    int timeout_ms = (int)(timeout * 1000);
    // wait until the pipe is opened
    while ((timeout_ms) > 0) {
        fd = open_pipe(pipe_name, false);
        if (fd >= 0) {
            break;
        }
        // sleep for 10 ms
        timeout_ms -= 10;
        usleep(10000); 
    }
    if (fd < 0) {
        fprintf(stderr, "Timeout waiting for pipe to open\n");
        return -1;
    }

    bytes_written = write_to_pipe(fd, buf, buflen);
    close(fd);

    return bytes_written;
}

/**
 * Reads data from a named pipe with the given name, waiting up to `timeout` seconds for data to be available.
 *
 * @param pipe_name The name of the named pipe to read from.
 * @param buf A pointer to the buffer where the data will be stored.
 * @param timeout The maximum number of seconds to wait for data to be available on the named pipe.
 * @return The number of bytes read from the named pipe, or -1 on error.
 * @throws If an error occurs while opening the named pipe, waiting for data to be available on the pipe, or reading from the named pipe, an appropriate error message will be printed to stderr.
 */
int read_data(const char *pipe_name, char *buf, double timeout) {
    int fd, bytes_read;
    fd = open_pipe(pipe_name, true);
    if (fd == -1) {
        return -1;
    }
    bytes_read = read_from_pipe(fd, buf, timeout);
    close(fd);

    return bytes_read;
}
