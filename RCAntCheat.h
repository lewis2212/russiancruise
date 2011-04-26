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

struct CarAcl
{
    char    CName[4];
    float   MaxAcelerate4060;
    float   MaxAcelerate6080;
    float   MaxAcelerate80100;
    float   MaxAcelerate100;
    float   MaxSpeed;
};

struct Player
{
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
};



// Описание класса Такси
class RCAntCheat
{
private:
    // Переменные и функции, доступные только самому классу

    char RootDir[MAX_PATH]; // Полный путь до папки с программой

public:
    // Переменные и функции, доступные для всех

    RCAntCheat::RCAntCheat();   // Конструктор класса (обязательно)
    RCAntCheat::~RCAntCheat();  // Деструктор класса (обязательно)

    char errmsg[64];
    CInsim      *insim; // Переменная-указатель на класс CInsim
    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCBank      *bank;  // Переменная-указатель на класс RCBank

    struct  Player players[32];     // Структура игроков

    // Основные функции класса
    int init(char *dir,void *classname,void *CInSim, void *Message,void *Bank);    // classname - указатель на класс RCAntCheat.
    // Нужно для доступа к классу внутри потока
    // Эта штука нужна для того чтобы отдельно запущенный поток имел доступ к классу RCAntCheat
    //void readconfig(char *Track); // Чтение данных о точках "Пункт назначения"

    // функции-повторители основных фунцкий ядра
    void next_packet(); // Функция переборки типа пакета
    void ncn();   // Новый игрок зашел на сервер
    void npl();   // Игрок вышел из боксов
    void plp();   // Игрок ушел в боксы
    void pll();   // Игрок ушел в зрители
    void cnl();   // Игрок ушел с сервера
    void crp();   // Игрок переименовался
    void mci();   // Пакет с данными о координатах и т.д.
    void mso();   // Игрок отправил сообщение

    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);
    void pitlane(char *UName);
    void cheat(struct  Player *splayer);


    // Функции-утилиты
    // int check_pos (struct PizzaPlayer *splayer); //+

};
#endif // #define _RC_TAXI_H
// TODO: Юзать .pth для "Пункт назначения" по возможности
// Привязка к нодам (Рандом)
