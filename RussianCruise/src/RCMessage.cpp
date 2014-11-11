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

int RCMessage::init(DBMySQL *db, CInsim *InSim)
{
    this->db = db;
    if (!this->db)
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

    CCText("^3R"+ClassName+":\t^2inited");
    return 0;
}

void RCMessage::ReadConfig(const char *Track)
{
    char dir[ MAX_PATH ];
    sprintf(dir, "%s/data/RCMessages", RootDir);

	ReadLangDir(dir);

    CCText("  ^7RCMessage\t^2OK");
}

void
RCMessage::ReadLangDir(const char *path)
{
	char findPath[MAX_PATH];

	sprintf(findPath,"%s",path);

    DIR *dp;
    struct dirent *dent;

    if( (dp = opendir(findPath)) == NULL) {
        printf("opendir: %s: %s\n", findPath, strerror(errno));
        return;
    }

    while(dent = readdir(dp))
    {
        if( strlen( dent->d_name ) == 7 )
        {
            char fileName[MAX_PATH];
            sprintf(fileName,"%s/%s", path, dent->d_name );

            ReadLangFile( fileName );
        }
    }

    closedir(dp);
	return;
}

void
RCMessage::ReadLangFile(const char *file)
{
	xString File = file;
	vector<string> arFiles = File.split('/', 1);

    string lang = arFiles.back().substr( 0 , arFiles.back().find(".txt") );

    ifstream readf (file, ios::in);

    if(readf.is_open() == false)
    {
        CCText("^1 Can't open " + string(file));
        return;
    }

    while (readf.good())
    {
        char str[128];

        readf.getline(str, 128);

        if (strlen(str) > 1)
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


    players[packet->UCID].UName = packet->UName;
    players[packet->UCID].Admin = packet->Admin;

    string query = "SELECT lang FROM message WHERE username = '";
    query += packet->UName;
    query += "';";

    DB_ROWS result = db->select( query );

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

    if (Msg.find("!lang") == 0 )
    {
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

void
RCMessage::InsimBTC( struct IS_BTC* packet )
{
    if (packet->ClickID == 230)
    {
        ShowNotify(packet->UCID);
        if(players[packet->UCID].notifications.empty())
        {
            insim->SendBFN(packet->UCID, 230);
        }
    }
}

void RCMessage::Save (byte UCID)
{
    char query[MAX_PATH];
    sprintf(query,"REPLACE INTO message (username, lang) VALUES ('%s','%s')", players[ UCID ].UName.c_str(), players[ UCID ].Lang.c_str());

    db->exec( query );
}

void
RCMessage::AddNotify(byte UCID, string Notify)
{
    players[UCID].notifications.push(Notify);
}

void
RCMessage::ShowNotify(byte UCID)
{
    byte
    l=100, t=90,
    hButton=5,
    w=100,
    h=16+hButton;

    insim->SendButton(255, UCID, 176, l - w / 2, t - h / 2, w, h, ISB_DARK, "");
    insim->SendButton(255, UCID, 177, l - w / 2, t - h / 2, w, h, ISB_DARK, "");
    insim->SendButton(255, UCID, 178, l - w / 2, t - h / 2 + 2, w, 10, 0, players[UCID].notifications.front());
    insim->SendButton(254, UCID, 179, l - 7, t - h / 2 + 14, 14, 6, ISB_LIGHT + ISB_CLICK, "^2OK");

    players[UCID].notifications.pop();
}

void
RCMessage::Event()
{
    if(color != "1")
        color = "1";
    else
        color = "2";

    for(auto pl: players)
    {
        byte UCID = pl.first;
        if(!players[UCID].notifications.empty())
        {
            insim->SendButton(255, UCID, 230, 96, 180, 8, 8, ISB_CLICK + ISB_LIGHT, "^"+color+"^K¢Ï");
        }
    }
}