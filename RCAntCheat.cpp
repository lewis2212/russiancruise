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

void RCAntCheat::next_packet()
{
    switch (insim->peek_packet())
    {
    case ISP_NPL:
        npl();
        break;

    case ISP_NCN:
        ncn();
        break;

    case ISP_CNL:
        cnl();
        break;

    case ISP_PLL:
        pll();
        break;

    case ISP_PLP:
        plp();
        break;

    case ISP_CPR:
        crp();
        break;

    case ISP_MSO:
        mso();
        break;

    }
}

void RCAntCheat::cnl ()
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

void RCAntCheat::crp()
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

void RCAntCheat::mci ()
{
    //cout << "mci" << endl;
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]

    {
        for (int j =0; j < MAX_PLAYERS; j++) // прогон по всему массиву players[j]
        {

            if (pack_mci->Info[i].PLID == players[j].PLID) // старая заплатка ( and players[j].PLID != 0 and players[j].UCID != 0)
            {
                /** DO SOME CODE **/

                float X = pack_mci->Info[i].X/65536;
                float Y = pack_mci->Info[i].Y/65536;
                float Z = pack_mci->Info[i].Z/65536;

                float X1 = players[j].X/65536;
                float Y1 = players[j].Y/65536;
                float Z1 = players[j].Z/65536;

                if (X1==0 and Y1==0 and Z1==0)
                {
                    X1=X;
                    Y1=Y;
                    Z1=Z;
                }

                //printf("X=%d Y=%d\n",X,Y);

                float Dist = sqrt(pow((X-X1),2)+pow((Y-Y1),2)+pow((Z-Z1),2));
                Dist = (Dist)*2;

                // NOTE: speed hack
                float Speed = Dist;
                float Speed2 = players[j].Speed;

                float Accelerate = (Speed - Speed2)*2; // mci delay == 0.5 sec
                if (Accelerate > players[j].MaxAcelerate)
                players[j].MaxAcelerate = Accelerate;

                struct IS_BTN pack;
                memset(&pack, 0, sizeof(struct IS_BTN));
                pack.Size = sizeof(struct IS_BTN);
                pack.Type = ISP_BTN;
                pack.ReqI = 1;
                pack.UCID = players[j].UCID;
                pack.Inst = 0;
                pack.TypeIn = 0;
                pack.ClickID = 110;
                pack.BStyle = 32;
                pack.L = 136;
                pack.T = 10;
                pack.W = 30;
                pack.H = 8;
                sprintf(pack.Text,"^7%3.3f m/ss",Accelerate);
                //printf(pack.Text,"^7%f",Dist);
                insim->send_button(&pack);

                pack.ClickID = 111;
                pack.BStyle = 32;
                pack.L = 136;
                pack.T = 19;
                pack.W = 30;
                pack.H = 8;
                sprintf(pack.Text,"^7Max %3.3f m/ss",players[j].MaxAcelerate);
                //printf(pack.Text,"^7%f",Dist);
                insim->send_button(&pack);

                pack.ClickID = 112;
                pack.BStyle = 32;
                pack.L = 136;
                pack.T = 28;
                pack.W = 30;
                pack.H = 8;
                sprintf(pack.Text,"^7X %f m",X);
                //printf(pack.Text,"^7%f",Dist);
                insim->send_button(&pack);

                pack.ClickID = 113;
                pack.BStyle = 32;
                pack.L = 136;
                pack.T = 37;
                pack.W = 30;
                pack.H = 8;
                sprintf(pack.Text,"^7Y %f m",Y);
                //printf(pack.Text,"^7%f",Dist);
                insim->send_button(&pack);


                players[j].X = pack_mci->Info[i].X;
                players[j].Y = pack_mci->Info[i].Y;
                players[j].Z = pack_mci->Info[i].Z;
                players[j].Speed = Dist;



                /** NOTE: don't use break **/
            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }
}

void RCAntCheat::mso ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    // The chat message is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
    {
        //cout << "(Chat message by host: " << pack_mso->Msg + ((unsigned char)pack_mso->TextStart) << ")" << endl;
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


void RCAntCheat::ncn()
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

void RCAntCheat::npl()
{
    //cout << "joining race or leaving pits" << endl;
    int i;

    struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();

    // Find player using UCID and update his PLID
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_npl->UCID)
        {
            players[i].PLID = pack_npl->PLID;
            /** DO SOME CODE **/
            break;
        }
    }
}

void RCAntCheat::plp()
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
            players[i].MaxAcelerate = 0;
            players[i].X = 0;
            players[i].Y = 0;
            players[i].Z = 0;
            players[i].PLID = 0;
            /** DO SOME CODE **/
            break;
        }
    }
}

void RCAntCheat::pll()
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
            players[i].MaxAcelerate = 0;
            players[i].X = 0;
            players[i].Y = 0;
            players[i].Z = 0;
            players[i].PLID = 0;
            /** DO SOME CODE **/
            break;
        }
    }
}


