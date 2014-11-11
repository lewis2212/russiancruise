#ifndef _RC_CHEAT_H // Проверка, чтобы файл подключался один раз
#define _RC_CHEAT_H

#include "RCBaseClass.h"
#include "RCMessage.h"  // Messages
#include "RCBank.h"     // Bank

#include "tools.h"      // Check_Pos  etc.

struct track_inf
{
    int     PitCount;
    int     XPit[10];
    int     YPit[10];
    int     ShopCount;
    int     XShop[10];
    int     YShop[10];
};

// Задаем структуру игрока

struct Player
{
    bool    InPit;
    bool    ReadyForMCI;
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name

    char    CName[4];              // Car Name
    /** TELEPORT **/
    float   Distance;
    /** NPL Hack **/
    int     NPLTime;
    int     HackTime;
    int     NPLHack;
    bool    NPL;
};

// Описание класса Такси
class RCAntCheat:public RCBaseClass
{
private:


    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCBank      *bank;  // Переменная-указатель на класс RCBank

    map<byte, Player>players;

    void InsimCNL( struct IS_CNL* packet );   // Игрок ушел с сервера
    void InsimCPR( struct IS_CPR* packet );   // Игрок переименовался
    void InsimMCI( struct IS_MCI* packet );   // Пакет с данными о координатах и т.д.
    void InsimMSO( struct IS_MSO* packet );   // Игрок отправил сообщение
    void InsimNCN( struct IS_NCN* packet );   // Новый игрок зашел на сервер
    void InsimNPL( struct IS_NPL* packet );   // Игрок вышел из боксов
    void InsimPLA( struct IS_PLA* packet );
    void InsimPLL( struct IS_PLL* packet );   // Игрок ушел в зрители
    void InsimPLP( struct IS_PLP* packet );   // Игрок ушел в боксы

    void pitlane(const char *UName);
    void cheat(struct  Player *splayer);


public:
    // Переменные и функции, доступные для всех

    RCAntCheat(const char* Dir);   // Конструктор класса (обязательно)
    ~RCAntCheat();  // Деструктор класса (обязательно)
    struct  track_inf TrackInf;             // Where PitBox and Shop
    // Основные функции класса
    int init(DBMySQL *conn,CInsim *InSim, void *Message);    // classname - указатель на класс RCAntCheat.

};
#endif // #define _RC_TAXI_H
