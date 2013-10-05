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

/** арестованные игроки **/
struct APlayer
{
	char    UName[24];
    time_t  ArestTime;
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

    RCMessage   *msg;   // Переменная-указатель на класс RCMessage
    RCBank      *bank;  // Переменная-указатель на класс RCBank
    RCDL        *dl;
    RCStreet    *street;
    RCEnergy	*nrg;
    RCLight		*lgh;

	string siren = "^0";
	map <byte, PolicePlayer> players;

	map <byte, APlayer> ArestPlayers;

	void InsimNCN( struct IS_NCN* packet );   // Новый игрок зашел на сервер
    void InsimNPL( struct IS_NPL* packet );   // Игрок вышел из боксов
    void InsimPLP( struct IS_PLP* packet );   // Игрок ушел в боксы
    void InsimPLL( struct IS_PLL* packet );   // Игрок ушел в зрители
    void InsimCNL( struct IS_CNL* packet );   // Игрок ушел с сервера
    void InsimCPR( struct IS_CPR* packet );   // Игрок переименовался
    void InsimMSO( struct IS_MSO* packet );   // Игрок отправил сообщение
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

	struct fine fines[MAX_FINES];

	int init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void *STreet, void *Energy, void *Light);

	void SaveUserFines( byte UCID );
	void SetUserBID( byte UCID, byte BID);
	void ReadFines();
	bool ReadCop(byte UCID);
	void SetSirenLight( string sirenWord );
	bool IsCop(byte UCID);
	int InPursuite(byte UCID);
	int GetFineCount();
    char* GetFineName(byte UCID, int FineID);

	void Event();

	void InsimMCI( struct IS_MCI* packet );
};

#endif
