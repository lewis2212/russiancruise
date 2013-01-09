using namespace std;

#include "RCMessage.h"

RCMessage::RCMessage()
{

}

RCMessage::~RCMessage()
{

}

char* RCMessage::GetMessage(byte UCID, int MsgID)
{
    for (int i=0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            if (players[i].LangID == 0)
                return (char*)("^1LangID == 0");

            return MsgArray[players[i].LangID][MsgID];
        }
    }
    return (char*)("^1User Not Found");
}



int RCMessage::init(const char *dir, void *CInSim)
{
    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        printf ("Can't struct CInsim class");
        return -1;
    }
    return 0;
}

void RCMessage::readconfig(const char *Track)
{
    char file[255];
    sprintf(file,"%sdata\\RCMessages\\rus.txt",RootDir);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
        return;
    FindClose(fff);

    ifstream readf (file,ios::in);
    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);
        if (strlen(str) > 6)
        {
            char * id;
            char * mesage;

            id = strtok (str,"\"");
            mesage = strtok (NULL,"\"");
            memset(&MsgArray[1][atoi(id)],0,sizeof(MsgArray[1][atoi(id)]));
            strncpy(MsgArray[1][atoi(id)], mesage,strlen(mesage));
        }
    }
    readf.close();

    sprintf(file,"%sdata\\RCMessages\\eng.txt",RootDir);

    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
        return;

    FindClose(fff);


    ifstream readf2 (file,ios::in);
    while (readf2.good())
    {
        char str[128];
        readf2.getline(str,128);
        if (strlen(str) > 6)
        {
            char * id;
            char * mesage;

            id = strtok (str,"\"");
            mesage = strtok (NULL,"\"");
            memset(&MsgArray[2][atoi(id)],0,sizeof(MsgArray[2][atoi(id)]));
            strncpy(MsgArray[2][atoi(id)], mesage,strlen(mesage));

        }
    }
    readf2.close();
}

void RCMessage::insim_ncn( struct IS_NCN* packet )
{
    int i;



    if (packet->UCID == 0)
        return;

    for (i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == 0)
            break;


    if (i == MAX_PLAYERS)
        return;

    strcpy(players[i].UName, packet->UName);
    players[i].UCID = packet->UCID;

    /** read file **/

    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCMessages\\users\\%s.txt",RootDir,players[i].UName);


    // Try Find New File
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf("Can't find %s\n Create File for user",file);
        players[i].LangID = 2;
        save(players[i].UCID);
    }
    else
    {
        ifstream readf (file,ios::in);

        while (readf.good())
        {
            char str[128];
            readf.getline(str,128);
            if (strlen(str) > 0)
            {
                // Get Cash
                if (strncmp("LangID=",str,7)==0)
                {
                    players[i].LangID = atoi(str+7);
                }
                // Get Credits
                // Get Deposits
            }
        }
        readf.close();
    }
    FindClose(fff);
}

void RCMessage::insim_cnl( struct IS_CNL* packet )
{
    int i;



    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {
            /** save file**/
            save(players[i].UCID);

            memset(&players[i],0,sizeof(struct MPlayer));
            break;
        }
    }
}

void RCMessage::insim_mso( struct IS_MSO* packet )
{
    int i;



    char Msg[96];
    strcpy(Msg,packet->Msg + ((unsigned char)packet->TextStart));

    // The chat GetMessage is sent by the host, don't do anything
    if (packet->UCID == 0)
        return;

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
        if (players[i].UCID == packet->UCID)
            break;



    if (strncmp(Msg, "!save", 5) == 0 )
        save(players[i].UCID);

    if ((strncmp(Msg, "!lang", 5) == 0 ) or (strncmp(Msg, "!^Cязык", 7) == 0 ))
    {
        cout << players[i].UName << " send !lang" << endl;

        char message2[96];
        strcpy(message2,Msg);

        if (strlen(message2) < 8)
        {
            send_mtc(players[i].UCID,GetMessage(players[i].UCID,2104));
            return;
        }

        char * comand;
        char * id;

        comand = strtok (message2," ");
        id = strtok (NULL," ");

        if ((!id) or (strlen(id) != 3))
        {
            send_mtc(players[i].UCID,GetMessage(players[i].UCID,2105));
            return;
        }

        if (strstr("* eng rus",id) == 0)
            return;


        if (strcmp(id,"eng") == 0)
        {
            players[i].LangID = 2;
        }
        else if (strcmp(id,"rus") == 0)
        {
            players[i].LangID = 1;
        }

    }

}

void RCMessage::save (byte UCID)
{
    // Find player and set the whole player struct he was using to 0

    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {

            char file[MAX_PATH];
            sprintf(file,"%sdata\\RCMessages\\users\\%s.txt",RootDir,players[i].UName);

            ofstream writef (file,ios::out);
            writef << "LangID=" << players[i].LangID << endl;
            writef.close();

            break;
        }
    }
}
