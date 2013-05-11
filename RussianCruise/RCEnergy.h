#ifndef _RC_ENERGY_H
#define _RC_ENERGY_H

#include "RCBaseClass.h"
#include "tools.h"
#include "RCMessage.h"
#include "RCBank.h"

#include "tools.h"

struct EnergyPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    Zone;
    /** Energy **/
    int     Energy; // Energy xD from 0 to 10000
    time_t  EnergyTime;

    bool	Lock;
};

struct energy_info
{
    int     CafeCount;
    int     XCafe[10];
    int     YCafe[10];
};


class RCEnergy:public RCBaseClass
{
private:

	// Дескриптор соединения
    MYSQL	rcNrgDB;
    // Дескриптор результирующей таблицы
    MYSQL_RES *rcNrgRes;
    // Массив полей текущей строки
    MYSQL_ROW rcNrgRow;

    time_t nrgtime;
    struct  energy_info TrackInf;             // Where PitBox and Shop

	map<byte, EnergyPlayer>players;     // Array of players

    char RootDir[MAX_PATH];

    void InsimNCN( struct IS_NCN* packet );
    void InsimNPL( struct IS_NPL* packet );
    void InsimPLP( struct IS_PLP* packet );
    void InsimPLL( struct IS_PLL* packet );
    void InsimCNL( struct IS_CNL* packet );
    void InsimCPR( struct IS_CPR* packet );
    void InsimMSO( struct IS_MSO* packet );
    //! HIT CONTROL
    void InsimCON( struct IS_CON* packet );
    void InsimOBH( struct IS_OBH* packet );


    // Функции-утилиты
    int check_pos ( byte UCID ); //+

public:
    RCEnergy();
    ~RCEnergy();

    RCMessage   *msg;
    RCBank      *bank;

    struct  place zone;

    // Основные функции класса
    int     init(const char *dir,void *CInSim, void *RCMessageClass,void *Bank);
    void    readconfig(const char *Track);
    // функции-повторители основных фунцкий ядра

    void    InsimMCI( struct IS_MCI* packet );

    void    energy_save(byte UCID);
    void    btn_energy ( byte UCID );


    int     GetEnergy(byte UCID);
    bool    Lock(byte UCID);
    bool    Unlock(byte UCID);
    bool    Islocked(byte UCID);

    bool	AddEnergy( byte UCID, int Energy);
    bool	RemoveEnergy( byte UCID, int Energy);

};

#endif
