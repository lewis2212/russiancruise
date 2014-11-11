#include "RCQuest.h"

RCQuest::RCQuest(const char* Dir)
{
    ClassName = "RCQuest";
    strcpy(RootDir,Dir);
}

RCQuest::~RCQuest()
{

}

void RCQuest::init(DBMySQL *db, CInsim *InSim)
{
    this->db = db;
    if (!this->db)
    {
        CCText("^3"+ClassName+": Can't sctruct MySQL Connector\n");
        return ;
    }

    insim = InSim;
    if (!insim)
    {
        CCText ("^3"+ClassName+": Can't struct CInsim class");
        return ;
    }

    CCText("^3"+ClassName+":\t^2inited");
}

void RCQuest::ReadConfig(const char *Track)
{
    char file[MAX_PATH];
    sprintf(file, "%s/data/%s/%s.json", RootDir, ClassName.c_str(), Track);

    ifstream readf (file, ios::in);

    if(!readf.is_open())
    {
        CCText("  ^7"+ClassName+"    ^1ERROR: ^8file " + (string)file + " not found");
        return ;
    }

    bool readed = configReader.parse( readf, config, false );

    if ( !readed )
    {
        readf.close();
        // report to the user the failure and their locations in the document.
        cout  << ClassName+": Failed to parse configuration\n"
                   << configReader.getFormattedErrorMessages();
        return;
    }
    readf.close();

    if(!config.isObject ())
    {
        CCText("  ^7"+ClassName+"\t^1FAIL");
    }

    CCText("  ^7"+ClassName+"\t^2OK");
}


// обработчик события когда на сервер заходит новый пользователь
void RCQuest::InsimNCN( struct IS_NCN *packet )
{
    if ( packet->UCID == 0 )
        return;

    players[ packet->UCID ].UName = packet->UName;
    players[ packet->UCID ].PName = packet->PName;
}

void RCQuest::InsimNPL( struct IS_NPL *packet )
{
    PLIDtoUCID[packet->PLID] = packet->UCID;
}

void RCQuest::InsimPLP( struct IS_PLP *packet )
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCQuest::InsimPLL( struct IS_PLL *packet )
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCQuest::InsimCNL( struct IS_CNL *packet )
{
    players.erase( packet->UCID );
}

void RCQuest::InsimCPR( struct IS_CPR *packet )
{
    players[ packet->UCID ].PName = packet->PName;
}

void RCQuest::InsimMSO( struct IS_MSO *packet )
{
    byte UCID = packet->UCID;

    if ( UCID == 0 )
        return;

    if ( packet->UserType != MSO_PREFIX )
        return;

    string Msg = packet->Msg + packet->TextStart;
}

void RCQuest::InsimMCI(struct IS_MCI *packet)
{
    for (int i = 0; i < packet->NumC; i++)
    {
        byte UCID = PLIDtoUCID[ packet->Info[i].PLID ];

        
    }
}
