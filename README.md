IPC-demo
========

Linux Inter Process Communication (IPC)

A collection of examples that demostrate different IPC mechanisms in Linux.
All the examples rely on running two processes (A and B). Surprisingly some of these communication mechanisms
do not require A and B to run at the same time.

## How to run

A makefile is available for every example.
You will be running 2 processes so it may require you to run process A and process B in two different TTY.

Generally, I should be simple as `make`, then `./a.out` (on terminal 1) and `./b.out` (on terminal 2).
Occasionally, a sigle executable is available, in which case the original process will fork. 
To remove all the builds `make clean`

## Contents
### Signals
Signals are one of the primitive type of IPC. They should be handle with care, because they are able to interrupt
the process at any time. Therefore readings (other than sig\_atomic\_t) may have inconsistent data.

`signal` is considered obsolete than I used `sigaction` (POSIX)

###### Reference
* man 7 signal
* man 2 sigaction

### Pipe
An half-duplex communication channel (unidirectional). Pipes are very easy to use and the `pipe` system call
generate the two file descriptors needed for the communication. These descriptors cannot be accessed by other processes, 
then you can only fork the process and inherit the file descriptors.

To use pipe we usually need to follow this steps:
1. Call the pipe system call and create the two file descriptors.
2. Fork the process
3. Close the file descriptor that won't be used

###### Reference
* man 2 pipe

### Named Pipe
These pipes have names and are represented in the filesystem by special files (FIFO). The advantage of using named pipe
is quite obvious, processes do not need to belong to the same hierarchy.
Named pipes are files and they may persist after process termination. It is required that a reader process and a writer process
open the pipe otherwise read/write operations can be blocking or returning error.

###### Command line
mkfifo - create a named pipe

###### Reference
* man 2 mknod

### Shared memory
The kernel allows processes to share an address space.
It is up to the processes attaching this shared location to synchronize to avoid conflict.

###### Command line
* ipcs  - list all System V IPC object in the system
* ipcrm - remove IPC object

###### Reference
* man 2 shmget
* man 2 shmat
* man 2 shmctl

### Message queue
A kernel managed queue that stores labeled messages.
Messages can be retrieve from the oldest to newest and filter by label. 

###### Command line
* ipcs  - list all System V IPC object in the system
* ipcrm - remove IPC object

###### Reference
* man 2 msgget
* man 2 msgsnd
* man 2 msgrcv
* man 2 shmctl

### Semaphores Set
Semaphore set can hold multiple semaphores that are used to as synchornizing or signaling mechanism.
Operation on semaphore are atomic and consist of descrementing or incrementing the semaphore value.
* If the operation wants to decrement the value of a semaphore for a quantity larger than the current semaphore value, the process may block until the value of the semaphore is at least equal to the quantity requested.
* If the operation decrement the value of the semaphore for a quantity equal at 0, the process may block until the value of the semaphore is 0.

Semaphore can be used as mutex:
A process descrement the semaphore before entering a critical section and restore its value when exiting.

Semaphore can be use as counter:
A process may set a semaphore an wait for other processes to consume the counter. 

###### Command line
* ipcs  - list all System V IPC object in the system
* ipcrm - remove IPC object

###### Reference
* man 2 semget
* man 2 semop
* man 2 semctl

### File mapping
File mapping is very similar to shared memory, it can be used by processes to map a file into their memory,
any changes are visible to other processes (MAP\_SHARED). An advisory locking mechanism is available for files
through fcntl. Advisory means that process need to cooperate and request the locks before operating on the shared file.

###### Reference
* man 2 mmap
* man 2 fcntl

### Sockets
Sockets can be used for IPC. Linux provides domain sockets for _Stream_ and _Datagram_.
Although these socket are local to the same system, the underlying implementation is abstracted by the Socket API, then
they can be used as any other socket of the corresponding type.

###### Command line
* netstat

###### Reference
* man 2 socket
* man 2 bind
* man 2 listen
* man 2 connect
* man 2 sent
* man 2 recv

## License
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
