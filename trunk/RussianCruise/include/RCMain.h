// *** ADDED BY HEADER FIXUP ***
#include <ctime>
// *** END ***
 #include<clocale>

#ifndef _CRUISE_H_
#define _CRUISE_H_
/**

Russian Cruise by TurboSnail

**/





#include <windows.h>
#include <mysql/mysql.h>

#include <json/json.h>

#include <math.h>
#include <map>
#include <thread>

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
#include "RCPolice.h"
#include "RCTaxi.h"
#include "RCRoadSign.h"

#include "RCQuest.h"
// #include "RCAutoschool.h"

#define MAX_CARS 30

using namespace std;


/*** GLOBAL ***/
char IS_PRODUCT_NAME[16];

// Дескриптор соединения
MYSQL rcMaindb, *rcMaindbConn;
// Дескриптор результирующей таблицы
MYSQL_RES *rcMainRes;
// Массив полей текущей строки
MYSQL_ROW rcMainRow;


time_t  s_time;
int     chek_users =0;

CInsim *insim;

int ok = 1;

struct IS_VER pack_ver;
word	isf_flag;

char RootDir[MAX_PATH];
char ServiceName[64];

list<RCBaseClass *> classes;
list<RCBaseClass *>::iterator cl;

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
    int     CafeCount;
    int     *XCafe = NULL;
    int     *YCafe = NULL;
};

struct player: public GlobalPlayer
{
    struct CompCar Info;
    user_car cars[MAX_CARS];
    //map <string, user_car> cars;

    char    UName[24];             // Username
    int     CTune;                 // Car Tuning
    byte    SetF;
    byte    H_TRes;
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
    char    Msg[128];
    int     FloodCount;
    int     FloodTime;


    time_t  LastSave;
    time_t  LastBFN;

    unsigned PLC;
};

map < byte, player > players;
map < byte, byte >PLIDtoUCID;
map < string , cars > carMap;

class GlobalInfo
{
public:

    cars car[MAX_CARS];                    // Array of cars (need for shop)

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

Json::Value 		bonuses;
Json::Reader 		bonusesReader;
Json::StyledWriter 	bonusesWriter;
void ReadBonuses();
void SaveBonuses();
#endif
