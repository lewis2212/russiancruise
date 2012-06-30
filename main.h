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

#include "CInsim.h"
#include "RCPizza.h"
#include "RCMessage.h"
#include "RCEnergy.h"
#include "RCBank.h"
#include "RCDrivingLicense.h"
#include "RCAntCheat.h"
#include "RCStreet.h"
#include "RCLight.h"
#include "RCTaxi.h"

//#include "sqlite/SQLite.h"



#define MAX_CARS 30
#define MAX_FINES 100


/*** GLOBAL ***/
char IS_PRODUCT_NAME[16];

const char* siren = "^0";



time_t  stime;
int     chek_users =0;

CInsim insim;

ofstream  out;   // выходной файл для протокола работы сервиса

char RootDir[MAX_PATH];
char ServiceName[64];

SERVICE_STATUS    service_status;
SERVICE_STATUS_HANDLE  hServiceStatus;
SC_HANDLE  hServiceControlManager, hService;

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);

pthread_mutex_t RCmutex;                // Mutex var used for send_packet() method



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
struct user_fine
{
    int     fine_id;
    int     fine_date;
};



struct track_info
{
    int     PitCount;
    int     XPit[10];
    int     YPit[10];
    int     ShopCount;
    int     XShop[10];
    int     YShop[10];
};

struct CompCar2 // Car info in 28 bytes - there is an array of these in the MCI (below)
{
    word	Node;		// current path node
    word	Lap;		// current lap
    byte	PLID;		// player's unique id
    byte	Position;	// current race position : 0 = unknown, 1 = leader, etc...
    byte	Info;		// flags and other info - see below
    byte	Sp3;
    int		X;			// X map (65536 = 1 metre)
    int		Y;			// Y map (65536 = 1 metre)
    int		Z;			// Z alt (65536 = 1 metre)
    int		X2;			// X map (65536 = 1 metre)
    int		Y2;			// Y map (65536 = 1 metre)
    int		Z2;			// Z alt (65536 = 1 metre)
    word	Speed;		// speed (32768 = 100 m/s)
    word	Direction;	// direction of car's motion : 0 = world y direction, 32768 = 180 deg
    word	Heading;	// direction of forward axis : 0 = world y direction, 32768 = 180 deg
    short	AngVel;		// signed, rate of change of heading : (16384 = 360 deg/s)
};


struct player
{
    struct  CompCar2 Info;
    struct user_car cars[MAX_CARS];
    struct user_fine fines[MAX_FINES];

    int     str_count;                          // Count of streets
    byte    StreetNum;



    //char    GetMessage[20000][64];    // GetMessages !!!!!!!!!!!!!!!!!
    char    UName[24];             // Username
    char    PName[24];             // Player name
    //char    Cars[256];             // Avalible Cars
    char    CName[4];              // Car Name
    char    SName[16];             // Car Skin Name
    int     CTune;                 // Car Tuning
    byte    SetF;
    byte    H_TRes;
    byte    UCID;                  // Connection ID
    byte    PLID;                  // Player ID
    byte    BID;
    byte    BID2;
    byte    bfn;
    //float   cash;       // Деньги
    float   Distance;
    byte    Zone;
    byte    Shop; // NO DELETE!!!!
    int     Action;
    /*** bonus ***/
    float   Bonus_dist;
    int     Bonus_count;
    /******************/
    char    Lang[4];
    byte    lang_id;
    byte    Svetofor;
    /** COP **/
    byte    cop;
    byte    radar;
    byte    sirena;         // коповский выключатель сирены
    byte    sirenaOnOff;    // постаянная запись положения сирены у духов
    byte    sirenaKey;      // определяем включить или выключить сирену у духов
    int     sirenaSize;      // размер кнопки
    byte    Pogonya;
    char    PogonyaReason[64];
    int     StopTime;
    byte    Penalty;        // Если превысил скорость в питах
    /** Energy **/
    //int     Energy; // Energy xD
    //time_t  EnergyTime;
    /** Flood **/
    char    Msg[96];
    int     FloodCount;
    int     FloodTime;
    /** Work **/
    int     WorkTime;			// время за которое он должен доставить товар

    time_t  LastSave;
    time_t  LastBFN;

    unsigned PLC;
};


struct fine
{
    int     id;
    char    name[64];
    int     cash;
};

struct global_info
{
    struct  player players[MAX_PLAYERS];     // Array of players
    byte    Sp1;
    struct  cars car[MAX_CARS];                    // Array of cars (need for shop)
    byte    Sp2;
    struct  fine fines[MAX_FINES];                 // Array of fines (for cops)
    byte    Sp3;
    /** Bad words **/
    int     WordsCount;
    char    Words[100][32];
    /**  **/
    struct  track_info TrackInf;             // Where PitBox and Shop

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

void read_track();
void read_lang(struct player *splayer);
void read_car();
void read_fines();
int read_cop(struct player *splayer);

char errmsg[64];

void send_mtc (byte UCID,const char* Msg)
{
    char errmsg[64];
    ZeroMemory(&errmsg,64);
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strncpy(pack_mtc.Text, Msg,strlen(Msg));
    insim.send_packet(&pack_mtc);
};

void send_mst (const char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strncpy(pack_mst.Msg,Text,strlen(Text));
    insim.send_packet(&pack_mst,errmsg);
};


void send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim.send_packet(&pack,errmsg);
};

void send_plc (byte UCID, unsigned PLC)
{

	struct IS_PLC pack;
    memset(&pack, 0, sizeof(struct IS_PLC));
    pack.Size = sizeof(struct IS_PLC);
    pack.Type = ISP_PLC;
    pack.UCID = UCID;
    pack.Cars = PLC;
    insim.send_packet(&pack,errmsg);
}



/******************************************************************
*
*               SVETOFOR ODNAKO
*
*
******************************************************************/



#endif
