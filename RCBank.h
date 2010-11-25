#ifndef _RC_BANK_H
#define _RC_BANK_H

//typedef unsigned char byte;
//typedef unsigned short word;

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <windows.h>

#include "tools.h"
#include "CInsim.h"
#include "RCMessage.h"




struct BankPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    float   Cash;
    //char    CName[4];              // Car Name
    byte    Zone;
};




class RCBank
{
private:

    char RootDir[MAX_PATH];

public:
    RCBank::RCBank();
    RCBank::~RCBank();

    CInsim  *insim;
    RCMessage *msg;

    struct  place zone;
    struct  BankPlayer players[32];     // Array of players
    // Основные функции класса
    int init(char *dir,void *CInSim, void *Message);
    void readconfig(char *Track);
     // функции-повторители основных фунцкий ядра
    void bank_ncn();
    void bank_npl();
    void bank_plp();
    void bank_pll();
    void bank_cnl();
    void bank_crp();
    void bank_mci();
    void bank_mso();


    // функции-повторители основных фунцкий ядра
    void bank_save(int j);
    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);
    void btn_cash(int i);

    // Функции-утилиты
    int check_pos (struct BankPlayer *splayer); //+

};

#endif
