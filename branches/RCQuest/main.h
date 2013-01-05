#ifndef _CRUISE_H_
#define _CRUISE_H_

using namespace std;

#include <fstream>
#include <math.h>
#include <windows.h>
#include <time.h>

#include "Cinsim.h"
#include "RCQuest.h"

/*** GLOBAL ***/
char IS_PRODUCT_NAME[16];


time_t  stime;

char RootDir[MAX_PATH];
char ServiceName[64];
pthread_mutex_t RCmutex;                // Mutex var used for send_packet() method

class GlobalInfo
{
	public:
    /** IS_STA **/
    /** IS_RST **/
    char    Track[6];                          // Current track (for streets)
    char    Product[6];                        // DEMO or S1 or S2
    /** insim **/
    char    IP[16];                            // IP port for connect (for InSim IS_III)
    word    TCPPORT;                           // TCP port (for InSim IS_III)
    word    UDPPORT;                           // UDP port for mci packets (for InSim IS_III)
    char    ADMIN[10];                         // Admin pass (for InSim IS_III)
};



#endif
