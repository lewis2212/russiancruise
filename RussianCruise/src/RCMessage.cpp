using namespace std;

#include "RCMessage.h"

RCMessage::RCMessage(const char* Dir)
{
    ClassName = "RCMessage";
    strcpy(RootDir,Dir);
}

RCMessage::~RCMessage()
{

}

const char* RCMessage::_( byte UCID, string CODE )
{
    if ( players[ UCID ].Lang.size() == 0)
	{
		players[ UCID ].Lang = "rus";
        return (char*)("^1Set default language");
	}

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

void RCMessage::ReadConfig(const char *Track)
{
    char dir[ MAX_PATH ];
    sprintf(dir, "%s\\data\\RCMessages", RootDir);

	ReadLangDir(dir);

    CCText("  ^7RCMessage\t^2OK");
}

void
RCMessage::ReadLangDir(const char *path)
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
	xString File = file;
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

            if( id.size() == 0 )
			{
				CCText("RCMesages: Bad code");
				continue;
			}

            id.erase(id.find_last_not_of(" \t\r\n\0")+1);

            mesage = strtok (NULL, "\"");

			if( mesage.size() == 0 )
			{
				CCText("RCMesages: Bad message");
				continue;
			}

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

    players[ packet->UCID ].UName = packet->UName;

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
        Save( packet->UCID );
	}
}

void RCMessage::InsimCNL( struct IS_CNL* packet )
{
    Save(packet->UCID);
    players.erase( packet->UCID );
}

string RCMessage::GetLangList()
{
    string lang = "";
    for( auto& l: MsgArray )
        lang += ", " + l.first;
    return lang.substr(2);
}

void RCMessage::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
        return;

    xString Msg = packet->Msg + packet->TextStart;

    if (Msg.find("!lang") == 0 or Msg.find("!^Cязык")== 0 )
    {
        cout << players[ packet->UCID ].UName << " send !lang" << endl;

        if ( Msg.size() < 8)
        {
            insim->SendMTC( packet->UCID, _( packet->UCID , "2104"));
            return;
        }

        vector<string> args = Msg.split(' ');

		if(args.size() < 2)
        {
            insim->SendMTC(packet->UCID, _( packet->UCID , "2105"));
            return;
        }

        string id = args[1];

		for( auto& l: MsgArray )
		{
			if( l.first == id )
			{
                players[ packet->UCID ].Lang = id;

				insim->SendMTC(packet->UCID, _(packet->UCID, "^1| ^7Language: ") + id);
				return;
			}
		}

		insim->SendMTC(packet->UCID, StringFormat(_(packet->UCID, "^1| ^7Language %s not found"), id.c_str()));
		return;
    }

}

void RCMessage::Save (byte UCID)
{
    char query[MAX_PATH];
    sprintf(query,"REPLACE INTO message (username, lang) VALUES ('%s','%s')", players[ UCID ].UName.c_str(), players[ UCID ].Lang.c_str());

    dbExec( query );
}
