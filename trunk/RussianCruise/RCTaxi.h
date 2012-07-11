#ifndef _RC_TAXI_H // Проверка, чтобы файл подключался один раз
#define _RC_TAXI_H

#include <math.h>       // sqrt pow
#include <stdio.h>      //
#include <iostream>     //
#include <time.h>       // для работы с временем и рандомом
#include <windows.h>    // не помню для чего но ингда нужно

#include "CInsim.h"     // Insim
#include "RCMessage.h"  // Messages
#include "RCBank.h"     // Bank

#include "RCDrivingLicense.h"
#include "RCStreet.h"
#include "RCPizza.h"

#include "tools.h"      // Check_Pos  etc.

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


// Задаем структуру игрока

struct TaxiPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    char    CName[4];              // Car Name
    byte    Zone;
    /** Work **/
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


    byte 	Start;
};



// Описание класса Такси
class RCTaxi
{
private:
    // Переменные и функции, доступные только самому классу
    string  GameTrack;

    time_t  acctime;
    int     accept_time;
    int     NumP;
    char RootDir[MAX_PATH]; // Полный путь до папки с программой

    int		DialContCount;
    char 	Dialog_Cont[11][128];

    int		DialObhCount;
    char 	Dialog_Obh[11][128];

    int		DialDistCount;
    char 	Dialog_Dist[11][128];

    int		DialDoneCount;
    char 	Dialog_Done[11][128];

    int		DialPastCount;
    char 	Dialog_Past[11][128];

    int		DialStopCount;
    char 	Dialog_Stop[11][128];

    int		DialExitCount;
    char 	Dialog_Exit[11][128];


    CInsim      *insim; // Переменная-указатель на класс CInsim
    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCBank      *bank;  // Переменная-указатель на класс RCBank
    RCDL        *dl;
    RCStreet    *street;


    struct  Taxi_info TrackInf;
    int PointCount;


    struct  Taxi_points *Points; // Рабочая строчка
    struct  Taxi_points PointsAdd[256]; //Для добавления точек


    struct  place zone;
    struct  TaxiPlayer players[32];     // Структура игроков



    void accept_user();
    void accept_user2(byte UCID);
    void taxi_done(TaxiPlayer *splayer);

    void taxi_ncn();   // Новый игрок зашел на сервер
    void taxi_npl();   // Игрок вышел из боксов
    void taxi_plp();   // Игрок ушел в боксы
    void taxi_pll();   // Игрок ушел в зрители
    void taxi_cnl();   // Игрок ушел с сервера
    void taxi_crp();   // Игрок переименовался
    void taxi_mso();   // Игрок отправил сообщение
    void con();   // Игрок отправил сообщение
    void obh();   // Игрок отправил сообщение

    void send_bfn(byte UCID, byte ClickID);
    void send_mst (const char* Text);
    void send_mst (string Text);
    void send_mtc (byte UCID,const char* Msg);

    void read_user(struct TaxiPlayer *splayer);
    void save_user(struct TaxiPlayer *splayer);

    void btn_stress(struct TaxiPlayer *splayer);
    void btn_Dist(struct TaxiPlayer *splayer, const char* Text);

public:
    // Переменные и функции, доступные для всех

    RCTaxi();   // Конструктор класса (обязательно)
    ~RCTaxi();  // Деструктор класса (обязательно)

    char errmsg[64];






    /*struct PTH pth;
    struct PTH_NODES nodes[2000];*/

    // Основные функции класса
    int init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet, void *Pizza);
    byte inited;

    void readconfig(const char *Track); // Чтение данных о точках "Пункт назначения"

    // функции-повторители основных фунцкий ядра
    void next_packet(); // Функция переборки типа пакета

    void taxi_mci();   // Пакет с данными о координатах и т.д.
    bool IfWork(byte UCID);


    // Функции-утилиты
    // int check_pos (struct PizzaPlayer *splayer); //+

};
#endif // #define _RC_TAXI_H
