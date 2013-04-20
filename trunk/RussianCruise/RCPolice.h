#ifndef _RC_POLICE_H
#define _RC_POLICE_H

#include "RCBaseClass.h"

#include "RCMessage.h"  // Messages
#include "RCBank.h"     // Bank
#include "RCDrivingLicense.h"
#include "RCStreet.h"

#define MAX_FINES 100

struct fine
{
    int     id;
    char    name[64];
    int     cash;
};

struct user_fine
{
    int     fine_id;
    int     fine_date;
};

// struct  fine fines[100];                 // Array of fines (for cops)

struct PolicePlayer
{
    struct  CompCar Info;
    struct user_fine fines[MAX_FINES];
    /** GENERAL **/
    char    UName[24];             // Username
    char    PName[24];             // Player name
    char    CName[4];              // Car Name
	/** COP **/
    byte    cop;
    byte    radar;
    byte    sirena;         // ��������� ����������� ������
    byte    sirenaOnOff;    // ���������� ������ ��������� ������ � �����
    byte    sirenaKey;      // ���������� �������� ��� ��������� ������ � �����
    int     sirenaSize;      // ������ ������
    byte    Pogonya;
    char    PogonyaReason[64];
    int     StopTime;
    byte    Penalty;        // ���� �������� �������� � �����
};

class RCPolice: public RCBaseClass
{
private:

    RCMessage   *msg;   // ����������-��������� �� ����� RCMessage
    RCBank      *bank;  // ����������-��������� �� ����� RCBank
    RCDL        *dl;
    RCStreet    *street;

	struct  fine fines[MAX_FINES];
	map<byte, PolicePlayer>players;

	void insim_ncn( struct IS_NCN* packet );   // ����� ����� ����� �� ������
    void insim_npl( struct IS_NPL* packet );   // ����� ����� �� ������
    void insim_plp( struct IS_PLP* packet );   // ����� ���� � �����
    void insim_pll( struct IS_PLL* packet );   // ����� ���� � �������
    void insim_cnl( struct IS_CNL* packet );   // ����� ���� � �������
    void insim_cpr( struct IS_CPR* packet );   // ����� ��������������
    void insim_mso( struct IS_MSO* packet );   // ����� �������� ���������
    void insim_con( struct IS_CON* packet );   // ����� �������� ���������
    void insim_obh( struct IS_OBH* packet );   // ����� �������� ���������
    void insim_btt( struct IS_BTT* packet );
    void insim_pen( struct IS_PEN* packet );
    void insim_pla( struct IS_PLA* packet );


    void ReadUserFines( byte UCID );


public:
	RCPolice();
	~RCPolice();

	int init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet);

	void SaveUserFines( byte UCID );

	void ReadFines();
};

#endif
