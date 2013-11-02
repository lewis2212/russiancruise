using namespace std;
#ifndef _RC_BANLIST_H
#define _RC_BANLIST_H

#include "RCBaseClass.h"


struct BanPlayer
{
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
};

class RCBanList:public RCBaseClass
{
public:
    RCBanList();
    ~RCBanList();
    int init( const char* RootDir, void *CInSim );

    void removeUser( const char* username );
    void addUser( const char* username, time_t timestamp );

private:
    void InsimNCN( struct IS_NCN* packet );
};

#endif // RCBANLIST_H
