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



//#include "sqlite/SQLite.h"






#define IS_PRODUCT_NAME "Cruise v1.1.11"
#define MAX_CARS 30
#define MAX_FINES 100


/*** GLOBAL ***/


char* siren = "^0";



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
};

struct user_car
{
    char    car[6];
    int     tuning;
    int     dist;
};
struct user_fine
{
    int     fine_id;
    int     fine_date;
};

struct streets
{
    char    Street[50];
    word    NodeBeg;
    word    NodeEnd;
    int     SpeedLimit;
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
    struct  streets street[30];              // Array of streets
    byte    StreetNum;

    struct  track_info TrackInf;             // Where PitBox and Shop

    //char    message[20000][64];    // Messages !!!!!!!!!!!!!!!!!
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
    //byte    Bonus_key;
    int     Bonus_laps;
    byte    Bonus_s1;
    byte    Bonus_s2;
    byte    Bonus_s3;
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
    /** NPL Hack **/
    int     NPLTime;
    int     HackTime;
    int     NPLHack;
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

    /** IS_STA **/
    /** IS_RST **/
    word	Node_Finish;		// node index - finish line
    word	Node_Split1;		// node index - split 1 byte = 1
    word	Node_Split2;		// node index - split 2 byte = 2
    word	Node_Split3;		// node index - split 3 byte = 4
    int     Splits_Count;       // split1(1) + split2(2) + split3(4) = max 7
    char    Track[6];                          // Current track (for streets)
    char    Product[6];                        // DEMO or S1 or S2
    /** insim **/
    char    IP[16];                            // IP port for connect (for InSim IS_III)
    word    TCPPORT;                           // TCP port (for InSim IS_III)
    word    UDPPORT;                           // UDP port for mci packets (for InSim IS_III)
    char    ADMIN[10];                         // Admin pass (for InSim IS_III)
};

void read_track(struct player *splayer);
void read_lang(struct player *splayer);
void read_car();
void read_fines();
int read_cop(struct player *splayer);



void send_mtc (byte UCID,char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strcpy(pack_mtc.Msg, Msg);
    insim.send_packet(&pack_mtc);
};

void send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim.send_packet(&pack_mst);
};


void send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim.send_packet(&pack);
};

/*************************************************************************************************
*
*                   WORK
*       USING FOR POSTMAN AND PIZZA
*
*
*
**************************************************************************************************/

/**
class PostPizza
{
private:
    char    Product[6];
    char    WorkName[16];
    byte    WorkType;
    struct  place zone;

public:
    PostPizza::PostPizza();
    void init(char *Name, char *Lic,byte Type); //+
    void readconfig(char *Track); //+
    void deal(struct player *splayer); //+
    void undeal(struct player *splayer, char *Reason);
    void take (struct player *splayer, struct global_info *ginfo); //+
    void done (struct player *splayer); //+
    int check_pos (struct player *splayer); //+
};



PostPizza::PostPizza()
{

}

void PostPizza::init(char *Name, char *Lic,byte Type)
{
    strcpy(WorkName,Name);
    strcpy(Product,Lic);
    WorkType = Type;

}

**/


/******************************************************************
*
*               SVETOFOR ODNAKO
*
*
******************************************************************/

const char* signal1 ="^0•";
const char* signal2 ="^0•";
const char* signal3 ="^0•";
const char* signal11 ="^0•";
const char* signal12 ="^0•";
const char* signal13 ="^0•";


void btn_svetofor1 (struct player *splayer)
{

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.BStyle = 32;
    pack.TypeIn = 0;

    pack.ClickID = 190;
    pack.L = 92;
    pack.T = 15;
    pack.W = 16;
    pack.H = 40;
    insim.send_packet(&pack);

    pack.BStyle = 16;
    pack.ClickID = 191;
    pack.L = 93;
    pack.T = 16;
    pack.W = 14;
    pack.H = 38;
    insim.send_packet(&pack);



    pack.BStyle = 1;
    pack.ClickID = 192;
    pack.L = 85;
    pack.T = 10;
    pack.W = 30;
    pack.H = 30;
    strcpy(pack.Text,signal1);
    insim.send_packet(&pack);


    pack.ClickID = 193;
    pack.T = 10+10;
    strcpy(pack.Text,signal2);
    insim.send_packet(&pack);

    pack.ClickID = 194;
    pack.T = 10+20;
    strcpy(pack.Text,signal3);
    insim.send_packet(&pack);
}

void btn_svetofor2 (struct player *splayer)
{

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.BStyle = 32;
    pack.TypeIn = 0;

    pack.ClickID = 190;
    pack.L = 92;
    pack.T = 15;
    pack.W = 16;
    pack.H = 40;
    insim.send_packet(&pack);

    pack.BStyle = 16;
    pack.ClickID = 191;
    pack.L = 93;
    pack.T = 16;
    pack.W = 14;
    pack.H = 38;
    insim.send_packet(&pack);



    pack.BStyle = 1;
    pack.ClickID = 192;
    pack.L = 85;
    pack.T = 10;
    pack.W = 30;
    pack.H = 30;
    strcpy(pack.Text,signal11);
    insim.send_packet(&pack);


    pack.ClickID = 193;
    pack.T = 10+10;
    strcpy(pack.Text,signal12);
    insim.send_packet(&pack);

    pack.ClickID = 194;
    pack.T = 10+20;
    strcpy(pack.Text,signal13);
    insim.send_packet(&pack);
}


void btn_svetofor3 (struct player *splayer)
{

    struct IS_BTN pack_btn;
    memset(&pack_btn, 0, sizeof(struct IS_BTN));
    pack_btn.Size = sizeof(struct IS_BTN);
    pack_btn.Type = ISP_BTN;
    pack_btn.ReqI = 1;
    pack_btn.UCID = splayer->UCID;
    pack_btn.Inst = 0;
    pack_btn.BStyle = 32;
    pack_btn.TypeIn = 0;
    /**** telo1 **/
    pack_btn.ClickID = 195;
    pack_btn.L = 148;
    pack_btn.T = 110;
    pack_btn.W = 8;
    pack_btn.H = 20;
    strcpy(pack_btn.Text,"");
    insim.send_packet(&pack_btn);
    /**********/


    pack_btn.BStyle = 1;
    pack_btn.ClickID = 196;
    pack_btn.L = 144;
    pack_btn.T = 107;
    pack_btn.W = 16;
    pack_btn.H = 16;
    strcpy(pack_btn.Text,signal1);
    insim.send_packet(&pack_btn);


    pack_btn.ClickID = 197;
    pack_btn.T = 112;
    strcpy(pack_btn.Text,signal2);
    insim.send_packet(&pack_btn);

    pack_btn.ClickID = 198;
    pack_btn.T = 117;
    strcpy(pack_btn.Text,signal3);
    insim.send_packet(&pack_btn);
    /********************************/
    pack_btn.BStyle = 32;
    /**** telo 2 *******/
    pack_btn.ClickID = 199;
    pack_btn.L = 157;
    pack_btn.T = 110;
    pack_btn.W = 8;
    pack_btn.H = 20;
    strcpy(pack_btn.Text,"");
    insim.send_packet(&pack_btn);
    /*************************/
    pack_btn.BStyle = 1;

    pack_btn.ClickID = 200;
    pack_btn.L = 153;
    pack_btn.T = 107;
    pack_btn.W = 16;
    pack_btn.H = 16;
    strcpy(pack_btn.Text,signal11);
    insim.send_packet(&pack_btn);


    pack_btn.ClickID = 201;
    pack_btn.T = 112;
    strcpy(pack_btn.Text,signal12);
    insim.send_packet(&pack_btn);

    pack_btn.ClickID = 202;
    pack_btn.T = 117;
    strcpy(pack_btn.Text,signal13);
    insim.send_packet(&pack_btn);
};

void clear_svetofor(struct player *splayer)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.ReqI = 0;
    pack.SubT = 0;
    pack.UCID = splayer->UCID;
    pack.ClickID = 190;
    insim.send_packet(&pack);
    pack.ClickID = 191;
    insim.send_packet(&pack);
    pack.ClickID = 192;
    insim.send_packet(&pack);
    pack.ClickID = 193;
    insim.send_packet(&pack);
    pack.ClickID = 194;
    insim.send_packet(&pack);
};

#endif
