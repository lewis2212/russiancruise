#ifndef _RC_LEVEL_H
#define _RC_LEVEL_H

#include "RCBaseClass.h"
#include "tools.h"
#include "RCBaseClass.h"
#include "RCMessage.h"




struct DLPlayer
{
    struct  CompCar Info;
    char    UName[32];             // Username
    char    PName[32];             // Player name
    u_int   LVL;
    u_int   Skill;
    u_int   mcicount;
    bool 	Lock;
};




class RCDL:public RCBaseClass
{
private:

		// Дескриптор соединения
    MYSQL	rcDLDB;
    // Дескриптор результирующей таблицы
    MYSQL_RES *rcDLRes;
    // Массив полей текущей строки
    MYSQL_ROW rcDLRow;

    char RootDir[MAX_PATH];

    RCMessage *msg;

    //struct  DLPlayer players[32];     // Array of players
    map<byte, DLPlayer>players;     // Array of players

    // функции-повторители основных фунцкий ядра
    void insim_ncn( struct IS_NCN* packet );
    void insim_npl( struct IS_NPL* packet );
    void insim_plp( struct IS_PLP* packet );
    void insim_pll( struct IS_PLL* packet );
    void insim_cnl( struct IS_CNL* packet );
    void insim_cpr( struct IS_CPR* packet );
    void insim_mso( struct IS_MSO* packet );
    void insim_con( struct IS_CON* packet );

    void btn_dl( byte UCID );

public:
    RCDL();
    ~RCDL();

    //
    void    save(byte UCID);

    int     GetLVL(byte UCID);
    int     GetSkill(byte UCID);
    bool    AddSkill(byte UCID);
    bool    AddSkill(byte UCID, float coef);
    bool    RemSkill(byte UCID);
    bool    RemSkill(byte UCID, float coef);
    bool    Lock(byte UCID);
    bool    Unlock(byte UCID);
    bool    Islocked(byte UCID);

    byte    inited;

    void    insim_mci( struct IS_MCI* packet );

    // Основные функции класса
    int init(const char *dir,void *CInSim, void *GetMessage);
};

#endif
