#ifndef _RC___H
#define _RC___H

#include "RCBaseClass.h"

#include "tools.h"

#define LANG_ID_RUS 1
#define LANG_ID_ENG 2

struct MPlayer
{
    char UName[24];
    int LangID;
};

class RCMessage:public RCBaseClass
{
private:

    char RootDir[MAX_PATH];

    //char MsgArray[3][20000][64]; // message[lang_id][msg_id][msg_len];
    //map< int , map< int ,string> >MsgArray;
    map< int , map< string ,string> >MsgArray;

    map<byte, MPlayer>players;

    void InsimNCN( struct IS_NCN* packet );
    void InsimCNL( struct IS_CNL* packet );
    void InsimMSO( struct IS_MSO* packet );

public:
    RCMessage(const char* Dir);
    ~RCMessage();

    int init(MYSQL *conn, CInsim *InSim);
    void readconfig(const char *Track);

    int GetLangID(byte UCID);

    void save(byte UCID);

    const char* _( byte UCID, string CODE );

}; // this method is not good but better when old method.


#endif
