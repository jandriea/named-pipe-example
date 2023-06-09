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

#ifndef PIPE_HANDLER_H
#define PIPE_HANDLER_H

#include <stdbool.h>

#define PIPE_SET_NAME "/tmp/my_pipe_set"
#define PIPE_GET_NAME "/tmp/my_pipe_get"

#define BLOCK_SIZE 4096

int open_pipe(const char *name, bool isRead);
int write_to_pipe(int fd, char *data, size_t datalen);
int read_from_pipe(int fd, char* buf, double timeout);
int send_data(const char *pipe_name, char *buf, int buflen, double timeout);
int read_data(const char *pipe_name, char *buf, double timeout);

#endif
