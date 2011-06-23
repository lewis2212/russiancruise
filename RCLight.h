#ifndef _RC_LIGHT_H // Проверка, чтобы файл подключался один раз
#define _RC_LIGHT_H

#include <stdio.h>      //
#include <iostream>     //
#include <time.h>       //
#include <windows.h>    //

#include "CInsim.h"     // Insim
#include "RCMessage.h"  // Messages

#include "tools.h"      // Check_Pos  etc.

// Задаем структуру игрока

struct LghPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    byte    Light;
     /** misc **/
    byte	WrongWay;
};

struct Lights
{
    byte    ID;
    int     Heading;
    int     PointCount;
    int     *X;
    int     *Y;
};

// Описание класса Такси
class RCLight
{
private:
    // Переменные и функции, доступные только самому классу

    char RootDir[MAX_PATH]; // Полный путь до папки с программой

    char errmsg[64];
    CInsim      *insim; // Переменная-указатель на класс CInsim
    RCMessage   *msg;   // Переменная-указатель на класс RCMessage

    struct  LghPlayer players[32];     // Структура игроков
    int     LightsCount;
    struct  Lights Light[30];              // Array of streets
    // struct  streets2 Street2[30];              // Array of streets

    // функции-повторители основных фунцкий ядра
    void ncn();   // Новый игрок зашел на сервер
    void npl();   // Игрок вышел из боксов
    void plp();   // Игрок ушел в боксы
    void pll();   // Игрок ушел в зрители
    void cnl();   // Игрок ушел с сервера
    void crp();   // Игрок переименовался

    void mso();   // Игрок отправил сообщение

    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);
    void btn_svetofor1 (struct LghPlayer *splayer);
    void btn_svetofor2 (struct LghPlayer *splayer);
    void btn_svetofor3 (struct LghPlayer *splayer);
    void btn_wrong_way (byte UCID);


public:
    // Переменные и функции, доступные для всех

    RCLight::RCLight();   // Конструктор класса (обязательно)
    RCLight::~RCLight();  // Деструктор класса (обязательно)


    bool IfInited;


    // Основные функции класса
    int init(char *dir,void *CInSim, void *Message);    // classname - указатель на класс RCStreet.
    // Нужно для доступа к классу внутри потока
    // Эта штука нужна для того чтобы отдельно запущенный поток имел доступ к классу RCStreet
    void readconfig(char *Track); // Чтение данных о точках "Пункт назначения"

    void mci();   // Пакет с данными о координатах и т.д.
    void next_packet(); // Функция переборки типа пакета

};
#endif // #define _RC_STREET_H
