#ifndef _RC_ENERGY_H
#define _RC_ENERGY_H

#include "RCBaseClass.h"
#include "tools.h"
#include "RCMessage.h"
#include "RCBank.h"

#include "tools.h"

struct EnergyPlayer: public GlobalPlayer
{
    byte    Zone;

    /** Energy **/
    int     Energy; // 0-10000
    int     leftToFeel = 0;
    int 	feelStep = 0;
    time_t  EnergyTime;
    bool	EnergyAlarm = false;
    bool	Lock;

    time_t  LastT;
};

struct energy_info
{
    int     CafeCount;
    int     XCafe[10];
    int     YCafe[10];
};

class RCEnergy:public RCBaseClass
{
private:

    time_t nrgtime;
    struct  energy_info TrackInf;       // Where PitBox and Shop

    map<byte, EnergyPlayer>players;     // Array of players

    void InsimCNL( struct IS_CNL* packet );
    void InsimCON( struct IS_CON* packet );
    void InsimCPR( struct IS_CPR* packet );
    void InsimHLV( struct IS_HLV* packet );
    void InsimMCI( struct IS_MCI* packet );
    void InsimMSO( struct IS_MSO* packet );
    void InsimNCN( struct IS_NCN* packet );
    void InsimNPL( struct IS_NPL* packet );
    void InsimOBH( struct IS_OBH* packet );
    void InsimPLL( struct IS_PLL* packet );
    void InsimPLP( struct IS_PLP* packet );

public:
    RCEnergy(const char* Dir);
    ~RCEnergy();

    RCMessage   *msg;
    RCBank      *bank;

    struct  place zone;

    // �������� ������� ������
    int     init(DBMySQL *db,CInsim *InSim, void *RCMessageClass,void *Bank);
    void    ReadConfig(const char *Track);


    void    Save(byte UCID);

    int     GetEnergy(byte UCID);
    bool    Lock(byte UCID);
    bool    Unlock(byte UCID);
    bool    Islocked(byte UCID);

    bool	AddEnergy( byte UCID, int Energy);
    bool	RemoveEnergy( byte UCID, int Energy);

    bool    InCafe ( byte UCID );

    void    Event();
};

#endif
