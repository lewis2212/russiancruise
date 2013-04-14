#ifndef _RC_LIGHT_H // Проверка, чтобы файл подключался один раз
#define _RC_LIGHT_H

#include "RCBaseClass.h"
#include "RCMessage.h"  // Messages
#include "RCDrivingLicense.h"

#include "tools.h"      // Check_Pos  etc.

// Задаем структуру игрока

struct LghPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    Light;
    bool    Light3;
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

class RCLight:public RCBaseClass
{
private:
    // Переменные и функции, доступные только самому классу

    char RootDir[MAX_PATH]; // Полный путь до папки с программой
    char TrackName[5];
    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCDL        *dl;

    //struct  LghPlayer players[MAX_PLAYERS];     // Структура игроков
    //struct  LghPlayer *players;     // Структура игроков
    map< byte, LghPlayer > players;

    int     LightsCount;
    struct  Lights Light[30];              // Array of streets
    // struct  streets2 Street2[30];              // Array of streets

    // функции-повторители основных фунцкий ядра
    void insim_ncn( struct IS_NCN* packet );   // Новый игрок зашел на сервер
    void insim_npl( struct IS_NPL* packet );   // Игрок вышел из боксов
    void insim_plp( struct IS_PLP* packet );   // Игрок ушел в боксы
    void insim_pll( struct IS_PLL* packet );   // Игрок ушел в зрители
    void insim_cnl( struct IS_CNL* packet );   // Игрок ушел с сервера
    void insim_cpr( struct IS_CPR* packet );   // Игрок переименовался
    void insim_mso( struct IS_MSO* packet );   // Игрок отправил сообщение

    void btn_svetofor1 ( byte UCID );
    void btn_svetofor2 ( byte UCID );
    void btn_svetofor3 ( byte UCID );
    void btn_wrong_way ( byte UCID );


public:
    // Переменные и функции, доступные для всех

    RCLight();   // Конструктор класса (обязательно)
    ~RCLight();  // Деструктор класса (обязательно)


    bool IfInited;

    bool SetLight3(byte UCID,bool Key);


    // Основные функции класса
    int init(const char *dir,void *CInSim, void *Message, void *RCDLic);    // classname - указатель на класс RCStreet.
    // Нужно для доступа к классу внутри потока
    // Эта штука нужна для того чтобы отдельно запущенный поток имел доступ к классу RCStreet
    void readconfig(const char *Track); // Чтение данных о точках "Пункт назначения"

    void insim_mci( struct IS_MCI* packet );   // Пакет с данными о координатах и т.д.


};
#endif // #define _RC_STREET_H
