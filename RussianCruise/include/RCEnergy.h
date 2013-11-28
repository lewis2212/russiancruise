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

    void InsimNCN( struct IS_NCN* packet );
    void InsimNPL( struct IS_NPL* packet );
    void InsimPLP( struct IS_PLP* packet );
    void InsimPLL( struct IS_PLL* packet );
    void InsimCNL( struct IS_CNL* packet );
    void InsimCPR( struct IS_CPR* packet );
    void InsimMSO( struct IS_MSO* packet );
    void InsimCON( struct IS_CON* packet );
    void InsimOBH( struct IS_OBH* packet );
    void InsimHLV( struct IS_HLV* packet );

    int check_pos ( byte UCID );
public:
    RCEnergy(const char* Dir);
    ~RCEnergy();

    RCMessage   *msg;
    RCBank      *bank;

    struct  place zone;

    // Основные функции класса
    int     init(MYSQL *conn,CInsim *InSim, void *RCMessageClass,void *Bank);
    void    readconfig(const char *Track);

    void    InsimMCI( struct IS_MCI* packet );

    void    Save(byte UCID);
    void    btn_energy ( byte UCID );


    int     GetEnergy(byte UCID);
    bool    Lock(byte UCID);
    bool    Unlock(byte UCID);
    bool    Islocked(byte UCID);

    bool	AddEnergy( byte UCID, int Energy);
    bool	RemoveEnergy( byte UCID, int Energy);

};

#endif
