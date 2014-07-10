#ifndef _RC_MESSAGE_H
#define _RC_MESSAGE_H

#include "RCBaseClass.h"
#include "tools.h"

#include <dirent.h>

struct MPlayer: public GlobalPlayer
{
    string Lang;
};

class RCMessage:public RCBaseClass
{
private:

    char RootDir[MAX_PATH];

    map< string , map< string ,string> >MsgArray;

    map<byte, MPlayer>players;

    bool InsimNCN( struct IS_NCN* packet );
    bool InsimCNL( struct IS_CNL* packet );
    bool InsimMSO( struct IS_MSO* packet );

public:
    RCMessage(const char* Dir);
    ~RCMessage();

    int init(MYSQL *conn, CInsim *InSim);
    bool ReadConfig(const char *Track);

    string GetLang(byte UCID);

    bool Save(byte UCID);

    const char* _( byte UCID, string CODE );

    void ReadLangDir( const char *path );
    void ReadLangFile(const char *file);
    string GetLangList();

}; // this method is not good but better when old method.


#endif
