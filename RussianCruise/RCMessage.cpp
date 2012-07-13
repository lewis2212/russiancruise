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

            //printf("%s LangID=%d MsgID=%d\n",players[i].UName,players[i].LangID,MsgID);

            //strncpy(MSG,MsgArray[players[i].LangID][MsgID],strlen(MsgArray[players[i].LangID][MsgID]));
            //sprintf(MSG,"STRLEN = %d\n",strlen(MsgArray[players[i].LangID][MsgID]));
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
    strcpy(file,RootDir);
    //strcat(file,"misc\\mysql.cfg");
    strcat(file,"data\\RCMessages\\rus.txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
        return;
    }
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
            //cout << "id = " << id << "message = " << message[1][atoi(id)] << endl;
            //send_mst(MsgArray[1][atoi(id)]);

        }
    }
    readf.close();

    strcpy(file,RootDir);
    //strcat(file,"misc\\mysql.cfg");
    strcat(file,"data\\RCMessages\\eng.txt");

//    HANDLE fff;
//    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
        return;
    }
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

            //cout << "id = " << id << "GetMessage = " << mesage << endl;

            //memset(&GetMessage[atoi(id)],0,sizeof(GetMessage));
            strncpy(MsgArray[2][atoi(id)], mesage,strlen(mesage));

        }
    }
    readf2.close();
}


void RCMessage::next_packet()
{
    switch (insim->peek_packet())
    {
    case ISP_NCN:
        ncn();
        break;

    case ISP_CNL:
        cnl();
        break;

    case ISP_MSO:
        mso();
        break;

    }
}

void RCMessage::ncn()
{
    //printf("New player connect\n");
    int i;

    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

    //cout << "ReqI = " << (int)pack_ncn->ReqI << endl;


    if (pack_ncn->UCID == 0)
    {
        //cout << "(Host connected, not adding him to array...)" << endl << endl;
        return;
    }



    for (i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == 0)
            break;


    if (i == MAX_PLAYERS)
    {
        //cout << "NO MORE PEOPLE ALLOWED!!!" << endl << endl;
        return;
    }

    // Copy all the player data we need into the players[] array
    strcpy(players[i].UName, pack_ncn->UName);
    players[i].UCID = pack_ncn->UCID;

    /** read file **/

    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCMessages\\users\\%s.txt",RootDir,players[i].UName);
    //strcat(file,"tracks\\.txt");

    // Read Data From File

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

void RCMessage::cnl()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            /** save file**/
            save(players[i].UCID);

            memset(&players[i],0,sizeof(struct MPlayer));
            break;
        }
    }
}

void RCMessage::mso()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    char Msg[96];
    strcpy(Msg,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));

    // The chat GetMessage is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
    {
        //cout << "(Chat GetMessage by host: " << pack_mso->Msg + ((unsigned char)pack_mso->TextStart) << ")" << endl;
        return;

    }

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_mso->UCID)
        {
            //cout << "Msg: " << pack_mso->Msg << endl;
            break;
        }
    }



    if (strncmp(Msg, "!save", 5) == 0 )
    {
        cout << players[i].UName << " send !save (RCMessages)" << endl;


        save(players[i].UCID);

    }

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
        {
            return;
        }


        //read_lang(&ginfo.players[i]);
        if (strcmp(id,"eng") == 0)
        {
            players[i].LangID = 2;
        }
        else if (strcmp(id,"rus") == 0)
        {
            players[i].LangID = 1;
        }

        //read_track(&ginfo.players[i]);

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
            //printf(file);

            ofstream writef (file,ios::out);
            writef << "LangID=" << players[i].LangID << endl;
            writef.close();

            break;

        }
    }

}

void RCMessage::send_mtc (byte UCID,const char* Msg)
{
    char errmsg[64];
    ZeroMemory(&errmsg,64);
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strncpy(pack_mtc.Text, Msg,strlen(Msg));
    if (!insim->send_packet(&pack_mtc,errmsg))
        cout << errmsg << endl;
};

void RCMessage::send_mst (const char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst,errmsg);
};


