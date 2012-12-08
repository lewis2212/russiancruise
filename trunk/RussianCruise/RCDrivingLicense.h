#ifndef _RC_LEVEL_H
#define _RC_LEVEL_H

//typedef unsigned char byte;
//typedef unsigned short word;

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <windows.h>
#include <math.h>

#include "RCBaseClass.h"
#include "tools.h"
#include "RCBaseClass.h"
#include "RCMessage.h"




struct DLPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    u_int   LVL;
    u_int   Skill;
    u_int   mcicount;
    bool 	Lock;
};




class RCDL:public RCBaseClass
{
private:
    char RootDir[MAX_PATH];

    char errmsg[64];
    RCMessage *msg;

    // функции-повторители основных фунцкий ядра
    void insim_ncn();
    void insim_npl();
    void insim_plp();
    void insim_pll();
    void insim_cnl();
    void insim_crp();
    void insim_mso();
    void insim_con();

    void btn_dl(struct DLPlayer *splayer);

    int check_pos (struct BankPlayer *splayer); //+

public:
    RCDL();
    ~RCDL();

    //
    void    save(byte UCID);

    int     GetLVL(byte UCID);
    int     GetSkill(byte UCID);
    bool    AddSkill(byte UCID);
    bool    RemSkill(byte UCID);
    bool    Lock(byte UCID);
    bool    Unlock(byte UCID);
    bool    Islocked(byte UCID);

    byte    inited;

    void    insim_mci();

    struct  DLPlayer players[32];     // Array of players
    // Основные функции класса
    int init(const char *dir,void *CInSim, void *GetMessage);
};

#endif
