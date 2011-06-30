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


class RCEnergy
{
private:
    pthread_t tid; // Thread ID
    pthread_attr_t attr;
    time_t nrgtime;
    struct  energy_info TrackInf;             // Where PitBox and Shop

    char RootDir[MAX_PATH];

    void ncn();
    void npl();
    void plp();
    void pll();
    void cnl();
    void crp();
    void mso();
    //! HIT CONTROL
    void con();
    void obh();
    void hlv();

    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);
    void btn_work (struct BankPlayer *splayer);

    // Функции-утилиты
    int check_pos (struct EnergyPlayer *splayer); //+

public:
    RCEnergy::RCEnergy();
    RCEnergy::~RCEnergy();

    char errmsg[64];
    CInsim      *insim;
    RCMessage   *msg;
    RCBank      *bank;

    struct  place zone;
    struct  EnergyPlayer players[32];     // Array of players
    // Основные функции класса
    int init(char *dir,void *classname,void *CInSim, void *GetMessage,void *Bank);
    void readconfig(char *Track);
    // функции-повторители основных фунцкий ядра
    void next_packet();
    void mci();
    void energy_save(byte UCID);
    void btn_energy (struct EnergyPlayer *splayer);

    int GetEnergy(byte UCID);
    bool    Lock(byte UCID);
    bool    Unlock(byte UCID);
    bool    Islocked(byte UCID);

};

#endif
