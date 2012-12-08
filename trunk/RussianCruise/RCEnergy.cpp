using namespace std;
#include "RCEnergy.h"

void *nrg_thread_btn (void *energy)
{

    RCEnergy *nrg = (RCEnergy *)energy; //struct our RCPizza class in thread

    if(!nrg)
    {
        printf ("Can't start Energy thread");
        return 0;
    }

    int ok = 1;

    printf("\tthread \"Energy button\" started\n");
    while (ok > 0)
    {
        for (int i=0; i<MAX_PLAYERS; i++)
            if (nrg->players[i].UCID != 0)
                nrg->btn_energy(&nrg->players[i]);

        Sleep(2000);
    }
    return 0;
};


RCEnergy::RCEnergy()
{

}

RCEnergy::~RCEnergy()
{

}

int RCEnergy::init(const char *dir,void *classname,void *CInSim, void *Message,void *Bank)
{
    strcpy(RootDir,dir);

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    if (pthread_create(&tid,&attr,nrg_thread_btn,classname) < 0)
        return -1;

    insim = (CInsim *)CInSim;
    if(!insim)
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


void RCEnergy::readconfig(const char *Track)
{
    char file[255];
    sprintf(file,"%sdata\\RCEnergy\\maps\\%s.txt", RootDir, Track );

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

        if (strstr(str,"//")) {}
        if (strlen(str) > 0)
        {
            if (strncmp(str,"/cafe",5)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);
                TrackInf.CafeCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,";");
                    Y = strtok (NULL,";");
                    TrackInf.XCafe[i] = atoi(X);
                    TrackInf.YCafe[i] = atoi(Y);
                }
            }

        } // if strlen > 0
    } //while readf.good()

    readf.close();
}



void RCEnergy::insim_ncn()
{
    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

    if (pack_ncn->UCID == 0)
        return;

    int i;
    for (i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == 0)
            break;

    if (i == MAX_PLAYERS)
        return;

    // Copy all the player data we need into the players[] array
    strcpy(players[i].UName, pack_ncn->UName);
    strcpy(players[i].PName, pack_ncn->PName);
    players[i].UCID = pack_ncn->UCID;

    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCEnergy\\%s.txt", RootDir, players[i].UName);

    // Try Find New File
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        players[i].Energy = 10000;
        energy_save(i);
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
                if (strncmp("Energy=",str,7)==0)
                    players[i].Energy = atoi(str+7);
            }
        }
        readf.close();
    }
    FindClose(fff);
}

void RCEnergy::insim_npl()
{
    int i;

    struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();

    // Find player using UCID and update his PLID
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_npl->UCID)
        {

            char Text[64];
            strcpy(Text, "/spec ");
            strcat (Text, players[i].UName);

            players[i].PLID = pack_npl->PLID;
            players[i].EnergyTime = time(&nrgtime);

            if ( players[i].Energy < 500 )
            {

                send_mtc(players[i].UCID,msg->GetMessage(players[i].UCID,2402));
                send_mtc(players[i].UCID,msg->GetMessage(players[i].UCID,2403));
                players[i].Zone = 1;
                players[i].PLID = 0;
                send_mst(Text);
                return;
            }
        }
    }
}

void RCEnergy::insim_plp()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_plp->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCEnergy::insim_pll()
{
    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    // Find player and set his PLID to 0
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_pll->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCEnergy::insim_cnl ()
{
    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            energy_save(players[i].UCID);

            memset(&players[i],0,sizeof(struct EnergyPlayer));
            break;
        }
    }
}

void RCEnergy::energy_save (byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {

            char file[255];
            sprintf(file,"%sdata\\RCEnergy\\%s.txt", RootDir, players[i].UName);

            ofstream writef (file,ios::out);
            writef << "Energy=" << players[i].Energy << endl;
            writef.close();
        }
    }


}




void RCEnergy::insim_crp()
{
    int i;

    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cpr->UCID)
        {
            strcpy(players[i].PName, pack_cpr->PName);
            break;
        }
    }
}

void RCEnergy::insim_mci ()
{
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();

    for (int i = 0; i < pack_mci->NumC; i++)
    {
        for (int j =0; j < MAX_PLAYERS; j++)
        {
            if (pack_mci->Info[i].PLID == players[j].PLID and players[j].PLID != 0 and players[j].UCID != 0)
            {

                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;
                int Z = pack_mci->Info[i].Z/65536;
                int D = pack_mci->Info[i].Direction/182;
                int H = pack_mci->Info[i].Heading/182;

                int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

                int A = pack_mci->Info[i].AngVel*360/16384;

                int X1 = players[j].Info.X/65536;
                int Y1 = players[j].Info.Y/65536;
                int Z1 = players[j].Info.Z/65536;
                int D1 = players[j].Info.Direction/182;
                int H1 = players[j].Info.Heading/182;
                int S1 = ((int)players[j].Info.Speed*360)/(32768);
                int A1 = players[j].Info.AngVel*360/16384;


                long dA = A-A1;
                long dS = S-S1;
                long dD = abs((int)(sin(D)*100))-abs((int)(sin(D1)*100));
                long dH = abs((int)(sin(H)*100))-abs((int)(sin(H1)*100));

                int K = (int)sqrt(abs((dD-dH)*(1+dA)*dS))/8;


                if ((players[j].Energy > 5) and (S > 5))
                {
                    if (!Islocked(players[i].UCID))
                        players[j].Energy -= K;
                }


                if (Check_Pos(TrackInf.CafeCount,TrackInf.XCafe,TrackInf.YCafe,X,Y))
                    players[j].Zone = 3;
                else
                    players[j].Zone = 0;

                if (S == 0)
                {
                    int time_i = time(&nrgtime) - players[j].EnergyTime;

                    if (time_i > 59)
                    {
                        if (players[j].Zone == 3)
                            players[j].Energy += 400;
                        else
                            players[j].Energy += 200;

                        players[j].EnergyTime = time(&nrgtime);
                    }

                }

                if (players[j].Energy > 10000 )
                {
                    players[j].Energy = 10000;
                }
                else if (players[j].Energy < 0 )
                {
                    players[j].Energy = 0;
                }

                if (X1==0 and Y1==0 and Z1==0)
                {
                    X1=X;
                    Y1=Y;
                    Z1=Z;
                }

                memcpy( &players[j].Info , &pack_mci->Info[i] , sizeof(struct CompCar) );

                if (players[j].Energy < 10)
                {
                    char Text[64];
                    strcpy(Text, "/spec ");
                    strcat (Text, players[j].UName);
                    players[j].PLID = 0;
                    players[j].Zone = 1;
                    send_mst(Text);
                }
            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }
}


void RCEnergy::insim_mso ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    if (pack_mso->UCID == 0)
        return;

    for (i=0; i < MAX_PLAYERS; i++)
        if (players[i].UCID == pack_mso->UCID)
            break;

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!coffee", 7) == 0)
    {
        //out << players[i].UName << " send !coffee" << endl;
        if (((players[i].Zone == 1) and (players[i].Energy < 500)) or (players[i].Zone == 3))
        {
            if (bank->GetCash(players[i].UCID) > 50)
            {
                players[i].Energy += 500;
                bank->RemCash(players[i].UCID,50);
                bank->AddToBank(50);

            }
            else
            {
                send_mtc(players[i].UCID,msg->GetMessage(players[i].UCID,2001));
            }
        }
        else
        {
            send_mtc(players[i].UCID,msg->GetMessage(players[i].UCID,2002));
        }
    }

    //!redbule
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!redbull", 8) == 0)
    {
        //out << players[i].UName << " send !redbull" << endl;
        if (((players[i].Zone == 1) and (players[i].Energy < 500)) or (players[i].Zone == 3))
        {
            if (bank->GetCash(players[i].UCID) > 100)
            {
                players[i].Energy += 1000;
                bank->RemCash(players[i].UCID,100);
                bank->AddToBank(100);
            }
            else
            {
                send_mtc(players[i].UCID,msg->GetMessage(players[i].UCID,2001));
            }
        }
        else
        {
            send_mtc(players[i].UCID,msg->GetMessage(players[i].UCID,2002));
        }

    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!save", 5) == 0 )
        energy_save(players[i].UCID);

}

void RCEnergy::btn_energy (struct EnergyPlayer *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    // bg
    pack.ClickID = 207;
    pack.BStyle = 32;
    pack.L = 100;
    pack.T = 1;
    pack.W = 35;
    pack.H = 4;
    strcat(pack.Text,"");
    insim->send_packet(&pack);

    pack.ClickID = 209;
    pack.BStyle = 1;
    pack.L = 100;
    pack.T = 1;
    pack.W = 7;
    pack.H = 4;
    if (splayer->Zone == 3)
    {
        strcpy(pack.Text,"^2");
    }
    else
    {
        strcpy(pack.Text,"^1");
    }
    strcat(pack.Text,msg->GetMessage(splayer->UCID,100));
    insim->send_packet(&pack);
    //
    pack.ClickID = 208;
    pack.BStyle = 64;
    pack.L = 107;
    pack.T = 1;
    pack.W = 26;
    pack.H = 4;

    int nrg = splayer->Energy/50 ;


    if (nrg <= 40 and nrg > 0)
        strcpy(pack.Text,"^1");
    else if (nrg <= 140 and nrg > 40)
        strcpy(pack.Text,"^3");
    else
        strcpy(pack.Text,"^2");

    float nrg2 = (splayer->Energy)*100/10000;

    sprintf(pack.Text+2,"%1.1f%%",nrg2);

    insim->send_packet(&pack);

}

int RCEnergy::check_pos(struct EnergyPlayer *splayer)
{
    int PLX = splayer->Info.X/65536;
    int PLY = splayer->Info.Y/65536;

    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
    {
        return 1;
    }

    return 0;
}


int RCEnergy::GetEnergy(byte UCID)
{
    for (int i=0; i<32; i++)
    {
        if (players[i].UCID == UCID)
        {
            return (players[i].Energy*100/10000);
        }
    }
    return 0;
}


void RCEnergy::insim_con()
{
    //printf("Car contact\n");


    struct IS_CON *pack_con = (struct IS_CON*)insim->get_packet();

    for (int i=0; i<MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_con->A.PLID)
        {
            if (!Islocked(players[i].UCID))
                players[i].Energy -= 10 * pack_con->SpClose;

            break;
        }
    }

    for (int j=0; j<MAX_PLAYERS; j++)
    {
        if (players[j].PLID == pack_con->B.PLID)
        {
            if (!Islocked(players[j].UCID))
                players[j].Energy -= 10 * pack_con->SpClose;

            break;
        }
    }



}

void RCEnergy::insim_obh()
{


    struct IS_OBH *pack_obh = (struct IS_OBH*)insim->get_packet();

    if((pack_obh->Index > 45 and pack_obh->Index < 125) or (pack_obh->Index > 140))
    {
        for (int i=0; i<MAX_PLAYERS; i++)
        {
            if (players[i].PLID == pack_obh->PLID)
            {
                if (!Islocked(players[i].UCID))
                    players[i].Energy -=  pack_obh->SpClose;

                break;
            }
        }
    }
}



bool    RCEnergy::Lock(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            players[i].Lock = 1;
            return true;
        }
    }
    return false;
}

bool    RCEnergy::Unlock(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            players[i].Lock = 0;
            return true;
        }
    }
    return false;
}

bool    RCEnergy::Islocked(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            if (players[i].Lock == 1)
                return true;
            else
                return false;
        }
    }
    return false;
}
