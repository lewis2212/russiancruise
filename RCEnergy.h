#ifndef _RC_ENERGY_H
#define _RC_ENERGY_H

//typedef unsigned char byte;
//typedef unsigned short word;


#include <math.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <windows.h>

#include "tools.h"
#include "CInsim.h"
#include "RCMessage.h"
#include "RCBank.h"


struct EnergyPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    byte    Zone;
    /** Energy **/
    int     Energy; // Energy xD
    time_t  EnergyTime;
};

struct energy_info
{
    int     CafeCount;
    int     XCafe[10];
    int     YCafe[10];
};


class RCEnergy
{
private:
    time_t nrgtime;
    struct  energy_info TrackInf;             // Where PitBox and Shop
     pthread_t tid; // Thread ID
    pthread_attr_t attr;
public:
    RCEnergy::RCEnergy();
    RCEnergy::~RCEnergy();

    CInsim      *insim;
    RCMessage   *msg;
    RCBank      *bank;

    struct  place zone;
    struct  EnergyPlayer players[32];     // Array of players
    // Основные функции класса
    int init(void *classname,void *CInSim, void *Message,void *Bank);
    void readconfig(char *Track);
     // функции-повторители основных фунцкий ядра
    void energy_ncn();
    void energy_npl();
    void energy_plp();
    void energy_pll();
    void energy_cnl();
    void energy_crp();
    void energy_mci();
    void energy_mso();


    // функции-повторители основных фунцкий ядра
    void btn_energy (struct EnergyPlayer *splayer);
    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);
    void btn_work (struct BankPlayer *splayer);

    // Функции-утилиты
    int check_pos (struct EnergyPlayer *splayer); //+

};

#endif
