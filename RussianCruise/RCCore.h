//#ifndef _RC_CORE_H
//#define _RC_CORE_H


#include <math.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <windows.h>

#include "CInsim.h"
#include "RCMessage.h"
#include "RCBank.h"
#include "RCEnergy.h"

#include "tools.h"

const int MAX_CARS = 30;
const int MAX_FINES = 100;

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

class RCCore
{
    private:

    char RootDir[MAX_PATH];
    struct  player players[MAX_PLAYERS];     // Array of players

     /** IS_RST **/
    word	Node_Finish;		// node index - finish line
    word	Node_Split1;		// node index - split 1 byte = 1
    word	Node_Split2;		// node index - split 2 byte = 2
    word	Node_Split3;		// node index - split 3 byte = 4
    int     Splits_Count;       // split1(1) + split2(2) + split3(4) = max 7
    char    Track[6];                          // Current track (for streets)
    char    Product[6];                        // DEMO or S1 or S2

    public:
    RCCore();
    ~RCCore();

    CInsim      *insim;
    RCMessage   *msg;
    RCBank      *bank;

    int init(char *dir,void *CInSim);

    void help_cmds (struct player *splayer,int h_type);
    void btn_main (struct player *splayer);
   // void btn_info (struct player *splayer, int b_type);
    void btn_panel (struct player *splayer);
    void case_bfn ();
    void case_btc ();
    void case_btt ();
    void case_cnl ();
    void case_toc ();
    void case_cpr ();
    void case_mci ();
    void case_mci_svetofor ();
    void case_mso ();
    void case_ncn ();
    void case_npl ();
    void case_pen ();
    void case_pla ();
    void case_pll ();
    void case_plp ();
    void case_rst ();
    void case_vtn ();

    void send_mtc (byte UCID,char* Msg);
    void send_mst (char* Text);
    void send_bfn (byte UCID, byte ClickID);

    void btn_svetofor1 (struct player *splayer);
    void btn_svetofor2 (struct player *splayer);
    void btn_svetofor3 (struct player *splayer);
    void clear_svetofor (struct player *splayer);

};


//#enfif
