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
    bool 	Lock;
};




class RCDL:public RCBaseClass
{
private:
    MYSQL	rcDLDB;
    MYSQL_RES *rcDLRes;
    MYSQL_ROW rcDLRow;

    char RootDir[MAX_PATH];

    RCMessage *msg;

    map<byte, DLPlayer>players;     // Array of players

    // функции-повторители основных фунцкий ядра
    void InsimNCN( struct IS_NCN* packet );
    void InsimNPL( struct IS_NPL* packet );
    void InsimPLP( struct IS_PLP* packet );
    void InsimPLL( struct IS_PLL* packet );
    void InsimCNL( struct IS_CNL* packet );
    void InsimCPR( struct IS_CPR* packet );
    void InsimMSO( struct IS_MSO* packet );
    void InsimCON( struct IS_CON* packet );

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

    void    InsimMCI( struct IS_MCI* packet );

    // Основные функции класса
    int init(const char *dir,void *CInSim, void *RCMessageClass);
};

#endif
