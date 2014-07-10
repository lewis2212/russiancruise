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

    // Переменные и функции, доступные только самому классу
    bool InsimMSO( struct IS_MSO* packet );   // Игрок отправил сообщение
    //bool InsimREO( struct IS_REO* packet );
    bool InsimPLA( struct IS_PLA* packet );
    // функции-повторители основных фунцкий ядра
    //void next_packet(); // Функция переборки типа пакета
    bool InsimNCN( struct IS_NCN* packet );   // Новый игрок зашел на сервер
    bool InsimNPL( struct IS_NPL* packet );   // Игрок вышел из боксов
    bool InsimPLP( struct IS_PLP* packet );   // Игрок ушел в боксы
    bool InsimPLL( struct IS_PLL* packet );   // Игрок ушел в зрители
    bool InsimCNL( struct IS_CNL* packet );   // Игрок ушел с сервера
    bool InsimCPR( struct IS_CPR* packet );   // Игрок переименовался

    void pitlane(const char *UName);
    void cheat(struct  Player *splayer);


public:
    // Переменные и функции, доступные для всех

    RCAntCheat(const char* Dir);   // Конструктор класса (обязательно)
    ~RCAntCheat();  // Деструктор класса (обязательно)
    struct  track_inf TrackInf;             // Where PitBox and Shop
    // Основные функции класса
    int init(MYSQL *conn,CInsim *InSim, void *Message);    // classname - указатель на класс RCAntCheat.
    bool InsimMCI( struct IS_MCI* packet );   // Пакет с данными о координатах и т.д.

};
#endif // #define _RC_TAXI_H
