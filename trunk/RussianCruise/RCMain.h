#ifndef _CRUISE_H_
#define _CRUISE_H_
/**

Russian Cruise by TurboSnail

**/

/**

tunbyte
ecu     1
trb     2
wht     4
abs     8
skin    16

1;XFG;80000
2;XRG;120000
3;LX4;300000
4;LX6;350000
5;RB4;150000
6;FXO;180000
7;XRT;220000
8;MRT;55000
9;VWS;9999999
11;UF1;30000
12;RAC;350000
13;FZ5;300000
14;UFR;380000
15;XFR;420000
16;FXR;450000
17;XRR;480000
18;FZR;500000

Zones:
0- no works
1- time to change streetname
2- shop

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

workplace
L = 38;
T = 55;
W = 124;
H = 118;

*/
using namespace std;



#include <math.h>
#include <windows.h>
#include <time.h>
#include <map>

//#include "mysql/include/my_global.h"
#include "mysql/include/mysql.h"

#include "Cinsim.h"
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
#include "RCBanList.h"

//#include "sqlite/SQLite.h"



#define MAX_CARS 30

#ifndef _RC_POLICE_H
#define MAX_FINES 100
#endif

/*** GLOBAL ***/
char IS_PRODUCT_NAME[16];



// Дескриптор соединения
MYSQL rcMaindb;
// Дескриптор результирующей таблицы
MYSQL_RES *rcMainRes;
// Массив полей текущей строки
MYSQL_ROW rcMainRow;


time_t  stime;
int     chek_users =0;

CInsim *insim;

ofstream  out;   // выходной файл для протокола работы сервиса

char RootDir[MAX_PATH];
char ServiceName[64];

SERVICE_STATUS    service_status;
SERVICE_STATUS_HANDLE  hServiceStatus;
SC_HANDLE  hServiceControlManager, hService;

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);

pthread_mutex_t RCmutex;                // Mutex var used for send_packet() method

typedef struct cars
{
    int             id;
    char            car[6];
    unsigned long   cash;
    unsigned long   sell;
    unsigned		PLC;
} _cars;

typedef struct user_car
{
    char    car[6];
    int     tuning;
    float     dist;
} _user_car;



struct track_info
{
    int     PitCount;
    int     XPit[10];
    int     YPit[10];
    int     ShopCount;
    int     XShop[10];
    int     YShop[10];
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
    bool    Pitlane;
    byte    Shop; // NO DELETE!!!!
    int     Action;
    /*** bonus ***/
    float   Bonus_dist;
    int     Bonus_count;
    /******************/
    char    Lang[4];
    byte    lang_id;
    byte    Svetofor;

    /** Flood **/
    char    Msg[128];
    int     FloodCount;
    int     FloodTime;
    /** Work **/
    int     WorkTime;			// время за которое он должен доставить товар

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

void send_mtc (byte UCID,const char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    sprintf( pack_mtc.Text, "%.127s\0", Msg );
    insim->send_packet(&pack_mtc);
};

void send_mst (const char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    sprintf( pack_mst.Msg, "%.63s\0", Text );
    insim->send_packet(&pack_mst);
};


void send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack);
};

void send_plc (byte UCID, unsigned PLC)
{
    struct IS_PLC pack;
    memset(&pack, 0, sizeof(struct IS_PLC));
    pack.Size = sizeof(struct IS_PLC);
    pack.Type = ISP_PLC;
    pack.UCID = UCID;
    pack.Cars = PLC;
    insim->send_packet(&pack);
}
#endif
