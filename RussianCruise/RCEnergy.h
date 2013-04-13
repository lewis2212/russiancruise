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
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
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

    pthread_t tid; // Thread ID
    pthread_attr_t attr;
    time_t nrgtime;
    struct  energy_info TrackInf;             // Where PitBox and Shop

    char RootDir[MAX_PATH];

    void insim_ncn( struct IS_NCN* packet );
    void insim_npl( struct IS_NPL* packet );
    void insim_plp( struct IS_PLP* packet );
    void insim_pll( struct IS_PLL* packet );
    void insim_cnl( struct IS_CNL* packet );
    void insim_cpr( struct IS_CPR* packet );
    void insim_mso( struct IS_MSO* packet );
    //! HIT CONTROL
    void insim_con( struct IS_CON* packet );
    void insim_obh( struct IS_OBH* packet );


    // Функции-утилиты
    int check_pos (struct EnergyPlayer *splayer); //+

public:
    RCEnergy();
    ~RCEnergy();

    RCMessage   *msg;
    RCBank      *bank;

    struct  place zone;
    struct  EnergyPlayer *players;     // Array of players
    // Основные функции класса
    int     init(const char *dir,void *CInSim, void *GetMessage,void *Bank);
    void    readconfig(const char *Track);
    // функции-повторители основных фунцкий ядра

    void    insim_mci( struct IS_MCI* packet );

    void    energy_save(byte UCID);
    void    btn_energy (struct EnergyPlayer *splayer);


    int     GetEnergy(byte UCID);
    bool    Lock(byte UCID);
    bool    Unlock(byte UCID);
    bool    Islocked(byte UCID);

};

#endif
