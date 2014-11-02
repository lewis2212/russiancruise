#ifndef _RC_LEVEL_H
#define _RC_LEVEL_H

#include "RCBaseClass.h"
#include "tools.h"
#include "RCBaseClass.h"
#include "RCMessage.h"




struct DLPlayer : GlobalPlayer
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

    RCMessage *msg;

    map<byte, DLPlayer>players;     // Array of players

    // функции-повторители основных фунцкий ядра
    void InsimCNL( struct IS_CNL* packet );
    void InsimCON( struct IS_CON* packet );
    void InsimCPR( struct IS_CPR* packet );
    void InsimMCI( struct IS_MCI* packet );
    void InsimMSO( struct IS_MSO* packet );
    void InsimNCN( struct IS_NCN* packet );
    void InsimNPL( struct IS_NPL* packet );
    void InsimPLL( struct IS_PLL* packet );
    void InsimPLP( struct IS_PLP* packet );

public:
    RCDL(const char* Dir);
    ~RCDL();

    //
    void    Save(byte UCID);

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

    void    Event();

    // Основные функции класса
    int init(MYSQL *conn,CInsim *InSim, void *RCMessageClass);
};

#endif