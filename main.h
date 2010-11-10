#ifndef _CRUISE_H_
#define _CRUISE_H_
/**

Russian Cruise by TurboSnail

v. 1.1.3 at 10.05.10
        - круиз работает в 2 режимах. консоль и сервис(служба)
        - so4 pit+ shop+ cafe+ work+points++
        - add cop system (!pay !sirena !users++)
        - check nickname when player work cop
        - забирать заказ из пиццирии можно не дожида€сь вызова

v 1.1.35 at 17.05.10
        - delete skin system
        - fix bugs

v 1.1.37 at 19.05.10
        - !radar for on/off radar (auto off if Speed > 5 km/h)
        - !trans show all cash transactions of player (if !trans user => show all cash transactions of player and user)


TODO:
        - button for pizza with action info
        - maping WE1R, ky3 (pitzone, shop, cafe, work, workpoints)
        - add light system on WE1R, KY3
        - add recovery_car,ambulator_car system

**/

/**

xfg-1
xrg-2
lx4-3
lx6-4
rb4-5
fxo-6
xrt-7
mrt-8

uf1-9
rac-10
fz5-11
ufr-12
xfr-13
fxr-14
xrr-15
fzr-16

Zones:
0- no works
1- time to change streetname
2- shop

buttons ClickId's:

1-49  - users
50- 79 - cars
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

workplace
L = 38;
T = 55;
W = 124;
H = 118;

*/
using namespace std;
typedef unsigned char byte;
typedef unsigned short word;

#include <fstream>

#include <math.h>
#include "CInsim.h"
#include <windows.h>
#include <time.h>






#define IS_PRODUCT_NAME "Cruise v1.2.1"
#define MAX_CARS 30
#define MAX_FINES 100


/*** GLOBAL ***/


char* siren = "^0";



time_t  stime;
int     chek_users =0;

CInsim insim;
ofstream  out;   // выходной файл дл€ протокола работы сервиса

SERVICE_STATUS    service_status;
SERVICE_STATUS_HANDLE  hServiceStatus;
SC_HANDLE  hServiceControlManager, hService;

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);
char RootDir[MAX_PATH];
char ServiceName[64];



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
    int     CafeCount;
    int     XCafe[10];
    int     YCafe[10];
};


struct player
{
    struct  CompCar Info;
    struct user_car cars[MAX_CARS];
    struct user_fine fines[MAX_FINES];

    int     str_count;                          // Count of streets
    struct  streets street[30];              // Array of streets
    byte    StreetNum;

    struct  track_info TrackInf;             // Where PitBox and Shop

    char    message[20000][64];    // Messages !!!!!!!!!!!!!!!!!
    char    UName[24];             // Username
    char    PName[24];             // Player name
    char    Cars[256];             // Avalible Cars
    char    CName[4];              // Car Name
    char    SName[16];             // Car Skin Name
    int     CTune;                 // Car Tuning
    byte    UCID;                  // Connection ID
    byte    PLID;                  // Player ID
    byte    BID;
    byte    BID2;
    byte    bfn;
    float   cash;       // ƒеньги
    float   Distance;
    byte    Zone;
    byte    Shop; // NO DELETE!!!!
    int     Action;
    char    Lang[4];
    byte    Svetofor;
    /** COP **/
    byte    cop;
    byte    radar;
    byte    sirena;         // коповский выключатель сирены
    byte    sirenaOnOff;    // поста€нна€ запись положени€ сирены у духов
    byte    sirenaKey;      // определ€ем включить или выключить сирену у духов
    byte    Pogonya;
    char    PogonyaReason[64];
    int     StopTime;
    /** Energy **/
    int     Energy; // Energy xD
    time_t  EnergyTime;
    /** Flood **/
    char    Msg[96];
    int     FloodCount;
    int     FloodTime;
    /** Work **/
    byte    WorkZone;
    byte    WorkType;			// тип работы
    byte    WorkAccept;			// 0 = не зан€т работой , 1 = зан€т работой
    byte    WorkPlayerAccept;   // если какойто плеер заказал пиццу (100 + позици€ в массиве)
    byte    WorkDestinaion;		// номер точки доставки
    int     WorkTime;			// врем€ за которое он должен доставить товар
    int     WorkCountDone;
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
    struct  cars car[20];                    // Array of cars (need for shop)
    byte    Sp2;
    struct  fine fines[100];                 // Array of fines (for cops)
    byte    Sp3;
    /** Bad words **/
    int     WordsCount;
    char    Words[100][32];
    /**  **/

    /** IS_STA **/
    byte	NumP;			// Number of players in race
    byte	NumConns;		// Number of connections including host
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

// for localization




bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y)
{

    int      i, j=polySides-1 ;
    bool  oddNodes=false     ;

    for (i=0; i<polySides; i++)
    {
        if (polyY[i]<y && polyY[j]>=y
                ||  polyY[j]<y && polyY[i]>=y)
        {
            if (polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i])<x)
            {
                oddNodes=!oddNodes;
            }
        }
        j=i;
    }
    return oddNodes;
}

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

struct worker
{
    byte    UCID;
    /** Work **/
    byte    WorkZone;
    byte    WorkType;			// тип работы
    byte    WorkAccept;			// 0 = не зан€т работой , 1 = зан€т работой
    byte    WorkPlayerAccept;   // если какойто плеер заказал пиццу (100 + позици€ в массиве)
    byte    WorkDestinaion;		// номер точки доставки
    int     WorkTime;			// врем€ за которое он должен доставить товар
    int     WorkCountDone;

};

struct square           // square of destination point
{
    int     X[5];       // 4 X points
    int     Y[5];       // 4 Y points
    char    Place[64];  // The name of Destination Point
};

struct place
{
    int     dealX[4];       // 4 X points of Dealer Place
    int     dealY[4];       // 4 Y points of Dealer Place
    int     NumPoints;      // Count of Destination points/ Need for random
    struct  square point[40]; // Destination points. See Bellow
};

enum
{
    WK_NULL =0,
    WK_PIZZA =1,
    WK_POST =2,
    WK_TAXI =3,
    WK_DRUGS =4,
    WK_LOTTERY =5
};


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

void PostPizza::deal(struct player *splayer)
{

    if (splayer->WorkType == WK_NULL)
    {

        splayer->WorkType = WK_PIZZA;
        splayer->WorkAccept = 0;
        send_mtc(splayer->UCID,splayer->message[4000]);


    }
    else if (splayer->WorkType == WK_PIZZA)
    {
        send_mtc(splayer->UCID,splayer->message[4001]);
    }
    else
    {
        send_mtc(splayer->UCID,splayer->message[4002]);
    }
}

int PostPizza::check_pos(struct player *splayer)
{
    int PLX = splayer->Info.X/65536;
    int PLY = splayer->Info.Y/65536;

    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
    {
        return 1;
    }

    if (Check_Pos(4,zone.point[splayer->WorkDestinaion].X,zone.point[splayer->WorkDestinaion].Y,PLX,PLY))
    {
        return 2;
    }

    return 0;
}


void PostPizza::take(struct player *splayer, struct global_info *ginfo)
{
    if (splayer->WorkType == WK_PIZZA)
    {
        if (splayer->WorkAccept == 1)
        {
            if (splayer->WorkPlayerAccept == 0)
            {
                srand(time(&stime));
                int place = rand()%zone.NumPoints;

                if (place == 0)
                    place ++;

                int worktime = time(&stime);
                splayer->WorkTime = worktime+60*6;
                splayer->WorkDestinaion = place;
                splayer->WorkAccept = 2;
                send_mtc(splayer->UCID,splayer->message[4200]);
                send_mtc(splayer->UCID,zone.point[place].Place);
            }
            else if (splayer->WorkPlayerAccept != 0)
            {
                int worktime = time(&stime);
                splayer->WorkTime = worktime+60*6;
                splayer->WorkDestinaion = splayer->WorkPlayerAccept;
                splayer->WorkAccept = 2;
                char text[96];
                strcpy(text,splayer->message[4201]);
                strcat(text,ginfo->players[splayer->WorkPlayerAccept - 100].PName);
                send_mtc(splayer->UCID,text);
            }

        }
        else
        {
            send_mtc(splayer->UCID,splayer->message[4202]);
        }
    }
    else
    {
        send_mtc(splayer->UCID,splayer->message[4203]);
    }
}

void PostPizza::done(struct player *splayer)
{

    //cout << "true" << endl;
    if ((splayer->WorkType == WK_PIZZA) and (splayer->WorkAccept == 2))
    {
        splayer->WorkDestinaion =0;
        splayer->WorkAccept = 0;
        splayer->WorkPlayerAccept = 0;
        splayer->cash += 800;
        splayer->WorkCountDone ++;
        send_mtc(splayer->UCID,splayer->message[4300]);
        send_bfn(splayer->UCID,210);
        splayer->WorkZone =0;
    }

}
void PostPizza::undeal(struct player *splayer, char *Reason)
{

    //cout << "true" << endl;
    if (splayer->WorkType == WK_PIZZA)
    {
        send_bfn(splayer->UCID,210);
        send_mtc(splayer->UCID,splayer->message[4100]);
        send_mtc(splayer->UCID,Reason);
        splayer->WorkType = WK_NULL;
        splayer->WorkDestinaion =0;
        splayer->WorkAccept = 0;
    }

}

void PostPizza::readconfig(char *Track)
{
    char file[MAX_PATH];
    strcpy(file,RootDir);
    strcat(file,"work\\");
    strcat(file,Track);
    strcat(file,"_");
    strcat(file,WorkName);
    strcat(file,".txt");


    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);


    int point = 0;

    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);


        if (strlen(str) > 0)
        {

            if (strncmp(str,"/dealer",7)==0)
            {
                for (int i=0; i<4; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,",");
                    Y = strtok (NULL,",");
                    zone.dealX[i] = atoi(X);
                    zone.dealY[i] = atoi(Y);
                    // cout << point << ". X=" << X << "; Y=" << Y << ";\n";
                }
            } // if /street

            if (strncmp(str,"point",5)==0)
            {
                readf.getline(str,64);
                strncpy(zone.point[point].Place,str,strlen(str));
                // cout << "place[" << point << "]= " << pizza.point[point].Place << endl;
                for (int i=0; i<4; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,",");
                    Y = strtok (NULL,",");
                    // cout << point << ". X["<<i<<"]= " << X << ", Y["<<i<<"]= " << Y << endl;
                    zone.point[point].X[i] = atoi(X);
                    zone.point[point].Y[i] = atoi(Y);
                }
                point ++;
                zone.NumPoints = point;
            } // if /street


        } // if strlen > 0
    } //while readf.good()

    readf.close();
}


/******************************************************************
*
*               SVETOFOR ODNAKO
*
*
******************************************************************/

const char* signal1 ="^0Х";
const char* signal2 ="^0Х";
const char* signal3 ="^0Х";
const char* signal11 ="^0Х";
const char* signal12 ="^0Х";
const char* signal13 ="^0Х";


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
