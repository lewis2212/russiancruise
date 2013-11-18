using namespace std;

#include "RCMessage.h"

RCMessage::RCMessage(const char* Dir)
{
    strcpy(RootDir,Dir);
}

RCMessage::~RCMessage()
{

}

const char* RCMessage::_( byte UCID, string CODE )
{
    if ( players[ UCID ].Lang.size() == 0)
        return (char*)("^1Lang = 0");

    if ( MsgArray[ players[ UCID ].Lang ].find( CODE ) == MsgArray[ players[ UCID ].Lang ].end() )
        return CODE.c_str();

    return MsgArray[ players[ UCID ].Lang ][ CODE ].c_str();
}

string RCMessage::GetLang(byte UCID)
{
    return players[UCID].Lang;
}

int RCMessage::init(MYSQL *conn, CInsim *InSim)
{
    dbconn = conn;
    if (!dbconn)
    {
        printf("RCMessage: Can't sctruct MySQL Connector\n");
        return -1;
    }

    insim = InSim;
    if (!insim)
    {
        printf ("RCMessage: Can't struct CInsim class");
        return -1;
    }
    return 0;
}

void RCMessage::readconfig(const char *Track)
{
    char dir[ MAX_PATH ];
    sprintf(dir, "%s\\data\\RCMessages", RootDir);

	ReadLangDirecroty(dir);

    CCText("  ^7RCMessage  ^2OK");
}

void
RCMessage::ReadLangDirecroty(const char *path)
{
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	char findPath[MAX_PATH];

	sprintf(findPath,"%s\\*.txt",path);

	if((hFind = FindFirstFile(findPath, &FindFileData)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			if( strlen( FindFileData.cFileName ) == 7 )
			{
				char fileName[MAX_PATH];
				sprintf(fileName,"%s\\%s", path, FindFileData.cFileName );

				ReadLangFile( fileName );
			}
		}
		while(FindNextFile(hFind, &FindFileData));

		FindClose(hFind);
	}
	return;
}

void
RCMessage::ReadLangFile(const char *file)
{
	SplitString File = file;
	vector<string> arFiles = File.split('\\', 1);

	string lang = arFiles.back().substr( 0 , arFiles.back().find(".txt") );

	ifstream readf (file, ios::in);
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);
        if (strlen(str) > 0)
        {
			if (!strncmp(str, "#", 1) or !strncmp(str, "//", 2) or !strncmp(str, "::", 2))
				continue;

            string id;
            string mesage;

            id = strtok (str, "\"");
            id.erase(id.find_last_not_of(" \t\r\n\0")+1);

            mesage = strtok (NULL, "\"");
            mesage.erase( mesage.find_last_not_of(" \t\r\n\0")+1);

            MsgArray[ lang ].erase( id );
            MsgArray[ lang ][ id ] = mesage;
        }
    }
    readf.close();
}

void RCMessage::InsimNCN( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
        return;

    strcpy(players[ packet->UCID ].UName, packet->UName);

    string query = "SELECT lang FROM message WHERE username = '";
	query += packet->UName;
    query += "';";
    DB_ROWS result = dbSelect( query );

    if( result.size() > 0 )
	{
		DB_ROW row = result.front();
		players[ packet->UCID ].Lang = row["lang"];
	}
	else
	{
		printf("Can't find %s\n Create user\n", packet->UName);
        players[ packet->UCID ].Lang = "rus";
        save( packet->UCID );
	}
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
    strcpy(Msg, packet->Msg + ((unsigned char)packet->TextStart));


    if (strncmp(Msg, "!save", 5) == 0 )
        save( packet->UCID );

    if ((strncmp(Msg, "!lang", 5) == 0 ) or (strncmp(Msg, "!^Cязык", 7) == 0 ))
    {
        cout << players[ packet->UCID ].UName << " send !lang" << endl;

        char message2[96], str[64];
        strcpy(message2, Msg);

        if (strlen(message2) < 8)
        {
            SendMTC( packet->UCID, _( packet->UCID , "2104"));
            return;
        }

        char * comand;
        char * id;

        comand = strtok (message2, " ");
        id = strtok (NULL, " ");

        if (!id)
        {
            SendMTC(packet->UCID, _( packet->UCID , "2105"));
            return;
        }

		for( auto& l: MsgArray )
		{
			if( l.first == id )
			{
				 players[ packet->UCID ].Lang = string(id);

				sprintf(str, _(packet->UCID, "^1| ^7Language: %s"), id);
				SendMTC(packet->UCID, str);
				return;
			}
		}

		sprintf(str, _(packet->UCID, "^1| ^7Language %s not found"), id);
		SendMTC(packet->UCID, str);
		return;
    }

}

void RCMessage::save (byte UCID)
{
    char query[MAX_PATH];
    sprintf(query,"REPLACE INTO message (username, lang) VALUES ('%s','%s')", players[ UCID ].UName, players[ UCID ].Lang.c_str());

    dbExec( query );
}
