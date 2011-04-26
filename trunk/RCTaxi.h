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

#include "tools.h"      // Check_Pos  etc.

struct Taxi_info
{
    int     ShopCount;
    int     XShop[10];
    int     YShop[10];
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
    int     WorkNodeDestinaion;     // Номер нода трассы, куда надо доставить
    int     WorkTime;			    // Время за которое он должен доставить товар

    byte    InZone;
    bool    OnStreet;
    bool    InPasZone;
    byte    WorkNow;
    int     Work;
    int     FiredPenalty;
    int     PenaltyCount;
    int     PassCount;
};



// Описание класса Такси
class RCTaxi
{
private:
    // Переменные и функции, доступные только самому классу
    time_t  acctime;
    int     accept_time;
    int     NumP;
    char RootDir[MAX_PATH]; // Полный путь до папки с программой

    void accept_user();

public:
    // Переменные и функции, доступные для всех

    RCTaxi::RCTaxi();   // Конструктор класса (обязательно)
    RCTaxi::~RCTaxi();  // Деструктор класса (обязательно)

    char errmsg[64];
    CInsim      *insim; // Переменная-указатель на класс CInsim
    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCBank      *bank;  // Переменная-указатель на класс RCBank
    RCDL        *dl;
    RCStreet    *street;

    struct  Taxi_info TrackInf;
    struct  place zone;
    struct  TaxiPlayer players[32];     // Структура игроков

    struct PTH pth;
    struct PTH_NODES nodes[2000];

    // Основные функции класса
    int init(char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet);
    byte inited;

    void readconfig(char *Track); // Чтение данных о точках "Пункт назначения"

    // функции-повторители основных фунцкий ядра
    void next_packet(); // Функция переборки типа пакета
    void taxi_ncn();   // Новый игрок зашел на сервер
    void taxi_npl();   // Игрок вышел из боксов
    void taxi_plp();   // Игрок ушел в боксы
    void taxi_pll();   // Игрок ушел в зрители
    void taxi_cnl();   // Игрок ушел с сервера
    void taxi_crp();   // Игрок переименовался
    void taxi_mci();   // Пакет с данными о координатах и т.д.
    void taxi_mso();   // Игрок отправил сообщение

    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);

    void read_user(struct TaxiPlayer *splayer);
    void save_user(struct TaxiPlayer *splayer);

    // Функции-утилиты
    // int check_pos (struct PizzaPlayer *splayer); //+

};
#endif // #define _RC_TAXI_H
// TODO: Юзать .pth для "Пункт назначения" по возможности
// Привязка к нодам (Рандом)
