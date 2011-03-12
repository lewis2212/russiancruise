#ifndef _RC_LEVEL_H
#define _RC_LEVEL_H

//typedef unsigned char byte;
//typedef unsigned short word;

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <windows.h>
#include <math.h>

#include "tools.h"
#include "CInsim.h"
#include "RCMessage.h"




struct DLPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    u_int     LVL;
    u_int     Skill;
    byte    mcicount;
};




class RCDL
{
private:
    char RootDir[MAX_PATH];

    CInsim  *insim;
    RCMessage *msg;

    // функции-повторители основных фунцкий ядра
    void ncn();
    void npl();
    void plp();
    void pll();
    void cnl();
    void crp();
    void mso();

    //

    void btn_dl(struct DLPlayer *splayer);

    // функции-повторители основных фунцкий ядра

    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);

    // Функции-утилиты
    int check_pos (struct BankPlayer *splayer); //+

public:
    RCDL::RCDL();
    RCDL::~RCDL();

    //
    void save(byte UCID);

    int     GetLVL(byte UCID);
    int     GetSkill(byte UCID);
    bool    AddSkill(byte UCID);
    bool    RemSkill(byte UCID);

    byte inited;

    void next_packet();
    void mci();

    struct  DLPlayer players[32];     // Array of players
    // Основные функции класса
    int init(char *dir,void *CInSim, void *GetMessage);
};

#endif