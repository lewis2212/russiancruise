#ifndef _RC_BANK_H
#define _RC_BANK_H

#include "RCBaseClass.h"
#include "tools.h"
#include "RCMessage.h"
#include "RCDrivingLicense.h"



struct BankPlayer: public GlobalPlayer
{
    double   	Cash;

    bool    	InZone;                 //���� �����
    u_int   	Credit;
    time_t  	Date_create;

    u_int   	Deposit;
    time_t   	Dep_Date_create;
};

struct Bank_info
{
    int     BankCount = 0;
    int     XBank[10];
    int     YBank[10];
};


class RCBank:public RCBaseClass
{
private:

    struct  Bank_info TrackInf;
    map<byte, BankPlayer>players;
    struct  place zone;
    double BankFond;

    char RootDir[MAX_PATH];
    RCMessage   *msg;
    RCDL        *dl;

    void InsimCNL( struct IS_CNL* packet );
    void InsimCPR( struct IS_CPR* packet );
    void InsimMCI( struct IS_MCI* packet );
    void InsimMSO( struct IS_MSO* packet );
    void InsimNCN( struct IS_NCN* packet );
    void InsimNPL( struct IS_NPL* packet );
    void InsimPLL( struct IS_PLL* packet );
    void InsimPLP( struct IS_PLP* packet);

public:
    RCBank(const char* Dir);
    ~RCBank();

    bool AddToBank(int Cash);
    bool RemFrBank(int Cash);

    bool InBank(byte UCID);

    bool AddCash(byte UCID, int Cash,  bool Show);
    bool RemCash(byte UCID, int Cash);
    int  GetCash(byte UCID);

    // �������� ������� ������
    int init(DBMySQL *db, CInsim *InSim, RCMessage *RCMessageClass, RCDL *DL);
    void ReadConfig(const char *Track);
    void Save(byte UCID);

    void credit_penalty(byte UCID);

    void Event();

};

#endif
