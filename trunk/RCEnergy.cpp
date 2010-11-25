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

    printf("\tthread \"Buttons\" started\n");
    while (ok > 0)
    {
        for (int i=0; i<MAX_PLAYERS; i++)
        {
            if (nrg->players[i].UCID != 0)
            {
                //printf("Send btn to %s\n",nrg->players[i].UName);
                nrg->btn_energy(&nrg->players[i]);

            }
        }
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

int RCEnergy::init(char *dir,void *classname,void *CInSim, void *Message,void *Bank)
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


void RCEnergy::readconfig(char *Track)
{
    char file[255];
    strcpy(file,RootDir);
    //strcat(file,"misc\\mysql.cfg");
    strcat(file,"data\\RCEnergy\\maps\\");
    strcat(file,Track);
    //strcat(file,".");
    //strcat(file,splayer->Lang);
    strcat(file,".txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        //out << "Can't find " << file << endl;
        return;
    }
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

/** функции-повторители основных фунцкий ядра **/


void RCEnergy::energy_ncn()
{
    printf("New player connect\n");
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

    char file[MAX_PATH];
    strcpy(file,RootDir);
    strcat(file,"data\\RCEnergy\\");
    strcat(file,players[i].UName);
    strcat(file,".txt");
    //strcat(file,"tracks\\.txt");

    // Read Data From File

    // Try Find New File
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf("Can't find %s\n Cteat file user",file);
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
            // Get Cash
            if (strncmp("Energy=",str,7)==0)
            {
                //cout << "We Find Energy" << endl;
                players[i].Energy = atoi(str+7);
            }
            // Get Credits
            // Get Deposits
        }
    }


    readf.close();
    }
    FindClose(fff);



}

void RCEnergy::energy_npl()
{
    //cout << "joining race or leaving pits" << endl;
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
            //strcpy(players[i].CName ,pack_npl->CName);

            if ( players[i].Energy < 500 )
            {

                send_mtc(players[i].UCID,msg->message[0][2402]);
                send_mtc(players[i].UCID,msg->message[0][2403]);
                players[i].Zone = 1;
                players[i].PLID = 0;
                send_mst(Text);
                return;
            }
        }
    }
}

void RCEnergy::energy_plp()
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

void RCEnergy::energy_pll()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_pll->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCEnergy::energy_cnl ()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            if (pack_cnl->Reason != LEAVR_DISCO)
            {
                //players[i].cash += 500;
            }

            energy_save(i);

            memset(&players[i],0,sizeof(struct EnergyPlayer));
            break;
        }
    }
}

void RCEnergy::energy_save (int j)
{
    // Find player and set the whole player struct he was using to 0

            char file[255];

            strcpy(file,RootDir);
            strcat(file,"data\\RCEnergy\\");
            strcat(file,players[j].UName);
            strcat(file,".txt");

            ofstream writef (file,ios::out);
            writef << "Energy=" << players[j].Energy << endl;
            writef.close();


    }




void RCEnergy::energy_crp()
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

void RCEnergy::energy_mci ()
{

    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++)

    {
        for (int j =0; j < MAX_PLAYERS; j++)
        {

            if (pack_mci->Info[i].PLID == players[j].PLID and players[j].PLID != 0 and players[j].UCID != 0)

            {
                //printf("UName: %s\n",players[j].UName);

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

                //int K = int(sqrt(abs((dD-dH)*(1+dA)*dS))/32);
                int K = (int)sqrt(abs((dD-dH)*(1+dA)*dS))/8;


                if ((players[j].Energy > 5) and (S > 5))
                {
                    players[j].Energy -= K;
                    //cout << "nrg = " << players[j].Energy << endl;
                }


                if (Check_Pos(TrackInf.CafeCount,TrackInf.XCafe,TrackInf.YCafe,X,Y))
                {
                    players[j].Zone = 3;
                }
                else
                {
                    players[j].Zone = 0;
                }

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

                if (X1==0 and Y1==0 and Z1==0)
                {
                    X1=X;
                    Y1=Y;
                    Z1=Z;
                }


                players[j].Info.X = pack_mci->Info[i].X;
                players[j].Info.Y = pack_mci->Info[i].Y;
                players[j].Info.Node = pack_mci->Info[i].Node;
                players[j].Info.Direction = pack_mci->Info[i].Direction;
                players[j].Info.Heading = pack_mci->Info[i].Heading;
                players[j].Info.Speed = pack_mci->Info[i].Speed;
                players[j].Info.AngVel = pack_mci->Info[i].AngVel;

                if (players[j].Energy < 10)
                {
                    char Text[64];
                    strcpy(Text, "/spec ");
                    strcat (Text, players[j].UName);
                    //send_mtc(players[j].UCID,msg->message[players[j].lang_id][1800]);
                    //send_mtc(players[j].UCID,msg->message[players[j].lang_id][1801]);
                    //send_mtc(players[j].UCID,msg->message[players[j].lang_id][1802]);
                    players[j].PLID = 0;
                    players[j].Zone = 1;
                    send_mst(Text);
                }





            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }
}


void RCEnergy::energy_mso ()
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
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!coffee", 7) == 0)
    {
        //out << players[i].UName << " send !coffee" << endl;
        if (((players[i].Zone == 1) and (players[i].Energy < 500)) or (players[i].Zone == 3))
        {
             if (bank->players[i].Cash > 50)
             {
                 players[i].Energy += 500;
                 bank->players[i].Cash -= 50;
             }
             else
             {
                 send_mtc(players[i].UCID,msg->message[0][2001]);
             }
        }
        else
        {
            send_mtc(players[i].UCID,msg->message[0][2002]);
        }
    }
    //!redbule
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!redbull", 8) == 0)
    {
        //out << players[i].UName << " send !redbull" << endl;
        if (((players[i].Zone == 1) and (players[i].Energy < 500)) or (players[i].Zone == 3))
        {
             if (bank->players[i].Cash > 100)
             {
                 players[i].Energy += 1000;
                 bank->players[i].Cash -= 100;
             }
             else
             {
                 send_mtc(players[i].UCID,msg->message[0][2001]);
             }
        }
        else
        {
            send_mtc(players[i].UCID,msg->message[0][2002]);
        }

    }

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
    //
    pack.ClickID = 209;
    pack.BStyle = 32;
    pack.L = 100;
    pack.T = 7;
    pack.W = 8;
    pack.H = 4;
    strcpy(pack.Text,msg->message[0][100]);
    insim->send_packet(&pack);
    //
    pack.ClickID = 208;
    pack.BStyle = 32+64;
    pack.L = 108;
    pack.T = 7;
    pack.W = 42;
    pack.H = 4;
    //int life = 100;
    strcpy(pack.Text,"");

    //cout << "nrg = " << splayer->Energy/100 << endl;

    for (int i=1; i<=splayer->Energy/100; i++)
    {
        if (i == 1)
            strcat(pack.Text,"^1");
        if (i == 20)
            strcat(pack.Text,"^3");
        if (i == 70)
            strcat(pack.Text,"^2");

        strcat(pack.Text,"|");
    }

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

void RCEnergy::send_mtc (byte UCID,char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strcpy(pack_mtc.Msg, Msg);
    insim->send_packet(&pack_mtc);
};

void RCEnergy::send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst);
};


void RCEnergy::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack);
};
