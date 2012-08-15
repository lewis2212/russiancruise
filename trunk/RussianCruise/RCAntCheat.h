#ifndef _RC_CHEAT_H // Проверка, чтобы файл подключался один раз
#define _RC_CHEAT_H

#include <stdio.h>      //
#include <iostream>     //
#include <time.h>       // для работы с временем и рандомом
#include <windows.h>    // не помню для чего но ингда нужно
#include <math.h>

#include "CInsim.h"     // Insim
#include "RCMessage.h"  // Messages
#include "RCBank.h"     // Bank

#include "tools.h"      // Check_Pos  etc.

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
    // Переменные и функции, доступные только самому классу

    char RootDir[MAX_PATH]; // Полный путь до папки с программой

public:
    // Переменные и функции, доступные для всех

    RCAntCheat();   // Конструктор класса (обязательно)
    ~RCAntCheat();  // Деструктор класса (обязательно)

    char errmsg[64];
    //CInsim      *insim; // Переменная-указатель на класс CInsim
    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCBank      *bank;  // Переменная-указатель на класс RCBank

    struct  Player players[32];     // Структура игроков

    // Основные функции класса
    int init(char *dir,void *classname,void *CInSim, void *Message,void *Bank);    // classname - указатель на класс RCAntCheat.
    // Нужно для доступа к классу внутри потока
    // Эта штука нужна для того чтобы отдельно запущенный поток имел доступ к классу RCAntCheat
    //void readconfig(char *Track); // Чтение данных о точках "Пункт назначения"

    // функции-повторители основных фунцкий ядра
    //void next_packet(); // Функция переборки типа пакета
    void insim_ncn();   // Новый игрок зашел на сервер
    void insim_npl();   // Игрок вышел из боксов
    void insim_plp();   // Игрок ушел в боксы
    void insim_pll();   // Игрок ушел в зрители
    void insim_cnl();   // Игрок ушел с сервера
    void insim_crp();   // Игрок переименовался
    void insim_mci();   // Пакет с данными о координатах и т.д.
    void insim_mso();   // Игрок отправил сообщение
    void insim_reo();
    void insim_pla();

    void pitlane(const char *UName);
    void cheat(struct  Player *splayer);


    // Функции-утилиты
    // int check_pos (struct PizzaPlayer *splayer); //+

};
#endif // #define _RC_TAXI_H
