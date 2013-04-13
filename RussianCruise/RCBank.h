#ifndef _RC_BANK_H
#define _RC_BANK_H

#include "RCBaseClass.h"
#include "tools.h"
#include "RCMessage.h"
#include "RCDrivingLicense.h"




typedef struct BankPlayer
{
    struct  CompCar Info;
    char    UName[24];              // Username
    char    PName[24];              // Player name
    byte    UCID;                   // Connection ID
    byte    PLID;                   // PLayer ID
    double   Cash;
    //char    CName[4];             // Car Name
    byte    Zone;

    bool    InZone;                 //зона банка
    u_int   Credit;
    u_int   Date_create;


} _BankPlayer;

struct Bank_info
{
    int     BankCount = 0;
    int     XBank[10];
    int     YBank[10];
};


class RCBank:public RCBaseClass
{
private:

    // Дескриптор соединения
    MYSQL	rcbankDB;
    // Дескриптор результирующей таблицы
    MYSQL_RES *rcbankRes;
    // Массив полей текущей строки
    MYSQL_ROW rcbankRow;


    struct  Bank_info TrackInf;
    BankPlayer players[MAX_PLAYERS];     // Array of players
    struct  place zone;
    double BankFond;

    char RootDir[MAX_PATH];

    void insim_ncn( struct IS_NCN* packet );
    void insim_npl( struct IS_NPL* packet );
    void insim_plp( struct IS_PLP* packet);
    void insim_pll( struct IS_PLL* packet );
    void insim_cnl( struct IS_CNL* packet );
    void insim_cpr( struct IS_CPR* packet );
    void insim_mso( struct IS_MSO* packet );

public:
    RCBank();
    ~RCBank();

    char errmsg[64];
    RCMessage   *msg;
    RCDL        *dl;

    bool AddToBank(int Cash);
    bool RemFrBank(int Cash);

    bool InBank(byte UCID);

    bool AddCash(byte UCID, int Cash,  bool Show);
    bool RemCash(byte UCID, int Cash);
    int  GetCash(byte UCID);
    byte GetPlayerUCID (int i);

    // Основные функции класса
    int init(const char *dir,void *CInSim, void *GetMessage, void *dbconn, void *DL);
    void readconfig(const char *Track);
    void bank_save(byte UCID);

    void btn_cash(int i);

    void insim_mci( struct IS_MCI* packet );

    // Функции-утилиты
    int check_pos (struct BankPlayer *splayer);
};

#endif
