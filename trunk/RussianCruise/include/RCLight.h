#ifndef _RC_LIGHT_H // Проверка, чтобы файл подключался один раз
#define _RC_LIGHT_H

#include "RCBaseClass.h"
#include "RCMessage.h"  // Messages
#include "RCDrivingLicense.h"

#include "tools.h"      // Check_Pos  etc.

struct LghPlayer: GlobalPlayer
{
    byte    Light;
    bool    Light3;
    /** misc **/
    byte	WrongWay;
    bool	RedLight;
    bool	GreenLight;
    byte 	LightNum;
    byte 	DiffDir;
    bool 	OnRed = false; //проехал ли на красный
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
    char RootDir[MAX_PATH]; // Полный путь до папки с программой
    char TrackName[5];
    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCDL        *dl;

    map< byte, LghPlayer > players;

    int     LightsCount;
    // struct  Lights Light[30];
    vector<Lights> Light;

    bool	LightWorks=true;

    int red1, red2, yell1, yell2, green1, green2;
    bool gff=false;

    time_t sstime;

    void InsimCNL( struct IS_CNL* packet );   // Игрок ушел с сервера
    void InsimCPR( struct IS_CPR* packet );   // Игрок переименовался
    void InsimMCI( struct IS_MCI* packet );   // Пакет с данными о координатах и т.д.
    void InsimMSO( struct IS_MSO* packet );   // Игрок отправил сообщение
    void InsimNCN( struct IS_NCN* packet );   // Новый игрок зашел на сервер
    void InsimNPL( struct IS_NPL* packet );   // Игрок вышел из боксов
    void InsimPLL( struct IS_PLL* packet );   // Игрок ушел в зрители
    void InsimPLP( struct IS_PLP* packet );   // Игрок ушел в боксы


    void Svetofor1 ( byte UCID );
    void Svetofor2 ( byte UCID );
    void Svetofor3 ( byte UCID );
    void WrongWay ( byte UCID );


public:
    RCLight(const char* Dir);   // Конструктор класса (обязательно)
    ~RCLight();  // Деструктор класса (обязательно)

    bool SetLight3(byte UCID,bool Key);
    void Event();

    bool GetOnLight(byte UCID);
    void OnRedFalse(byte UCID);
    bool CheckOnRed(byte UCID);
    int CheckLight(byte UCID);

    int init(DBMySQL *db,CInsim *InSim, void *Message, void *RCDLic);    // classname - указатель на класс RCStreet.
    void ReadConfig(const char *Track); // Чтение данных о точках "Пункт назначения"
};
#endif // #define _RC_STREET_H
