#ifndef _RC_MESSAGE_H
#define _RC_MESSAGE_H


#include <stdio.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <windows.h>
#include "CInsim.h"


class RCMessage
{
    // private:


public:
    RCMessage::RCMessage();
    RCMessage::~RCMessage();

    int init(); // open file msg_rus.txt and msg_eng.txt and write msgs in message array
    char message[3][20000][64]; // message[lang_id][msg_id][msg_len];
}; // this method is not good but better when old method.


#endif
