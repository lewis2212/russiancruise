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

struct Bank_info
{
    int     BankCount;
    int     XBank[10];
    int     YBank[10];
};


class RCBank
{
private:

    struct  Bank_info TrackInf;
    struct  BankPlayer players[32];     // Array of players
    struct  place zone;

    float BankFond;

    char RootDir[MAX_PATH];

    void bank_ncn();
    void bank_npl();
    void bank_plp();
    void bank_pll();
    void bank_cnl();
    void bank_crp();
    void bank_mci();
    void bank_mso();


    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);

public:
    RCBank();
    ~RCBank();

    char errmsg[64];
    CInsim  *insim;
    RCMessage *msg;



    bool AddToBank(int Cash);
    bool RemFrBank(int Cash);

    bool AddCash(byte UCID, int Cash);
    bool RemCash(byte UCID, int Cash);
    int  GetCash(byte UCID);
    byte GetPlayerUCID (int i);



    // Основные функции класса
    int init(const char *dir,void *CInSim, void *GetMessage,void *Bank);
    void readconfig(const char *Track);
    void bank_save(byte UCID);
    // функции-повторители основных фунцкий ядра
    void next_packet();
    void btn_cash(int i);






    // функции-повторители основных фунцкий ядра


    // Функции-утилиты
    int check_pos (struct BankPlayer *splayer); //+

};

#endif
