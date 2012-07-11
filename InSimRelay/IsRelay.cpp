#include "IsRelay.h"

IsRelay::IsRelay ()
{
    // Initialize packet buffers
    memset(packet, 0, PACKET_MAX_SIZE);

}

/**
* Close connection to InSim
*/
int IsRelay::isclose()
{
    closesocket(sock);
    WSACleanup();
    return 0;
}

/**
* Get next packet ready
* This function also keeps the connection alive as long as you keep calling it
*/
/*
int IsRelay::next_packet()
{
    char size[1];
    char buffer[PACKET_MAX_SIZE];

    int retval = recv(sock, size,1, 0);
    // Deal with the results
    if (retval == 0)                // Connection has been closed at the other end
        return -2;

    if (retval < 0)                 // An error ocurred
        return -1;

    packet_size = int(size[0]);
    recv(sock, buffer,int(size[0])-1, 0);

    memcpy(packet,buffer,int(size[0])-1);

    if ((peek_packet() == ISP_TINY) && (*(packet+2) == TINY_NONE))
        {

            struct IS_TINY keepalive;
            keepalive.Size = sizeof(struct IS_TINY);
            keepalive.Type = ISP_TINY;
            keepalive.ReqI = 0;
            keepalive.SubT = TINY_NONE;

            // Send it back
            if (send_packet(&keepalive) < 0)
                return -1;
        }

    return 0;
}*/

int IsRelay::next_packet()
{

    char *buf = (char*)packet;

    while (true)
    {
        int retval = recv(sock, buf, 1, 0);
        if (retval <= 0)
            return -1;

           // std::cout << "Packet Size = " << (int)*buf << std::endl;
            packet_size = (int)*buf;

        retval = recv(sock, buf+1, (*buf)-1, 0);
        if (retval <= 0)
            return -1;

           // std::cout << "Get all packet" << std::endl;


        if ((peek_packet() == ISP_TINY) && (*(packet+2) == TINY_NONE))
        {
            //std::cout << "TINY_NONE" << std::endl;

            struct IS_TINY keepalive;
            keepalive.Size = sizeof(struct IS_TINY);
            keepalive.Type = ISP_TINY;
            keepalive.ReqI = 0;
            keepalive.SubT = TINY_NONE;

            // Send it back
            if (send_packet(&keepalive) < 0)
                return -1;
        }
        else
            break;
    }

    return 0;
}

/**
int IsRelay::next_packet()
{
    unsigned char oldp_size, p_size;
    bool alive = true;

    while (alive)                                               // Keep the connection alive!
    {
        alive = false;
        oldp_size = (unsigned char)*lbuf.buffer;

        if ((lbuf.bytes > 0) && (lbuf.bytes >= oldp_size))         // There's an old packet in the local buffer, skip it
        {
            // Copy the leftovers from local buffer to global buffer
            memcpy(gbuf.buffer, lbuf.buffer+oldp_size, lbuf.bytes-oldp_size);
            gbuf.bytes = lbuf.bytes - oldp_size;

            // Copy from global buffer back to the beginning of local buffer
            memset(lbuf.buffer, 0, PACKET_BUFFER_SIZE);
            memcpy(lbuf.buffer, gbuf.buffer, gbuf.bytes);
            lbuf.bytes = gbuf.bytes;
        }

        p_size = (unsigned char)*lbuf.buffer;

        while ((lbuf.bytes < p_size) || (lbuf.bytes < 1))       // Read until we have a full packet
        {
            // Clear them
            FD_ZERO(&readfd);
            FD_ZERO(&exceptfd);

            // Set them to watch our socket for data to read and exceptions that maybe thrown
            FD_SET(sock, &readfd);
            FD_SET(sock, &exceptfd);

            int rc = select(0, &readfd, NULL, &exceptfd, &select_timeout);

            if (rc == 0)                    // Timeout
                continue;

            if (rc < 0)                     // An error occured
                return -1;

            if (FD_ISSET(sock, &readfd))   // We got data!
            {
                // Recieve any waiting bytes
                int retval = recv(sock, (char*)lbuf.buffer + lbuf.bytes, PACKET_BUFFER_SIZE - lbuf.bytes, 0);

                // Deal with the results
                if (retval == 0)                // Connection has been closed at the other end
                    return -2;

                if (retval < 0)                 // An error ocurred
                    return -1;

                p_size = *lbuf.buffer;
                lbuf.bytes += retval;
            }
            else if (FD_ISSET(sock, &exceptfd))    // An exception occured - we want to quit
                return -1;
        }

        memcpy(packet, lbuf.buffer, p_size);

        if ((peek_packet() == ISP_TINY) && (*(packet+3) == TINY_NONE))
        {
            alive = true;

            struct IS_TINY keepalive;
            keepalive.Size = sizeof(struct IS_TINY);
            keepalive.Type = ISP_TINY;
            keepalive.ReqI = 0;
            keepalive.SubT = TINY_NONE;

            // Send it back
            if (send_packet(&keepalive) < 0)
                return -1;
        }
    }

    return 0;
}
**/
/**
* Return the type of the next packet
*/
char IsRelay::peek_packet()
{
    return *(packet+1);
}

char IsRelay::get_reqi()
{
    return *(packet+2);
}

char IsRelay::type_packet()
{
    return *(packet+3);
}

/**
* Return the contents of the next packet
*/
void* IsRelay::get_packet()
{
    if (peek_packet() == ISP_NONE)
        return NULL;

    return packet;
}

/**
* Send a packet
*/
int IsRelay::send_packet(void* s_packet)
{
    if (send(sock, (const char *)s_packet, *((unsigned char*)s_packet), 0) < 0)
        return -1;

    return 0;
}

int IsRelay::send_packet2(void* s_packet,int s_size)
{
    if (send(sock, (const char *)s_packet, s_size, 0) < 0)
        return -1;

    return 0;
}

