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
    double   Cash;
    //char    CName[4];             // Car Name
    bool    InZone;                 //зона банка
    u_int   Credit;
    time_t   Date_create;


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
    map<byte, BankPlayer>players;
    struct  place zone;
    double BankFond;

    char RootDir[MAX_PATH];
	RCMessage   *msg;
    RCDL        *dl;

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

    bool AddToBank(int Cash);
    bool RemFrBank(int Cash);

    bool InBank(byte UCID);

    bool AddCash(byte UCID, int Cash,  bool Show);
    bool RemCash(byte UCID, int Cash);
    int  GetCash(byte UCID);

    // Основные функции класса
    int init(const char *dir,void *CInSim, void *GetMessage, void *DL);
    void readconfig(const char *Track);
    void bank_save(byte UCID);

    void credit_penalty(byte UCID);

    void BtnCash( byte UCID );

    void insim_mci( struct IS_MCI* packet );

    // Функции-утилиты
    int check_pos (struct BankPlayer *splayer);
};

#endif
