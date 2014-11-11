#ifndef _RC_MESSAGE_H
#define _RC_MESSAGE_H

#include "RCBaseClass.h"
#include "tools.h"

#include <dirent.h>

struct MPlayer: public GlobalPlayer
{
    string Lang;
    queue<string> notifications;
};

class RCMessage:public RCBaseClass
{
private:
    string color = "1";
    char RootDir[MAX_PATH];

    map< string , map< string ,string> >MsgArray;

    map<byte, MPlayer>players;

    void InsimNCN( struct IS_NCN* packet );
    void InsimCNL( struct IS_CNL* packet );
    void InsimMSO( struct IS_MSO* packet );
    void InsimBTC( struct IS_BTC* packet );

public:
    RCMessage(const char* Dir);
    ~RCMessage();

    int init(DBMySQL *db, CInsim *InSim);
    void ReadConfig(const char *Track);

    string GetLang(byte UCID);

    void Save(byte UCID);

    const char* _( byte UCID, string CODE );

    void ReadLangDir( const char *path );
    void ReadLangFile(const char *file);
    string GetLangList();

    void AddNotify(byte UCID, string Notify);
    void ShowNotify(byte UCID);

    void Event();

};

#endif