#ifndef _CINSIM_H
#define _CINSIM_H

// Custom InSim specific database types
typedef unsigned char byte;
typedef unsigned short word;
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

#include "insim.h"
#include <winsock2.h>

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
    SOCKET sock;                            // TCP Socket (most packets)
    SOCKET sockudp;                         // UDP Socket (if requested, for NLP and MCI)
    byte using_udp;                         // 1 if we are using UDP for NLP or MCI packets
    struct buffer gbuf;                     // Our global buffer
    struct buffer lbuf;                     // Our local buffer
    char packet[PACKET_MAX_SIZE];           // A buffer where the current packet is stored
    fd_set readfd, exceptfd;                // File descriptor watches
    struct timeval select_timeout;          // timeval struct for the select() call
    struct buffer udp_lbuf;                 // (for NLP and MCI packets via UDP) Our local buffer (no global buffer needed for UDP)
    char udp_packet[PACKET_MAX_SIZE];       // (for NLP and MCI packets via UDP) A buffer where the current packet is stored
    fd_set udp_readfd, udp_exceptfd;        // (for NLP and MCI packets via UDP) File descriptor watches

  public:
    CInsim::CInsim();                   // Constructor
        //  "int init(...)" Establishes connection with the socket and insim.
        //+ The last argument ch_ver is a pointer to a IS_VER struct. If it's used an IS_VER packet
        //+ will be returned. If ch_ver is not used in the call no IS_VER will be requested/returned.
    int init (char *addr, word port, char *product, char *admin, struct IS_VER *pack_ver = NULL, byte prefix = 0, word flags = 0, word interval = 0, word udpport = 0);
    int isclose();                      // Closes connection from insim and from the socket
    int next_packet();                  // Gets next packet ready into "char packet[]"
    char peek_packet();                 // Returns the type of the current packet
    void* get_packet();                 // Returns a pointer to the current packet. Must be casted
    int send_packet(void* packet);      // Sends a packet to the host
    int send_text_packet(const char* s_packet);
    int udp_next_packet();              // (UDP) Gets next packet ready into "char udp_packet[]"
    char udp_peek_packet();             // (UDP) Returns the type of the current packet
    void* udp_get_packet();             // (UDP) Returns a pointer to the current packet. Must be casted
};


#endif
