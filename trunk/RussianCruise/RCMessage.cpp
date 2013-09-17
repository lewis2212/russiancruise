using namespace std;

#include "RCMessage.h"

RCMessage::RCMessage()
{

}

RCMessage::~RCMessage()
{

}

const char* RCMessage::_( byte UCID, string CODE )
{
	if (players[ UCID ].LangID == 0)
		return (char*)("^1LangID = 0");

	if( MsgArray[ players[ UCID ].LangID ].find( CODE ) == MsgArray[ players[ UCID ].LangID ].end() )
		return CODE.c_str();

	return MsgArray[ players[ UCID ].LangID ][ CODE ].c_str();
}

int RCMessage::GetLangID(byte UCID)
{
	return players[UCID].LangID;
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
            string id;
            string mesage;

            id = strtok (str,"\"");
            id.erase(id.find_last_not_of(" \t\r\n\0")+1);

			mesage = strtok (NULL,"\"");
			mesage.erase( mesage.find_last_not_of(" \t\r\n\0")+1);

			MsgArray[ LANG_ID_RUS ].erase( id );
            MsgArray[ LANG_ID_RUS ][ id ] = mesage;
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
			string id;
            string mesage;

            id = strtok (str,"\"");
            id.erase(id.find_last_not_of(" \t\r\n\0")+1);

			mesage = strtok (NULL,"\"");
			mesage.erase( mesage.find_last_not_of(" \t\r\n\0")+1);

			MsgArray[ LANG_ID_ENG ].erase( id );
            MsgArray[ LANG_ID_ENG ][ id ] = mesage;
        }
    }
    readf2.close();
}

void RCMessage::InsimNCN( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
        return;


    strcpy(players[ packet->UCID ].UName, packet->UName);


    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCMessages\\users\\%s.txt",RootDir,players[ packet->UCID ].UName);


    // Try Find New File
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf("Can't find %s\n Create File for user",file);
        players[ packet->UCID ].LangID = LANG_ID_RUS;
        save( packet->UCID );
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
                if (strncmp("LangID=",str,7)==0)
                {
                    players[ packet->UCID ].LangID = atoi(str+7);
                }
            }
        }
        readf.close();
    }
    FindClose(fff);
}

void RCMessage::InsimCNL( struct IS_CNL* packet )
{
	save(packet->UCID);
	players.erase( packet->UCID );
}

void RCMessage::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
        return;

    char Msg[128];
    strcpy(Msg,packet->Msg + ((unsigned char)packet->TextStart));


    if (strncmp(Msg, "!save", 5) == 0 )
        save( packet->UCID );

    if ((strncmp(Msg, "!lang", 5) == 0 ) or (strncmp(Msg, "!^Cязык", 7) == 0 ))
    {
        cout << players[ packet->UCID ].UName << " send !lang" << endl;

        char message2[128];
        strcpy(message2,Msg);

        if (strlen(message2) < 8)
        {
            SendMTC( packet->UCID ,_( packet->UCID , "2104"));
            return;
        }

        char * comand;
        char * id;

        comand = strtok (message2," ");
        id = strtok (NULL," ");

        if ((!id) or (strlen(id) != 3))
        {
            SendMTC( packet->UCID , _( packet->UCID , "2105"));
            return;
        }

        if (strstr("* eng rus",id) == 0)
            return;


        if (strcmp(id,"eng") == 0)
        {
            players[ packet->UCID ].LangID = LANG_ID_ENG;
        }
        else if (strcmp(id,"rus") == 0)
        {
            players[ packet->UCID ].LangID = LANG_ID_RUS;
        }

    }

}

void RCMessage::save (byte UCID)
{
	char file[MAX_PATH];
	sprintf(file,"%sdata\\RCMessages\\users\\%s.txt",RootDir,players[ UCID ].UName);

	ofstream writef (file,ios::out);
	writef << "LangID=" << players[ UCID ].LangID << endl;
	writef.close();
}
