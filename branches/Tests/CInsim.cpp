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

using namespace std;

#include "CInsim.h"

#ifdef CIS_LINUX
#define INVALID_SOCKET -1
#endif

/**
* Constructor: Initialize the buffers
*/
CInsim::CInsim ()
{
    // Initialize the mutex var
    pthread_mutex_init(&ismutex, NULL);

    // Initialize global buffers
    memset(gbuf.buffer, 0, PACKET_BUFFER_SIZE);
    gbuf.bytes = 0;

    // Initialize local buffers
    memset(lbuf.buffer, 0, PACKET_BUFFER_SIZE);
    memset(udp_lbuf.buffer, 0, PACKET_BUFFER_SIZE);
    lbuf.bytes = 0;
    udp_lbuf.bytes = 0;

    // Initialize packet buffers
    memset(packet, 0, PACKET_MAX_SIZE);
    memset(udp_packet, 0, PACKET_MAX_SIZE);

    // By default we're not using UDP
    using_udp = 0;

}


/**
* Destructor: Initialize the buffers
*/
CInsim::~CInsim ()
{
    // Destroy the mutex var
    pthread_mutex_destroy(&ismutex);
}


/**
* Initialize the socket and the Insim connection
* If "struct IS_VER *pack_ver" is set it will contain an IS_VER packet after returning. It's an optional argument
*/
int CInsim::init (char *addr, word port, char *product, char *admin, struct IS_VER *pack_ver, byte prefix, word flags, word interval, word udpport)
{
    // Initialise WinSock
    // Only required on Windows
#ifdef CIS_WINDOWS
    WSADATA wsadata;

    if (WSAStartup(0x202, &wsadata) == SOCKET_ERROR)
    {
        WSACleanup();
        return -1;
    }
#endif

    // Create the TCP socket - this defines the type of socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // Could we get the socket handle? If not the OS might be too busy or have run out of available socket descriptors
    if (sock == INVALID_SOCKET)
    {
#ifdef CIS_WINDOWS
        closesocket(sock);
        WSACleanup();
#endif
#ifdef CIS_LINUX
        close(sock);
#endif
        return -1;
    }

    // Resolve the IP address
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));

    saddr.sin_family = AF_INET;

    struct hostent *hp;
    hp = gethostbyname(addr);

    if (hp != NULL)
        saddr.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
    else
        saddr.sin_addr.s_addr = inet_addr(addr);

    // Set the port number in the socket structure - we convert it from host byte order, to network
    saddr.sin_port = htons(port);

    // Now the socket address structure is full, lets try to connect
    if (connect(sock, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
    {
#ifdef CIS_WINDOWS
        closesocket(sock);
        WSACleanup();
#endif
#ifdef CIS_LINUX
        close(sock);
#endif
        return -1;
    }

    // If the user asked for NLP or MCI packets and defined an udpport
    if ((udpport) && (flags >= ISF_NLP))
    {
        // Create the UDP socket - this defines the type of socket
        sockudp = socket(AF_INET, SOCK_DGRAM, 0);

        // Could we get the socket handle? If not the OS might be too busy or have run out of available socket descriptors
        if (sockudp == INVALID_SOCKET)
        {
#ifdef CIS_WINDOWS
            closesocket(sock);
            closesocket(sockudp);
            WSACleanup();
#endif
#ifdef CIS_LINUX
            close(sock);
            close(sockudp);
#endif
            return -1;
        }

        // Resolve the IP address
        struct sockaddr_in udp_saddr, my_addr;
        memset(&udp_saddr, 0, sizeof(udp_saddr));
        memset(&my_addr, 0, sizeof(my_addr));

        // Bind the UDP socket to my specified udpport and address
        my_addr.sin_family = AF_INET;         // host byte order
        my_addr.sin_port = htons(udpport);     // short, network byte order
        my_addr.sin_addr.s_addr = INADDR_ANY;
        memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

        // don't forget your error checking for bind():
        bind(sockudp, (struct sockaddr *)&my_addr, sizeof my_addr);

        // Set the server address and the connect to it
        udp_saddr.sin_family = AF_INET;

        struct hostent *udp_hp;
        udp_hp = gethostbyname(addr);

        if (udp_hp != NULL)
            udp_saddr.sin_addr.s_addr = *((unsigned long*)udp_hp->h_addr);
        else
            udp_saddr.sin_addr.s_addr = inet_addr(addr);

        // Set the UDP port number in the UDP socket structure - we convert it from host byte order, to network
        udp_saddr.sin_port = htons(port);

        // Connect the UDP using the same address as in the TCP socket
        if (connect(sockudp, (struct sockaddr *) &udp_saddr, sizeof(udp_saddr)) < 0)
        {
#ifdef CIS_WINDOWS
            closesocket(sock);
            closesocket(sockudp);
            WSACleanup();
#endif
#ifdef CIS_LINUX
            close(sock);
            close(sockudp);
#endif
            return -1;
        }

        // We are using UDP!
        using_udp = 1;
    }

    // Ok, so we're connected. First we need to let LFS know we're here by sending the IS_ISI packet
    struct IS_ISI isi_p;
    memset(&isi_p, 0, sizeof(struct IS_ISI));
    isi_p.Size = sizeof(struct IS_ISI);
    isi_p.Type = ISP_ISI;

    if (pack_ver != NULL)             // We request an ISP_VER if the caller asks for it
        isi_p.ReqI = 1;

    isi_p.Prefix = prefix;
    isi_p.UDPPort = udpport;
    isi_p.Flags = flags;
    isi_p.Interval = interval;
    memcpy(isi_p.IName, product, sizeof(isi_p.IName)-1);
    memcpy(isi_p.Admin, admin, 16);

    // Send the initialization packet
    if(!send_packet(&isi_p))
    {
        if (using_udp)
        {
#ifdef CIS_WINDOWS
            closesocket(sockudp);
#endif
#ifdef CIS_LINUX
            close(sockudp);
#endif
        }

#ifdef CIS_WINDOWS
        closesocket(sock);
        WSACleanup();
#endif
#ifdef CIS_LINUX
        close(sock);
#endif
        return -1;
    }

    // Set the timeout period
    select_timeout.tv_sec = IS_TIMEOUT;
    select_timeout.tv_usec = 0;

    // If an IS_VER packet was requested
    if (pack_ver != NULL)
    {
        if (next_packet() < 0)              // Get next packet, supposed to be an IS_VER
        {
            if (isclose() < 0)
            {
                if (using_udp)
                {
#ifdef CIS_WINDOWS
                    closesocket(sockudp);
#endif
#ifdef CIS_LINUX
                    close(sockudp);
#endif
                }

#ifdef CIS_WINDOWS
                closesocket(sock);
                WSACleanup();
#endif
#ifdef CIS_LINUX
                close(sock);
#endif
                return -1;
            }
            return -1;
        }

        switch (peek_packet())              // Check if the packet returned was an IS_VER
        {
        case ISP_VER:                     // It was, get it!
            memcpy(pack_ver, (struct IS_VER*)get_packet(), sizeof(struct IS_VER));
            INSIM_VERSION = pack_ver->InSimVer;
            break;
        default:                          // It wasn't, something went wrong. Quit
            if (isclose() < 0)
            {
                if (using_udp)
                {
#ifdef CIS_WINDOWS
                    closesocket(sockudp);
#endif
#ifdef CIS_LINUX
                    close(sockudp);
#endif
                }

#ifdef CIS_WINDOWS
                closesocket(sock);
                WSACleanup();
#endif
#ifdef CIS_LINUX
                close(sock);
#endif
            }
            return -1;
        }
    }
    return 0;
}

/**
* Close connection to InSim
*/
int CInsim::isclose()
{
    struct IS_TINY cl_packet;
    cl_packet.ReqI = 0;
    cl_packet.SubT = TINY_CLOSE;
    if (!send_packet(&cl_packet))
        return -1;

    if (using_udp)
    {
#ifdef CIS_WINDOWS
        closesocket(sockudp);
#endif
#ifdef CIS_LINUX
        close(sockudp);
#endif
    }

#ifdef CIS_WINDOWS
    closesocket(sock);
    WSACleanup();
#endif
#ifdef CIS_LINUX
    close(sock);
#endif
    return 0;
}

/**
* Get next packet ready
* This function also keeps the connection alive as long as you keep calling it
*/
int CInsim::next_packet()
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

#ifdef CIS_WINDOWS
            int rc = select(0, &readfd, NULL, &exceptfd, &select_timeout);
#endif
#ifdef CIS_LINUX
            int rc = select(sock + 1, &readfd, NULL, &exceptfd, &select_timeout);
#endif

            if (rc == 0)                    // Timeout
                continue;

            if (rc < 0)                     // An error occured
                return -1;

            if (FD_ISSET(sock, &readfd))   // We got data!
            {
                // Recieve any waiting bytes
                int retval = recv(sock, lbuf.buffer + lbuf.bytes, PACKET_BUFFER_SIZE - lbuf.bytes, 0);

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
            if (!send_packet(&keepalive))
                return -1;
        }
    }

    return 0;
}

/**
* Return the type of the next packet
*/
char CInsim::peek_packet()
{
    return *(packet+1);
}

/**
* Return the contents of the next packet
*/
void* CInsim::get_packet()
{
    if (peek_packet() == ISP_NONE)
        return NULL;

    return packet;
}


/**
* Get next UDP packet ready
*/
int CInsim::udp_next_packet()
{
    // Clear the local buffer
    memset(udp_lbuf.buffer, 0, PACKET_BUFFER_SIZE);
    udp_lbuf.bytes = 0;

    // Read until we have a full packet
    while (udp_lbuf.bytes < 1)
    {
        // Clear them
        FD_ZERO(&udp_readfd);
        FD_ZERO(&udp_exceptfd);

        // Set them to watch our socket for data to read and exceptions that maybe thrown
        FD_SET(sockudp, &udp_readfd);
        FD_SET(sockudp, &udp_exceptfd);

#ifdef CIS_WINDOWS
        int rc = select(0, &udp_readfd, NULL, &udp_exceptfd, &select_timeout);
#endif
#ifdef CIS_LINUX
        int rc = select(sockudp + 1, &udp_readfd, NULL, &udp_exceptfd, &select_timeout);
#endif

        if (rc == 0)                    // Timeout
            continue;

        if (rc < 0)                     // An error occured
            return -1;

        if (FD_ISSET(sockudp, &udp_readfd))   // We got data!
        {
            // Recieve any waiting bytes
            int retval = recv(sockudp, udp_lbuf.buffer + udp_lbuf.bytes, PACKET_BUFFER_SIZE - udp_lbuf.bytes, 0);

            // Deal with the results
            if (retval < 0)                 // An error ocurred
                return -1;

            udp_lbuf.bytes += retval;
        }
        else if (FD_ISSET(sockudp, &udp_exceptfd))    // An exception occured - we want to quit
            return -1;
    }

    memcpy(udp_packet, udp_lbuf.buffer, (unsigned char)*udp_lbuf.buffer);

    return 0;
}


/**
* Return the type of the next UDP packet
*/
char CInsim::udp_peek_packet()
{
    return *(udp_packet+1);
}


/**
* Return the contents of the next UDP packet
*/
void* CInsim::udp_get_packet()
{
    return udp_packet;
}

/**
* Send a packet
*/
bool CInsim::send_packet(void* s_packet)
{
    //if (INSIM_VERSION == 5)
    //{
    byte Type = *((unsigned char*)s_packet+1);
    switch(Type)
    {
    case ISP_BTN:
        return send_button(s_packet);
        break;

    case ISP_MTC:
        return send_mtc(s_packet);
        break;
    }
    //}

    pthread_mutex_lock (&ismutex);
    if (send(sock, (const char *)s_packet, *((unsigned char*)s_packet), 0) < 0)
    {
        pthread_mutex_unlock (&ismutex);
        return false;
    }
    pthread_mutex_unlock (&ismutex);
    return true;
}


/**
* Send a variable sized button
*/
bool CInsim::send_button(void* s_button)
{
    struct IS_BTN *pack_btn = (struct IS_BTN*)s_button;

    int text_len = strlen(pack_btn->Text);
    int text2send;

    if (text_len == 0)
        text2send = 0;
    else
        text2send = (1+((text_len-1)/4))*4;

    pack_btn->Size = 12 + text2send;

    pthread_mutex_lock (&ismutex);
    if (send(sock, (const char *)pack_btn, pack_btn->Size, 0) < 0)
    {
        pthread_mutex_unlock (&ismutex);
        return false;
    }
    pthread_mutex_unlock (&ismutex);
    return true;
}

/**
* Send a variable sized message to connect
*/
bool CInsim::send_mtc(void* s_mtc)
{

    struct IS_MTC *pack_mtc = (struct IS_MTC*)s_mtc;
    int text_len = strlen(pack_mtc->Text);
    int text2send;

    if (text_len == 0)
        return false;
    else if (text_len > 127)
        return false;

    text2send = text_len + 4 - text_len%4;
    pack_mtc->Size = 8 + text2send;

    pthread_mutex_lock (&ismutex);
    if (send(sock, (const char *)s_mtc,*((unsigned char*)s_mtc), 0) < 0)
    {
        pthread_mutex_unlock (&ismutex);
        return false;
    }
    pthread_mutex_unlock (&ismutex);
    return true;
}

/**
* Other functions!!!
*/


/**
* Converts miliseconds to a C string
* 14 characters needed in str to not run into buffer overflow ("-hh:mm:ss.xxx\0")
* @param    milisecs    Miliseconds to convert
* @param    str         String to be filled with the result in format "-hh:mm:ss.xxx"
* @param    thousands   Result shows: 0 = result hundreths of second; other = thousandths of second
*/
char* ms2str (long milisecs, char *str, int thousands)
{
    unsigned hours = 0;
    unsigned minutes = 0;
    unsigned seconds = 0;
    unsigned hundthou = 0;

    char shours[3], sminutes[3], sseconds[3], shundthou[4];

    memset(str, 0, 14);

    if (milisecs < 0)
    {
        strcpy(str,"-");
        milisecs *= -1;
    }

    if (milisecs >= 360000000)
        return 0;

    if (milisecs >= 3600000)
    {
        hours = milisecs / 3600000;
        milisecs %= 3600000;
    }
    if (milisecs >= 60000)
    {
        minutes = milisecs / 60000;
        milisecs %= 60000;
    }
    if (milisecs >= 1000)
    {
        seconds = milisecs / 1000;
        milisecs %= 1000;
    }
    if (thousands)
        hundthou = milisecs;
    else
        hundthou = milisecs / 10;

    if (hundthou == 0)
    {
        if (thousands)
            strcpy(shundthou, "000");
        else
            strcpy(shundthou, "00");
    }

    if (hours > 0)
    {
        sprintf(shours,"%d",hours);
        strcat(strcat(str,shours),":");

        if (minutes > 9)
            sprintf(sminutes,"%d",minutes);
        else
        {
            strcpy(sminutes,"0");
            sprintf(sminutes+1,"%d",minutes);
        }
        strcat(strcat(str,sminutes),":");

        if (seconds > 9)
            sprintf(sseconds,"%d",seconds);
        else
        {
            strcpy(sseconds,"0");
            sprintf(sseconds+1,"%d",seconds);
        }
        strcat(strcat(str,sseconds),".");

        if (thousands)
        {
            if (hundthou > 99)
                sprintf(shundthou,"%d",hundthou);
            else if (hundthou > 9)
            {
                strcpy(shundthou,"0");
                sprintf(shundthou+1,"%d",hundthou);
            }
            else
            {
                strcpy(shundthou,"00");
                sprintf(shundthou+2,"%d",hundthou);
            }
        }
        else
        {
            if (hundthou > 9)
                sprintf(shundthou,"%d",hundthou);
            else
            {
                strcpy(shundthou,"0");
                sprintf(shundthou+1,"%d",hundthou);
            }
        }
        strcat(str,shundthou);

    }
    else if (minutes > 0)
    {
        sprintf(sminutes,"%d",minutes);
        strcat(strcat(str,sminutes),":");

        if (seconds > 9)
            sprintf(sseconds,"%d",seconds);
        else
        {
            strcpy(sseconds,"0");
            sprintf(sseconds+1,"%d",seconds);
        }
        strcat(strcat(str,sseconds),".");

        if (thousands)
        {
            if (hundthou > 99)
                sprintf(shundthou,"%d",hundthou);
            else if (hundthou > 9)
            {
                strcpy(shundthou,"0");
                sprintf(shundthou+1,"%d",hundthou);
            }
            else
            {
                strcpy(shundthou,"00");
                sprintf(shundthou+2,"%d",hundthou);
            }
        }
        else
        {
            if (hundthou > 9)
                sprintf(shundthou,"%d",hundthou);
            else
            {
                strcpy(shundthou,"0");
                sprintf(shundthou+1,"%d",hundthou);
            }
        }
        strcat(str,shundthou);
    }
    else if (seconds > 0)
    {
        sprintf(sseconds,"%d",seconds);
        strcat(strcat(str,sseconds),".");

        if (thousands)
        {
            if (hundthou > 99)
                sprintf(shundthou,"%d",hundthou);
            else if (hundthou > 9)
            {
                strcpy(shundthou,"0");
                sprintf(shundthou+1,"%d",hundthou);
            }
            else
            {
                strcpy(shundthou,"00");
                sprintf(shundthou+2,"%d",hundthou);
            }
        }
        else
        {
            if (hundthou > 9)
                sprintf(shundthou,"%d",hundthou);
            else
            {
                strcpy(shundthou,"0");
                sprintf(shundthou+1,"%d",hundthou);
            }
        }
        strcat(str,shundthou);
    }
    else
    {
        strcat(str,"0.");

        if (thousands)
        {
            if (hundthou > 99)
                sprintf(shundthou,"%d",hundthou);
            else if (hundthou > 9)
            {
                strcpy(shundthou,"0");
                sprintf(shundthou+1,"%d",hundthou);
            }
            else
            {
                strcpy(shundthou,"00");
                sprintf(shundthou+2,"%d",hundthou);
            }
        }
        else
        {
            if (hundthou > 9)
                sprintf(shundthou,"%d",hundthou);
            else
            {
                strcpy(shundthou,"0");
                sprintf(shundthou+1,"%d",hundthou);
            }
        }
        strcat(str,shundthou);
    }

    return str;
}

//HACK: This should be considered a hack and needs more proper workaround.
/*#ifdef CIS_LINUX
void itoa(int value, char *buffer, int base)
{
	//  Implement simple itoa behavior, when base it set to 10, resulting
	//+ string should have a minus sign when integer value is negative.
	//+ Otherwise treat every string as unsigned.
	if (base!=10 && value < 0) value = value * (-1);
	sprintf(buffer, "%d", value);
}
#endif*/
