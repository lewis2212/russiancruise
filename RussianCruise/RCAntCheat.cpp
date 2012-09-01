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
/*
void RCAntCheat::next_packet()
{
    switch (insim->peek_packet())
    {
    case ISP_NPL:
        insim_npl();
        break;

    case ISP_NCN:
        insim_ncn();
        break;

    case ISP_CNL:
        insim_cnl();
        break;

    case ISP_PLL:
        insim_pll();
        break;

    case ISP_PLP:
        insim_plp();
        break;

    case ISP_CPR:
        insim_crp();
        break;

    case ISP_MSO:
        insim_mso();
        break;

    case ISP_PLA:
        insim_pla();
        break;
    case ISP_REO:
        insim_reo();
        break;

    }
}
*/
void RCAntCheat::insim_cnl ()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
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

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cpr->UCID)
        {
            /** DO SOME CODE **/

            strcpy(players[i].PName, pack_cpr->PName);
            break;
        }
    }
}

void RCAntCheat::insim_mci ()
{
    //cout << "mci" << endl;
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]

    {
        for (int j =0; j < MAX_PLAYERS; j++) // прогон по всему массиву players[j]
        {

            if (pack_mci->Info[i].PLID == players[j].PLID and players[j].PLID != 0 and players[j].UCID != 0)
            {
                /** DO SOME CODE **/

                    int X = pack_mci->Info[i].X/65536;
                    int Y = pack_mci->Info[i].Y/65536;

                    int Speed = ((int)pack_mci->Info[i].Speed*360)/(32768);

                    int lastX = players[j].Info.X;
                    int lastY = players[j].Info.Y;

                    int lastSpeed = ((int)players[j].Info.Speed*360)/(32768);

                    float d = Distance(X, Y, lastX, lastY);

                    players[j].Distance += d;

                    if (Check_Pos(TrackInf.PitCount,TrackInf.XPit,TrackInf.YPit,X,Y))
                    {
                        players[j].Distance = 0;
                    }

                    if (players[j].Distance > 200)
                    {
                         players[j].Distance -= 200;

                        char Text[64];
                        sprintf(Text, "!%s is using teleport", players[i].UName);
                        //send_mtc(players[i].UCID,"^1Hack detect");
                        //send_mst(Text);
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
    {
        //cout << "(Chat message by host: " << pack_mso->Msg + ((unsigned char)pack_mso->TextStart) << ")" << endl;
        if (pack_mso->UserType == MSO_SYSTEM)
        {
            char msg[128];
            strcpy(msg,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));
            if (strstr(msg,"vote for")== NULL)
                return;

            char * user = strtok (msg," ");

            if (!user)
                return;

            for (int i=0; i<MAX_PLAYERS;i++ )
            {
                if (strcmp(user,players[i].PName)==0)
                {
                    sprintf(msg,"/kick %s",players[i].UName);
                    send_mst(msg);
                    return;
                }
            }

            return;

        }

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



    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!text", 5) == 0 )
    {

        /** DO SOME CODE **/
    }

}


void RCAntCheat::insim_ncn()
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
    strcpy(players[i].PName, pack_ncn->PName);
    players[i].UCID = pack_ncn->UCID;

    /** DO SOME CODE **/


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

            /*******    ********/
            /** DO SOME CODE **/
            break;
        }
    }
}

void RCAntCheat::insim_plp()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_plp->PLID)
        {
            /** DO SOME CODE **/
            // NOTE: speed

            players[i].PLID = 0;
            players[i].NPL = 0;
            /** DO SOME CODE **/

            memset(&players[i].Info,0,sizeof(CompCar));
            players[i].InPit = true;
             players[i].ReadyForMCI = false;
            break;
        }
    }
}

void RCAntCheat::insim_pll()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_pll->PLID)
        {
            /** DO SOME CODE **/
            // NOTE: speed

            players[i].PLID = 0;
            players[i].NPL = 0;
            /** DO SOME CODE **/
            memset(&players[i].Info,0,sizeof(CompCar));
            players[i].InPit = true;
             players[i].ReadyForMCI = false;
            break;
        }
    }
}

void RCAntCheat::insim_pla ()
{
    //int i;

    //struct IS_PLA *pack_pla = (struct IS_PLA*)insim->get_packet();

    // Find player and set his PLID to 0
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
    //int i;

    struct IS_REO *pack = (struct IS_REO*)insim->get_packet();

    // Find player and set his PLID to 0

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
    insim->send_packet(&pack_mst,errmsg);
}
void RCAntCheat::cheat(struct  Player *splayer)
{

}
