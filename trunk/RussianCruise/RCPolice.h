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

struct PoliceStat
{
    int DateActive;
    int CurrentDay;
    // Day Stat
    int ArrestWithFineByDay;
    int ArrestWithOutFineByDay;
    int SolvedIncedentsByDay;
    int FinedByDay;
    int CanceledFinesByDay;
    // All Stat
    int ArrestWithFine;
    int ArrestWithOutFine;
    int SolvedIncedents;
    int Fined;
    int CanceledFines;
};

struct PolicePlayer: public GlobalPlayer
{
    /** GENERAL **/
    byte	Rank = 0;				// ������: 0 - �����, 1 - ��. ����., 2 - ����., 3 - ����. ����., 4 - �������

    /** COP **/
    bool    cop = false;
    time_t	StartWork; 	//����� ����������� �� �����
    bool    Sirena;
    bool    Radar;
    struct  PoliceStat  PStat;

    /** ��� **/
    int		DTP;
    int		DTPstatus;
    int		DTPfines;
    int		DoneCount = 0;
    int 	LastDtpTime = 0;
    bool 	blame = false;		//���� � ���
    int		FineC = 0;

    /** other players **/
    int		SirenaDist;
    int    	Pogonya;
    int     StopTime = 0;

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
    string CopUname = "";

    map <byte, PolicePlayer> players; 			// ��� ������
    map <byte, APlayer> ArestPlayers;			// ������������ ������

    int DTPvyzov[3][32];	// 1 - UCID player, 2 - time, 3 - UCID cop
    int FineAllow[5][MAX_FINES];

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
    void CopPayRoll(byte UCID, bool FullWork);
    void ReadFines();

    //Stat
    void LoadCopStat(byte UCID);
    void SaveCopStat(byte UCID);
    void ShowCopStat(byte UCID);
public:
    RCPolice(const char* Dir);
    ~RCPolice();
	void readconfig();

    struct fine fines[MAX_FINES];

    int init(MYSQL *conn,CInsim *InSim, void *Message,void *Bank,void *RCdl, void *STreet, void *Energy, void *Light);

    void SaveUserFines( byte UCID );
    void SendMTCToCop(const char* Msg, int Rank, ...);
    void SetUserBID( byte UCID, byte BID);
    void SetSirenLight( string sirenWord );
    bool IsCop(byte UCID);
    int GetCopRank(byte UCID);
    int GetCopDTPstatus(byte UCID);
    int InPursuite(byte UCID);
    int GetFineCount();
    int GetCopCount();
    char* GetFineName(byte UCID, int FineID);

    void Event();

    void InsimMCI( struct IS_MCI* packet );
};

#endif
