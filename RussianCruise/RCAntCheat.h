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
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
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

    //struct  Player players[ MAX_PLAYERS ];     // Структура игроков
    struct  Player *players;     // Структура игроков

    // Переменные и функции, доступные только самому классу
    void insim_mso( struct IS_MSO* packet );   // Игрок отправил сообщение
    void insim_reo( struct IS_REO* packet );
    void insim_pla( struct IS_PLA* packet );
    // функции-повторители основных фунцкий ядра
    //void next_packet(); // Функция переборки типа пакета
    void insim_ncn( struct IS_NCN* packet );   // Новый игрок зашел на сервер
    void insim_npl( struct IS_NPL* packet );   // Игрок вышел из боксов
    void insim_plp( struct IS_PLP* packet );   // Игрок ушел в боксы
    void insim_pll( struct IS_PLL* packet );   // Игрок ушел в зрители
    void insim_cnl( struct IS_CNL* packet );   // Игрок ушел с сервера
    void insim_cpr( struct IS_CPR* packet );   // Игрок переименовался

    void pitlane(const char *UName);
    void cheat(struct  Player *splayer);


public:
    // Переменные и функции, доступные для всех

    RCAntCheat();   // Конструктор класса (обязательно)
    ~RCAntCheat();  // Деструктор класса (обязательно)
	struct  track_inf TrackInf;             // Where PitBox and Shop
    // Основные функции класса
    int init(char *dir,void *CInSim, void *Message);    // classname - указатель на класс RCAntCheat.
    void insim_mci( struct IS_MCI* packet );   // Пакет с данными о координатах и т.д.

};
#endif // #define _RC_TAXI_H
