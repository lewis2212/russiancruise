#ifndef _RC_LEVEL_H
#define _RC_LEVEL_H

#include "RCBaseClass.h"
#include "tools.h"
#include "RCBaseClass.h"
#include "RCMessage.h"




struct DLPlayer: public GlobalPlayer
{
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
    bool InsimNCN( struct IS_NCN* packet );
    bool InsimNPL( struct IS_NPL* packet );
    bool InsimPLP( struct IS_PLP* packet );
    bool InsimPLL( struct IS_PLL* packet );
    bool InsimCNL( struct IS_CNL* packet );
    bool InsimCPR( struct IS_CPR* packet );
    bool InsimMSO( struct IS_MSO* packet );
    bool InsimCON( struct IS_CON* packet );

public:
    RCDL(const char* Dir);
    ~RCDL();

    //
    bool Save(byte UCID);

    int  GetLVL(byte UCID);
    int  GetSkill(byte UCID);
    bool AddSkill(byte UCID);
    bool AddSkill(byte UCID, float coef);
    bool RemSkill(byte UCID);
    bool RemSkill(byte UCID, float coef);
    bool Lock(byte UCID);
    bool Unlock(byte UCID);
    bool Islocked(byte UCID);

    byte inited;

    bool InsimMCI( struct IS_MCI* packet );
    bool Event();

    // Основные функции класса
    int init(MYSQL *conn,CInsim *InSim, void *RCMessageClass);
};

#endif
