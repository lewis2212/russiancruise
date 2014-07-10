#ifndef _RC_STREET_H // Проверка, чтобы файл подключался один раз
#define _RC_STREET_H

#include "RCBaseClass.h"
#include "RCMessage.h"  // Messages
#include "RCBank.h"     // Bank

#include "tools.h"      // Check_Pos  etc.

#define MAX_STREETS 32
#define DEFAULT_STREET_NUM 250
// Задаем структуру игрока

struct StrPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    int     StreetNum;
};

struct streets
{
    int 	StreetID;
    char    StreetRu[50];
    char    StreetEn[50];
    word    PointCount;
    int     *StreetX = NULL;
    int     *StreetY = NULL;
    int     SpeedLimit;
};


// Описание класса Такси
class RCStreet:public RCBaseClass
{
private:
    char RootDir[MAX_PATH]; // Полный путь до папки с программой

    char errmsg[64];

    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCBank      *bank;  // Переменная-указатель на класс RCBank

    //struct  StrPlayer *players;     // Структура игроков
    map< byte, StrPlayer >players;

    // struct  streets2 Street2[30];             // Array of streets

    // функции-повторители основных фунцкий ядра
    void InsimNCN( struct IS_NCN* packet );   // Новый игрок зашел на сервер
    void InsimNPL( struct IS_NPL* packet );   // Игрок вышел из боксов
    void InsimPLP( struct IS_PLP* packet );   // Игрок ушел в боксы
    void InsimPLL( struct IS_PLL* packet );   // Игрок ушел в зрители
    void InsimCNL( struct IS_CNL* packet );   // Игрок ушел с сервера
    void InsimCPR( struct IS_CPR* packet );   // Игрок переименовался
    void InsimMSO( struct IS_MSO* packet );   // Игрок отправил сообщение

    void BtnStreet( byte UCID );

public:
    // Переменные и функции, доступные для всех

    RCStreet(const char* Dir);   // Конструктор класса (обязательно)
    ~RCStreet();  // Деструктор класса (обязательно)

    byte    StreetNums;                          // Count of streets
    struct  streets Street[ MAX_STREETS ];       // Array of streets
    Json::Value lang;

    bool IfInited;
    int CurentStreetNum(byte UCID);
    int CurentStreetInfo(void *StreetInfo, byte UCID);
    int CurentStreetInfoByNum(void *StreetInfo, int StrNum);
    int StreetCount();

    const char* GetStreetName(byte UCID, int StreetID);

    int init(MYSQL *conn,CInsim *InSim, void *Message);
    void ReadConfig(const char *Track); 	// Чтение данных о точках "Пункт назначения"

    void InsimMCI( struct IS_MCI* packet );   // Пакет с данными о координатах и т.д.
};
#endif // #define _RC_STREET_H
