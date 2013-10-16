#ifndef _CRUISE_H_
#define _CRUISE_H_
/**

Russian Cruise by TurboSnail

**/

#define MAX_PLAYERS 32

/**

buttons ClickId's:

1-49  - users
50-59 btn_panel
60- 79 - pogonya users
80-99 - tuning
100 149 - info panel
{
	100 - 102 main bg;
}
150 - 160 - shop

161-189 police

190-202 svetofor

203-205 сирена

210 - pizza clock
211 - pizza action

212,213 - drive lic


*/
using namespace std;

#ifdef __linux__
//	#include <mysql.h>
#else
#include <windows.h>
#include "mysql/include/mysql.h"
#endif


#include <math.h>
#include <time.h>
#include <map>

#include "CInsim.h"
#include "RCButtonClickID.h"

#include "RCPizza.h"
#include "RCMessage.h"
#include "RCEnergy.h"
#include "RCBank.h"
#include "RCDrivingLicense.h"
#include "RCAntCheat.h"
#include "RCStreet.h"
#include "RCLight.h"
#include "RCTaxi.h"
#include "RCPolice.h"
//#include "RCRoadSign.h"
//#include "RCBanList.h"
#include "RCQuest.h"

#define MAX_CARS 30


/*** GLOBAL ***/
char IS_PRODUCT_NAME[16];



// Дескриптор соединения
MYSQL rcMaindb;
// Дескриптор результирующей таблицы
MYSQL_RES *rcMainRes;
// Массив полей текущей строки
MYSQL_ROW rcMainRow;


time_t  s_time;
int     chek_users =0;

CInsim *insim;

ofstream  out;   // выходной файл для протокола работы сервиса

char RootDir[MAX_PATH];
char ServiceName[64];

#ifndef __linux__
SERVICE_STATUS    service_status;
SERVICE_STATUS_HANDLE  hServiceStatus;
SC_HANDLE  hServiceControlManager, hService;

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);
#endif
pthread_mutex_t RCmutex;                // Mutex var used for send_packet() method
pthread_t main_tid; // Thread ID
struct cars
{
    int             id;
    char            car[6];
    unsigned long   cash;
    unsigned long   sell;
    unsigned		PLC;
};

struct user_car
{
    char    car[6];
    int     tuning;
    float     dist;
};



struct track_info
{
    int     PitCount;
    int     *XPit = NULL;
    int     *YPit = NULL;
    int     ShopCount;
    int     *XShop = NULL;
    int     *YShop = NULL;
};

struct player
{
    struct CompCar Info;
    user_car cars[MAX_CARS];
    //map <string, user_car> cars2;

    char    UName[24];             // Username
    char    PName[24];             // Player name
    char    CName[4];              // Car Name
    int     CTune;                 // Car Tuning
    byte    SetF;
    byte    H_TRes;
    byte    UCID;                  // Connection ID
    byte    PLID;                  // Player ID
    byte    BID;
    byte    BID2;
    byte    bfn;
    float   Distance;
    byte    Zone;
    //bool    Pitlane;
    byte    Shop; // NO DELETE!!!!
    int     Action;
    /*** bonus ***/
    float   Bonus_dist;
    int     Bonus_count;
    /******************/
    byte    Svetofor;

    /** Flood **/
    char  	Msg[128];
    int     FloodCount;
    int     FloodTime;


    time_t  LastSave;
    time_t  LastBFN;

    unsigned PLC;
};


class GlobalInfo
{
public:
    struct  player players[MAX_PLAYERS];     // Array of players

    struct  cars car[MAX_CARS];                    // Array of cars (need for shop)
    map < string , cars > carMap;

    /** Bad words **/
    int     WordsCount;
    char    Words[100][32];
    /**  **/
    struct  track_info TrackInf;             // Where PitBox and Shop

    /** IS_RST **/
    char    Track[6];                          // Current track (for streets)
    char    Product[6];                        // DEMO or S1 or S2
    /** insim **/
    char    IP[16];                            // IP port for connect (for InSim IS_III)
    word    TCPPORT;                           // TCP port (for InSim IS_III)
    word    UDPPORT;                           // UDP port for mci packets (for InSim IS_III)
    char    ADMIN[10];                         // Admin pass (for InSim IS_III)
};

void read_track();
void read_car();
void ShowUsersList(byte UCID);

void SendMTC (byte UCID,const char* Msg)
{
    IS_MTC *pack = new IS_MTC;
    memset( pack, 0, sizeof( IS_MTC ) );
    pack->Size = sizeof( IS_MTC );
    pack->Type = ISP_MTC;
    pack->UCID = UCID;
    sprintf( pack->Text, "%.127s\0", Msg );
    insim->send_packet( pack );
    delete pack;
};

void ClearBTN(byte UCID)
{
    IS_BFN *pack = new IS_BFN;
    memset( pack, 0, sizeof( IS_BFN ) );
    pack->Size = sizeof( IS_BFN );
    pack->Type = ISP_BFN;
    pack->ReqI = 0;
    pack->SubT = BFN_CLEAR;

    pack->UCID = UCID;
    insim->send_packet( pack );
    delete pack;
}

void SendButton(byte ReqI, byte UCID, byte ClickID, byte L, byte T, byte W, byte H, byte BStyle, const char * Text )
{
    IS_BTN *pack = new IS_BTN;
    memset( pack, 0, sizeof( IS_BTN ) );
    pack->Size = sizeof( IS_BTN );
    pack->Type = ISP_BTN;
    pack->ReqI = ReqI;
    pack->UCID = UCID;
    pack->Inst = 0;
    pack->BStyle = BStyle;
    pack->TypeIn = 0;
    pack->ClickID = ClickID;
    pack->L = L;
    pack->T = T;
    pack->W = W;
    pack->H = H;
    strcpy( pack->Text ,Text );
    insim->send_packet( pack );
    delete pack;
}

void SendMST (const char* Text)
{
    IS_MST *pack = new IS_MST;
    memset( pack, 0, sizeof( IS_MST));
    pack->Size = sizeof( IS_MST);
    pack->Type = ISP_MST;
    sprintf( pack->Msg, "%.63s\0", Text );
    insim->send_packet( pack );
    delete pack;
};


void SendBFN (byte UCID, byte ClickID)
{
    IS_BFN *pack = new IS_BFN;
    memset( pack, 0, sizeof( IS_BFN ) );
    pack->Size = sizeof( IS_BFN );
    pack->Type = ISP_BFN;
    pack->UCID = UCID;
    pack->ClickID = ClickID;
    insim->send_packet( pack );
    delete pack;
};

void SendPLC (byte UCID, unsigned PLC)
{
    IS_PLC *pack = new IS_PLC;
    memset( pack, 0, sizeof( IS_PLC ) );
    pack->Size = sizeof( IS_PLC );
    pack->Type = ISP_PLC;
    pack->UCID = UCID;
    pack->Cars = PLC;
    insim->send_packet( pack );
    delete pack;
}
#endif
