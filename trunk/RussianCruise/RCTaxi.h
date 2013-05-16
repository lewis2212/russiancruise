#ifndef _RC_TAXI_H // Проверка, чтобы файл подключался один раз
#define _RC_TAXI_H

#include "RCBaseClass.h"

#include "RCMessage.h"  // Messages
#include "RCBank.h"     // Bank
#include "RCDrivingLicense.h"
#include "RCStreet.h"
#include "RCPolice.h"
#include "RCLight.h"

#include "tools.h"      // Check_Pos  etc.

#define MAX_POINTS 2048
#define PASSANGER_INTERVAL 1200

struct Taxi_info
{
    int     ShopCount;
    int     XShop[10];
    int     YShop[10];
};

struct Taxi_points
{
    byte    Id;
    int     StreetId;
    int     X;
    int     Y;
};

struct Taxi_clients
{
    int     X;
    int     Y;
    int     Z;
    int     Dir;
    int     StreetId;
};


// Задаем структуру игрока

struct TaxiPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    char    CName[3];              // Car Name
    byte    Zone;

    /** Work **/
    bool CanWork;
    bool HandUp;
    int spd;

	int		AcceptTime;
    char    WorkDest[96];           // destination text
    byte    WorkAccept;			    // 0 = не занят работой , 1 = занят работой
    int     WorkStreetDestinaion;
    int     WorkPointDestinaion;     // Номер точки трассы, куда надо доставить
    int     WorkTime;			    // Время за которое он должен доставить товар

    byte    InZone;
    bool    OnStreet;
    bool    InPasZone;
    byte    WorkNow;
    int     Work;
    int     FiredPenalty;
    int     PenaltyCount;
    int     PassCount;

    int		PassStress; // from 0 to 1000
    int		StressOverCount;

    time_t  LastT;
};



// Описание класса Такси
class RCTaxi: public RCBaseClass
{
private:
    // Переменные и функции, доступные только самому классу
    time_t  acctime;
    int     NumP = 0;

    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCBank      *bank;  // Переменная-указатель на класс RCBank
    RCDL        *dl;
    RCStreet    *street;
    RCLight    	*lgh;
    RCPolice    *police;

    struct  Taxi_info TrackInf;
    int PointCount;
    int ClientCount;


    struct  Taxi_points *Points; // Рабочая строчка
    struct  Taxi_points PointsAdd[2048]; //Для добавления точек

    struct  Taxi_clients ClientPoints[ MAX_POINTS ];
    bool    StartPointsAdd;

    struct  place zone;

    map<string, map<int,string> >TaxiDialogs;
    map<byte, TaxiPlayer>players;     // Структура игроков

    void accept_user( byte UCID );
    void accept_user2( byte UCID );
    void taxi_done( byte UCID );

    void InsimNCN( struct IS_NCN* packet );   // Новый игрок зашел на сервер
    void InsimNPL( struct IS_NPL* packet );   // Игрок вышел из боксов
    void InsimPLP( struct IS_PLP* packet );   // Игрок ушел в боксы
    void InsimPLL( struct IS_PLL* packet );   // Игрок ушел в зрители
    void InsimCNL( struct IS_CNL* packet );   // Игрок ушел с сервера
    void InsimCPR( struct IS_CPR* packet );   // Игрок переименовался
    void InsimMSO( struct IS_MSO* packet );   // Игрок отправил сообщение
    void InsimCON( struct IS_CON* packet );
    void InsimOBH( struct IS_OBH* packet );
    void InsimHLV( struct IS_HLV* packet );

    void read_user( byte UCID );
    void save_user( byte UCID );

    void delete_marshal( byte UCID );

    void btn_stress( byte UCID );
    void btn_Dist( byte UCID , const char* Text);

public:

    RCTaxi();   // Конструктор класса (обязательно)
    ~RCTaxi();  // Деструктор класса (обязательно)


    // Основные функции класса
    int init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet, void * Police, void * Light);
    byte inited;

    void dead_pass(byte UCID);
    void readconfig(const char *Track); // Чтение данных о точках "Пункт назначения"


    void InsimMCI( struct IS_MCI* packet );   // Пакет с данными о координатах и т.д.
    bool IfWork(byte UCID);
    void Event();


};
#endif // #define _RC_TAXI_H
