#ifndef _RC_POLICE_H
#define _RC_POLICE_H

#include "RCBaseClass.h"
#include "RCMessage.h"
#include "RCBank.h"
#include "RCDrivingLicense.h"
#include "RCStreet.h"
#include "RCEnergy.h"
#include "RCLight.h"

#define MAX_FINES 100

struct fine
{
    int     id;
    char    NameRu[64];
    char    NameEn[64];
    int     cash;
};

struct user_fine
{
    int     fine_id;
    int     fine_date;
    string	CopName;
    string	CopPName;
};

/** ������������ ������ **/
struct APlayer
{
    char    UName[24];
    time_t  ArestTime;
};

struct Cops
{
	char    UName[24];
    int  	Rank;
};

struct PolicePlayer
{
    /** GENERAL **/
    char    UName[24];             // Username
    char    PName[24];             // Player name
    char    CName[4];              // Car Name

    /** COP **/
    bool    cop = false;
    bool    Sirena;
    bool    Radar;

    /** ��� **/
    int		DTP;
    int		DTPstatus;
    int		DTPfines;

    /** other players **/
    int		SirenaDist;
    int    	Pogonya;
    int     StopTime = 0;

    struct  CompCar Info;
    struct 	user_fine fines[MAX_FINES];

    byte    BID;
    byte    BID2;

    int 	ThisFineCount;
    char 	ThisFine[20][200];

    int     WorkTime;
    int 	speed_over;
};

class RCPolice: public RCBaseClass
{
private:

    RCMessage   *msg;
    RCBank      *bank;
    RCDL        *dl;
    RCStreet    *street;
    RCEnergy	*nrg;
    RCLight		*lgh;

    string siren = "";

    map <byte, PolicePlayer> players; 			// ��� ������
    map <byte, APlayer> ArestPlayers;			// ������������ ������

    int DTPvyzov[3][32];	// 1 - UCID player, 2 - time, 3 - UCID cop

    void InsimNCN( struct IS_NCN* packet );   	// ����� ����� ����� �� ������
    void InsimNPL( struct IS_NPL* packet );   	// ����� ����� �� ������
    void InsimPLP( struct IS_PLP* packet );   	// ����� ���� � �����
    void InsimPLL( struct IS_PLL* packet );   	// ����� ���� � �������
    void InsimCNL( struct IS_CNL* packet );   	// ����� ���� � �������
    void InsimCPR( struct IS_CPR* packet );   	// ����� ��������������
    void InsimMSO( struct IS_MSO* packet );   	// ����� �������� ���������
    void InsimBTC( struct IS_BTC* packet );
    void InsimBTT( struct IS_BTT* packet );
    void InsimPEN( struct IS_PEN* packet );
    void InsimPLA( struct IS_PLA* packet );

    void ReadUserFines( byte UCID );
    void BtnPogonya( byte UCID );
    void ButtonClock( byte UCID );
    void ShowFinesPanel( byte UCID, byte UCID2 );
public:
    RCPolice();
    ~RCPolice();
	void readconfig();

    struct fine fines[MAX_FINES];

    int init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void *STreet, void *Energy, void *Light);

    void SaveUserFines( byte UCID );
    void SendMTCToCop(const char* Msg);
    void SetUserBID( byte UCID, byte BID);
    void ReadFines();
    bool ReadCop(byte UCID);
    void SetSirenLight( string sirenWord );
    bool IsCop(byte UCID);
    int InPursuite(byte UCID);
    int GetFineCount();
    int GetCopCount();
    char* GetFineName(byte UCID, int FineID);

    void Event();

    void InsimMCI( struct IS_MCI* packet );
};

#endif
