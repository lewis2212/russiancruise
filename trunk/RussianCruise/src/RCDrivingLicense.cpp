using namespace std;

#include "RCDrivingLicense.h"

RCDL::RCDL(const char* Dir)
{
    strcpy(RootDir,Dir);
}

RCDL::~RCDL()
{

}

int RCDL::GetLVL(byte UCID)
{
    return players[ UCID ].LVL;
}

int RCDL::GetSkill(byte UCID)
{
    return players[ UCID ].Skill;
}

bool RCDL::AddSkill(byte UCID)
{
    if ( !Islocked( UCID ) )
    {
        players[ UCID ].Skill += 250 * players[ UCID ].LVL;

        char Text[64];
        float nextlvl = ( pow( players[ UCID ].LVL, 2 ) * 0.5 + 100 ) * 1000;
        float skl = ( players[ UCID ].LVL * 250 / nextlvl ) * 100;
        sprintf( Text, msg->_(UCID, "AddSkill"), skl);
        SendMTC( UCID, Text );
        return true;
    }
    return false;
}

bool RCDL::AddSkill(byte UCID, float coef)
{
    if (coef < 0)
    {
        return false;
    }

    if ( !Islocked( UCID ) )
    {
        players[ UCID ].Skill += (250 * players[ UCID ].LVL ) * coef;

        char Text[64];
        float nextlvl = ( pow( players[ UCID ].LVL, 2 ) * 0.5 + 100 ) * 1000;
        float skl = ( players[ UCID ].LVL * 250 / nextlvl ) * 100 * coef;
        sprintf( Text, msg->_(UCID, "AddSkill"), skl);
        SendMTC( UCID, Text );
        return true;
    }
    return false;
}

bool RCDL::RemSkill(byte UCID)
{
    if ( !Islocked( UCID ) )
    {
        if ( players[ UCID ].Skill < 500 * players[ UCID ].LVL )
        {

            if ( players[ UCID ].LVL > 0 )
            {
                players[ UCID ].Skill = ( ( pow( players[ UCID ].LVL - 1 , 2 ) * 0.5 + 100 ) * 1000 ) - ( 500 * players[ UCID ].LVL - players[ UCID ].Skill );
                players[ UCID ].LVL --;
            }
            else
            {
                players[ UCID ].Skill = 0;
            }

        }
        else
        {
            players[ UCID ].Skill -= 500 * players[ UCID ].LVL;
        }

        char Text[64];
        float nextlvl = ( pow( players[ UCID ].LVL , 2 ) * 0.5 + 100 ) * 1000;
        float skl = ( players[ UCID ].LVL * 500 / nextlvl ) * 100;
        sprintf(Text, msg->_(UCID, "RemSkill"), skl );
        SendMTC( UCID, Text );
        return true;
    }
    return false;
}

bool RCDL::RemSkill(byte UCID, float coef)
{
    if (coef < 0)
        return false;

    if ( !Islocked( UCID ) )
    {
        if ( players[ UCID ].Skill < 500 * players[ UCID ].LVL * coef )
        {

            if ( players[ UCID ].LVL > 0 )
            {
                players[ UCID ].Skill = ( ( pow( players[ UCID ].LVL - 1 , 2 ) * 0.5 + 100 ) * 1000 ) - ( 500 * players[ UCID ].LVL * coef - players[ UCID ].Skill );
                players[ UCID ].LVL --;
            }
            else
            {
                players[ UCID ].Skill = 0;
            }
        }
        else
        {
            players[ UCID ].Skill -= 500 * players[ UCID ].LVL * coef;
        }

        char Text[64];
        float nextlvl = ( pow( players[ UCID ].LVL , 2 ) * 0.5 + 100 ) * 1000;
        float skl = ( players[ UCID ].LVL * 500 / nextlvl ) * 100;
        sprintf(Text, msg->_(UCID, "RemSkill"), skl * coef );
        SendMTC( UCID , Text);

        return true;
    }

    return false;
}

bool RCDL::Lock(byte UCID)
{
    players[ UCID ].Lock = 1;
    return true;
}

bool RCDL::Unlock(byte UCID)
{
    players[ UCID ].Lock = 0;
    return true;
}

bool RCDL::Islocked(byte UCID)
{
    if (players[ UCID ].Lock == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int RCDL::init(MYSQL *conn, CInsim *InSim, void *RCMessageClass)
{
    dbconn = conn;
    if (!dbconn)
    {
        CCText("^3RCDL:\t\t^1Can't sctruct MySQL Connector");
        return -1;
    }

    insim = InSim;
    if (!insim)
    {
        CCText("^3RCDL:\t\t^1Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)RCMessageClass;
    if (!msg)
    {
        CCText("^3RCDL:\t\t^1Can't struct RCMessage class");
        return -1;
    }

    inited = 1;

    CCText("^3RCDL:\t\t^2Connected to MySQL server");

    return 0;
}

void RCDL::InsimNCN( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
    {
        return;
    }

    // Copy all the player data we need into the players[] array
    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);

    char query[128];
    sprintf(query, "SELECT lvl, skill FROM dl WHERE username='%s' LIMIT 1;", packet->UName);

    DB_ROWS result = dbSelect(query);
    DB_ROW row;

    if (dbres == NULL)
    {
        printf("Error: can't get the result description\n");
    }

    if( result.size() != 0 )
    {
    	row = result.front();
        players[packet->UCID].LVL = atof(row["lvl"].c_str());
        players[packet->UCID].Skill = atof(row["skill"].c_str());
    }
    else
    {
        printf("RCDL: Can't find %s - Create user\n", packet->UName);

        sprintf(query, "INSERT INTO dl (username) VALUES ('%s');", packet->UName);
        dbExec(query);

        players[ packet->UCID ].LVL = 0;
        Save( packet->UCID );

    }
    btn_dl( packet->UCID );
}

void RCDL::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[ packet->PLID ] = packet->UCID;
}

void RCDL::InsimPLP( struct IS_PLP* packet)
{
    memset(&players[PLIDtoUCID[ packet->PLID ]  ].Info, 0, sizeof( CompCar ) );
    PLIDtoUCID.erase( packet->PLID );
}

void RCDL::InsimPLL( struct IS_PLL* packet )
{
    memset(&players[PLIDtoUCID[ packet->PLID ]  ].Info, 0, sizeof( CompCar ) );
    PLIDtoUCID.erase( packet->PLID );
}

void RCDL::InsimCNL( struct IS_CNL* packet )
{
    Save( packet->UCID );
    players.erase( packet->UCID );
}

void RCDL::InsimCPR( struct IS_CPR* packet )
{
    strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCDL::InsimMSO( struct IS_MSO* packet )
{

}

void RCDL::Save (byte UCID)
{
    DB_ROW query;
    query["lvl"] = ToString(players[ UCID ].LVL);
    query["skill"] = ToString(players[ UCID ].Skill);
    dbUpdate("dl",query,{"username",players[ UCID ].UName});

}

void RCDL::InsimCON( struct IS_CON* packet )
{
    struct IS_CON *pack_con = (struct IS_CON*)insim->get_packet();
    /*
    	for (int i=0; i<MAX_PLAYERS; i++)
    	{
    		if (players[i].PLID == pack_con->A.PLID)
    		{
    			if (!Islocked(players[i].UCID))
    				RemSkill(players[i].UCID);
    			break;
    		}
    	}

    	for (int i=0; i<MAX_PLAYERS; i++)
    	{
    		if (players[i].PLID == pack_con->B.PLID)
    		{
    			if (!Islocked(players[i].UCID))
    				RemSkill(players[i].UCID);
    			break;
    		}
    	}

    */

}


void RCDL::InsimMCI( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
        byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];

        int X = pack_mci->Info[i].X / 65536;
        int Y = pack_mci->Info[i].Y / 65536;
        int S = ((int)pack_mci->Info[i].Speed * 360) / (32768);
        int X1 = players[ UCID ].Info.X / 65536;
        int Y1 = players[ UCID ].Info.Y / 65536;

        if (X1 == 0 and Y1 == 0)
        {
            X1=X;
            Y1=Y;
            players[ UCID ].Info = pack_mci->Info[i];
        }

        float Skill = Distance(X, Y, X1, Y1);

        if ((abs((int)Skill) > 10) and (S > 30))
        {
            players[ UCID ].Skill += abs((int)(Skill * 1.5f));
            players[ UCID ].Info = pack_mci->Info[i];
        }

        /** next lvl **/

        float nextlvl = (pow(players[ UCID ].LVL, 2) * 0.5+100) * 1000;

        if (players[ UCID ].Skill > nextlvl)
        {
            players[UCID].LVL ++;
            players[UCID].Skill = 0;
            char Msg[64];
            sprintf(Msg, msg->_(UCID, "GetLvl"), players[ UCID ].PName, players[ UCID ].LVL);

            if (UCID != 0)
                SendMTC(255, Msg);
        }

        /** buttons **/
        btn_dl( UCID );
    }
}

void RCDL::btn_dl( byte UCID )
{
    //TODO: refactoring
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 230;
    pack.BStyle = 32+64;
    pack.L = 100;
    pack.T = 5;
    pack.W = 30;
    pack.H = 4;

    float nextlvl = ( pow( players[ UCID ].LVL, 2) * 0.5 + 100 ) * 1000;
    float skl = ( players[ UCID ].Skill / nextlvl ) * 100;

    sprintf( pack.Text, msg->_(UCID, "LvlAndSkill"), players[ UCID ].LVL, skl);
    insim->send_packet(&pack);
}
