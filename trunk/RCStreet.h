#ifndef _RC_STREET_H // Проверка, чтобы файл подключался один раз
#define _RC_STREET_H

#include <stdio.h>      //
#include <iostream>     //
#include <time.h>       //
#include <windows.h>    //

#include "CInsim.h"     // Insim
#include "RCMessage.h"  // Messages
#include "RCBank.h"     // Bank

#include "tools.h"      // Check_Pos  etc.

// Задаем структуру игрока

struct StrPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    int     StreetNum;
};

struct streets
{
    char    Street[50];
    word    PointCount;
    int     *StreetX;
    int     *StreetY;
    int     SpeedLimit;
};


// Описание класса Такси
class RCStreet
{
private:
    // Переменные и функции, доступные только самому классу

    char RootDir[MAX_PATH]; // Полный путь до папки с программой

    char errmsg[64];
    CInsim      *insim; // Переменная-указатель на класс CInsim
    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCBank      *bank;  // Переменная-указатель на класс RCBank

    struct  StrPlayer players[32];     // Структура игроков
    byte    StreetNums;                          // Count of streets
    struct  streets Street[30];              // Array of streets
    // struct  streets2 Street2[30];              // Array of streets

    // функции-повторители основных фунцкий ядра
    void street_ncn();   // Новый игрок зашел на сервер
    void street_npl();   // Игрок вышел из боксов
    void street_plp();   // Игрок ушел в боксы
    void street_pll();   // Игрок ушел в зрители
    void street_cnl();   // Игрок ушел с сервера
    void street_crp();   // Игрок переименовался

    void street_mso();   // Игрок отправил сообщение

    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);

    void btn_street(struct StrPlayer *splayer);

public:
    // Переменные и функции, доступные для всех

    RCStreet::RCStreet();   // Конструктор класса (обязательно)
    RCStreet::~RCStreet();  // Деструктор класса (обязательно)


    bool IfInited;
    int CurentStreetNum(byte UCID);
    int CurentStreetInfo(void *StreetInfo, byte UCID);
    int CurentStreetInfoByNum(void *StreetInfo, int StrNum);
    int StreetCount();



    // Основные функции класса
    int init(char *dir,void *CInSim, void *Message);    // classname - указатель на класс RCStreet.
    // Нужно для доступа к классу внутри потока
    // Эта штука нужна для того чтобы отдельно запущенный поток имел доступ к классу RCStreet
    void readconfig(char *Track); // Чтение данных о точках "Пункт назначения"

    void street_mci();   // Пакет с данными о координатах и т.д.
    void next_packet(); // Функция переборки типа пакета

};
#endif // #define _RC_STREET_H
