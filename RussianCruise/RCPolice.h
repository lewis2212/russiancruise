#ifndef _RC_POLICE_H
#define _RC_POLICE_H

#include "RCBaseClass.h"

#include "RCMessage.h"  // Messages
#include "RCBank.h"     // Bank
#include "RCDrivingLicense.h"
#include "RCStreet.h"
#include "RCEnergy.h"
#include "RCLight.h"

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
    string	CopName;
};

struct PolicePlayer
{
    struct  CompCar Info;
    struct user_fine fines[MAX_FINES];
    /** GENERAL **/
    char    UName[24];             // Username
    char    PName[24];             // Player name
    char    CName[4];              // Car Name

    byte    BID;
    byte    BID2;

	/** COP **/
    bool    cop;
    byte    radar;
    byte    sirena;         // ��������� ����������� ������
    byte    sirenaOnOff;    // ���������� ������ ��������� ������ � �����
    byte    sirenaKey;      // ���������� �������� ��� ��������� ������ � �����
    int     sirenaSize;      // ������ ������
    byte    Pogonya;
    char    PogonyaReason[64];
    int     StopTime;

    int     WorkTime;			// ����� �� ������� �� ������ ��������� �����
};

class RCPolice: public RCBaseClass
{
private:

    RCMessage   *msg;   // ����������-��������� �� ����� RCMessage
    RCBank      *bank;  // ����������-��������� �� ����� RCBank
    RCDL        *dl;
    RCStreet    *street;
    RCEnergy	*nrg;
    RCLight		*lgh;

	string siren = "^0";
	struct  fine fines[MAX_FINES];
	map<byte, PolicePlayer>players;

	void InsimNCN( struct IS_NCN* packet );   // ����� ����� ����� �� ������
    void InsimNPL( struct IS_NPL* packet );   // ����� ����� �� ������
    void InsimPLP( struct IS_PLP* packet );   // ����� ���� � �����
    void InsimPLL( struct IS_PLL* packet );   // ����� ���� � �������
    void InsimCNL( struct IS_CNL* packet );   // ����� ���� � �������
    void InsimCPR( struct IS_CPR* packet );   // ����� ��������������
    void InsimMSO( struct IS_MSO* packet );   // ����� �������� ���������
    void InsimCON( struct IS_CON* packet );   // ����� �������� ���������
    void InsimOBH( struct IS_OBH* packet );   // ����� �������� ���������
    void InsimBTC( struct IS_BTC* packet );
    void InsimBTT( struct IS_BTT* packet );
    void InsimPEN( struct IS_PEN* packet );
    void InsimPLA( struct IS_PLA* packet );


    void ReadUserFines( byte UCID );
    void BtnSirena( byte UCID );
    void BtnPogonya( byte UCID );
    void ButtonClock( byte UCID );


public:
	RCPolice();
	~RCPolice();

	int init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void *STreet, void *Energy, void *Light);

	void SaveUserFines( byte UCID );
	void SetUserBID( byte UCID, byte BID);
	void ReadFines();
	void CopTurnOn( byte UCID );
	void CopTurnOff( byte UCID );
	void SetSirenLight( string sirenWord );
	int IfCop ( byte UCID );
	bool IsCop( byte UCID );
	int InPursuite( byte UCID );

	void Event();

	void InsimMCI( struct IS_MCI* packet );
};

#endif
