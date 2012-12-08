#ifndef _RC_GetMessage_H
#define _RC_GetMessage_H


#include <stdio.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <windows.h>

#include "RCBaseClass.h"

#include "tools.h"

struct MPlayer
{
    byte UCID;
    char UName[24];
    int LangID;
};

class RCMessage:public RCBaseClass
{
private:

    char RootDir[MAX_PATH];

    char MsgArray[3][20000][64]; // message[lang_id][msg_id][msg_len];

    struct MPlayer players[MAX_PLAYERS];

    void insim_ncn();
    void insim_cnl();
    void insim_mso();

public:
    RCMessage();
    ~RCMessage();

    int init(const char *dir,void *CInSim); // open file msg_rus.txt and msg_eng.txt and write msgs in GetMessage array
    void readconfig(const char *Track);

    void save(byte UCID);

    char* GetMessage(byte UCID, int MsgID);

}; // this method is not good but better when old method.


#endif
