#ifndef _RC_GetMessage_H
#define _RC_GetMessage_H

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
    map< int , map< int ,string> >MsgArray;

    map<byte, MPlayer>players;

    void insim_ncn( struct IS_NCN* packet );
    void insim_cnl( struct IS_CNL* packet );
    void insim_mso( struct IS_MSO* packet );

public:
    RCMessage();
    ~RCMessage();

    int init(const char *dir,void *CInSim); // open file msg_rus.txt and msg_eng.txt and write msgs in GetMessage array
    void readconfig(const char *Track);

    void save(byte UCID);

    const char* GetMessage(byte UCID, int MsgID);

}; // this method is not good but better when old method.


#endif
