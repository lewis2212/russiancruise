/*
 * Copyright (c) 2010, Cristóbal Marco
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * CInsim v0.6
 * ===========
 *
 * CInsim is a LFS InSim library written in basic C/C++. It provides basic
 * functionality to interact with InSim in Windows and *NIX. It uses WinSock2 for
 * the socket communication under Windows, and pthreads-w32 for thread safe sending
 * method under Windows.
 *
 * *NIX style sockets and POSIX compliant threads are used for *NIX compatibility.
 * *NIX compatibility code provided by MadCat.
 */


#ifndef _CINSIM_H
#define _CINSIM_H

#define MAX_PLAYERS 32


typedef unsigned char byte;
typedef unsigned short word;

// Custom InSim specific database types
typedef struct NodeLap NodeLap;
typedef struct CompCar CompCar;
typedef struct
{
    int x;
    int y;
    int z;
} Vec;
typedef struct
{
    float x;
    float y;
    float z;
} Vector;


/* Defines whether the Windows or Linux part of the source will be compiled.
 * Options are CIS_WINDOWS or CIS_LINUX
 */
#define CIS_WINDOWS


#include "insim.h"

#include <iostream>

// Includes for Windows (uses winsock2)
#ifdef CIS_WINDOWS
#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include "pthread.h"
#endif

// Includes for *NIX (no winsock2, these headers are needed instead)
#ifdef CIS_LINUX
#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif


#define PACKET_BUFFER_SIZE 512
#define PACKET_MAX_SIZE 512
#define IS_TIMEOUT 5


// Definition for our buffer datatype
struct buffer
{
    char buffer[PACKET_BUFFER_SIZE];    // Packet buffer - 512 should be more than enough
    unsigned int bytes;                 // Number of bytes currently in buffer
};

/**
* CInsim class to manage the Insim connection and processing of the packets
*/
class CInsim
{
private:
#ifdef CIS_WINDOWS
    SOCKET sock;                            // TCP Socket (most packets)
    SOCKET sockudp;                         // UDP Socket (if requested, for NLP and MCI)
#endif
#ifdef CIS_LINUX
    int sock;                               // TCP Socket (most packets)
    int sockudp;                            // UDP Socket (if requested, for NLP and MCI)
#endif
    byte using_udp;                         // 1 if we are using UDP for NLP or MCI packets
    struct buffer gbuf;                     // Our global buffer
    struct buffer lbuf;                     // Our local buffer
    char packet[PACKET_MAX_SIZE];           // A buffer where the current packet is stored
    fd_set readfd, exceptfd;                // File descriptor watches
    struct timeval select_timeout;          // timeval struct for the select() call
    struct buffer udp_lbuf;                 // (for NLP and MCI packets via UDP) Our local buffer (no global buffer needed for UDP)
    char udp_packet[PACKET_MAX_SIZE];       // (for NLP and MCI packets via UDP) A buffer where the current packet is stored
    fd_set udp_readfd, udp_exceptfd;        // (for NLP and MCI packets via UDP) File descriptor watches
    pthread_mutex_t ismutex;                // Mutex var used for send_packet() method

public:
    CInsim();                               // Constructor
    ~CInsim();                              // Destructor

    //  "int init(...)" Establishes connection with the socket and insim.
    //+ The last argument ch_ver is a pointer to a IS_VER struct. If it's used an IS_VER packet
    //+ will be returned. If ch_ver is not used in the call no IS_VER will be requested/returned.
    int init (char *addr, word port, char *product, char *admin, struct IS_VER *pack_ver = NULL, byte prefix = 0, word flags = 0, word interval = 0, word udpport = 0);
    int isclose();                      // Closes connection from insim and from the socket
    int next_packet();                  // Gets next packet ready into "char packet[]"
    char peek_packet();                 // Returns the type of the current packet
    void* get_packet();                 // Returns a pointer to the current packet. Must be casted
    /** Send packet without error protection **/
    bool send_packet(void* packet);      // Sends a packet to the host
    bool send_button(void* button);      // Sends a variable sized button to the host
    bool send_mtc(void* s_mtc); //  Send a variable sized message to connect
    /** Send packet with error protection **/
    bool send_packet(void* packet, char *errmsg);      // Sends a packet to the host
    bool send_button(void* button, char *errmsg);      // Sends a variable sized button to the host
    bool send_mtc(void* s_mtc, char *errmsg); //  Send a variable sized message to connect
    int udp_next_packet();              // (UDP) Gets next packet ready into "char udp_packet[]"
    char udp_peek_packet();             // (UDP) Returns the type of the current packet
    void* udp_get_packet();             // (UDP) Returns a pointer to the current packet. Must be casted
};


/**
* Other functions!!!
*/

char* ms2str (long milisecs, char *str, int thousands=0); // Converts miliseconds to a C string (including negative miliseconds).
/*#ifdef CIS_LINUX
void itoa(int value, char *str, int base);	//itoa() does not necessarily have to be in stdlib.h available on Linux systems, so I we have to rehack it ourselves:)
#endif*/
#endif
