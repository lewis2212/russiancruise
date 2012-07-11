#ifndef _ISRELAY_H
#define _ISRELAY_H

typedef unsigned char byte;
typedef unsigned short word;



#include "insim.h"
#include "relay.h"
#include <winsock2.h>
//#include <iostream>

#define PACKET_BUFFER_SIZE 512
#define PACKET_MAX_SIZE 512
#define IS_TIMEOUT 5

class IsRelay
{
  private:
    unsigned char packet[PACKET_MAX_SIZE];           // A buffer where the current packet is stored
    fd_set readfd, exceptfd;                // File descriptor watches
    struct timeval select_timeout;          // timeval struct for the select() call


  public:
    IsRelay::IsRelay();                 // Constructor
    SOCKET sock;                        // TCP Socket (most packets)
    byte Admin;                         // Admin on server
    int packet_size;
    int isclose();                      // Closes connection from insim and from the socket
    int next_packet();                  // Gets next packet ready into "char packet[]"
    char peek_packet();                 // Returns the type of the current packet
    char type_packet();                 // Returns the type of the current packet
    char get_reqi();
    void* get_packet();                 // Returns a pointer to the current packet. Must be casted
    int send_packet(void* packet);      // Sends a packet to the host
    int send_packet2(void* s_packet,int s_size);

};

#endif
