using namespace std;

#include "RCAntCheat.h"



RCAntCheat::RCAntCheat()
{

}

RCAntCheat::~RCAntCheat()
{

}

int RCAntCheat::init(char *dir,void *classname,void *CInSim, void *Message,void *Bank)
{
    strcpy(RootDir,dir); // Копируем путь до программы


    insim = (CInsim *)CInSim; // Присваиваем указателю область памяти
    if(!insim) // Проверяем на существование
    {
        printf ("Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)Message;
    if(!msg)
    {
        printf ("Can't struct RCMessage class");
        return -1;
    }

    bank = (RCBank *)Bank;
    if(!msg)
    {
        printf ("Can't struct RCBank class");
        return -1;
    }

    return 0;
}

void RCAntCheat::insim_cnl ()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            memset(&players[i],0,sizeof(struct Player));
            break;
        }
    }
}

void RCAntCheat::insim_crp()
{
    int i;

    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();

    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cpr->UCID)
        {
            strcpy(players[i].PName, pack_cpr->PName);
            break;
        }
    }
}

void RCAntCheat::insim_mci ()
{
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();

    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]

    {
        for (int j =0; j < MAX_PLAYERS; j++) // прогон по всему массиву players[j]
        {

            if (pack_mci->Info[i].PLID == players[j].PLID and players[j].PLID != 0 and players[j].UCID != 0)
            {
                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;

                int Speed = ((int)pack_mci->Info[i].Speed*360)/(32768);

                int lastX = players[j].Info.X/65536;
                int lastY = players[j].Info.Y/65536;

                int lastSpeed = ((int)players[j].Info.Speed*360)/(32768);

                float d = Distance(X, Y, lastX, lastY);

                if (Check_Pos(TrackInf.PitCount,TrackInf.XPit,TrackInf.YPit,X,Y))
                    d = 0;

                if (d > 500)
                {
                    SYSTEMTIME sm;
                    GetLocalTime(&sm);
                    char log[MAX_PATH];
                    sprintf(log,"%slogs\\cop\\teleport(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);
                }

                memcpy(&players[j].Info,&pack_mci->Info[i],sizeof(struct CompCar));

                /** NOTE: don't use break **/
            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }
}

void RCAntCheat::insim_mso ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    // The chat message is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
        return;

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_mso->UCID)
            break;
    }
}


void RCAntCheat::insim_ncn()
{
    int i;

    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

    if (pack_ncn->UCID == 0)
        return;

    for (i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == 0)
            break;

    if (i == MAX_PLAYERS)
        return;

    // Copy all the player data we need into the players[] array
    strcpy(players[i].UName, pack_ncn->UName);
    strcpy(players[i].PName, pack_ncn->PName);
    players[i].UCID = pack_ncn->UCID;
}

void RCAntCheat::insim_npl()
{
    //cout << "joining race or leaving pits" << endl;
    int i;

    struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();

    // Find player using UCID and update his PLID
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_npl->UCID)
        {
            players[i].NPL = 1;
            players[i].PLID = pack_npl->PLID;
            strcpy(players[i].CName,pack_npl->CName);


            /*****   Hack Detect ***/
            char Text[64];
            time_t timeh;
            int htime = time(&timeh); // get current time

            if (players[i].NPLTime == 0)
            {
                players[i].NPLTime = htime;
            }

            int ts = htime - players[i].HackTime;
            players[i].HackTime = htime;


            if (ts < 10)  // 10 sec
                players[i].NPLHack++;
            else
            {
                players[i].NPLHack = 1;
            }

            if (players[i].NPLHack > 4)   //max lines to tolerate
            {
                players[i].NPLHack = 0;
                strcpy(Text, "/kick ");
                strcat (Text, players[i].UName);
                send_mtc(players[i].UCID,"^1Hack detect");
                send_mst(Text);
                return;

            }

            if (strlen(pack_npl->CName) < 3)
            {
                strcpy(Text, "/kick ");
                strcat (Text, players[i].UName);
                send_mst("/msg ^1Hack detect");
                send_mst(Text);
                return;
            }

            players[i].ReadyForMCI = true;

            break;
        }
    }
}

void RCAntCheat::insim_plp()
{
    struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_plp->PLID)
        {
            players[i].PLID = 0;
            players[i].NPL = 0;

            memset(&players[i].Info,0,sizeof(CompCar));
            players[i].InPit = true;
            players[i].ReadyForMCI = false;
            break;
        }
    }
}

void RCAntCheat::insim_pll()
{
    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_pll->PLID)
        {
            players[i].PLID = 0;
            players[i].NPL = 0;

            memset(&players[i].Info,0,sizeof(CompCar));
            players[i].InPit = true;
            players[i].ReadyForMCI = false;
            break;
        }
    }
}

void RCAntCheat::insim_pla ()
{

    struct IS_TINY pack_requests;
    memset(&pack_requests, 0, sizeof(struct IS_TINY));
    pack_requests.Size = sizeof(struct IS_TINY);
    pack_requests.Type = ISP_TINY;
    pack_requests.ReqI = 1;
    pack_requests.SubT = TINY_REO;      //
    insim->send_packet(&pack_requests);
}

void RCAntCheat::insim_reo ()
{
    struct IS_REO *pack = (struct IS_REO*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID != 0)
        {
            int j = 0;
            while (j < pack->NumP)
            {
                if (players[i].PLID == pack->PLID[j])
                    break;

                ++j;
            }

            if (j == pack->NumP)
            {
                char Text[48];
                sprintf(Text,"/spec %s",players[i].UName);
                send_mst(Text);
                sprintf(Text,"/msg %s ^1have wrong PLID",players[i].UName);
                send_mst(Text);
            }
        }
    }
}


void RCAntCheat::pitlane (const char* UName)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    sprintf(pack_mst.Msg,"/pitlane %s",UName);
    insim->send_packet(&pack_mst);
}
