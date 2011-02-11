using namespace std;

#include "RCDrivingLicense.h"

RCDL::RCDL()
{

}

RCDL::~RCDL()
{

}

int     RCDL::GetLVL(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            return players[i].LVL;
        }
    }
    return 0;
}
int     RCDL::GetSkill(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            return players[i].Skill;
        }
    }
    return 0;
}

bool    RCDL::AddSkill(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            players[i].Skill += 800;
            return true;
        }
    }
    return false;
}

bool    RCDL::RemSkill(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            players[i].Skill -= 1000;
            return true;
        }
    }
    return false;
}

int RCDL::init(char *dir,void *CInSim, void *Message)
{
    strcpy(RootDir,dir);

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

    inited = 1;

    return 0;
}

void RCDL::next_packet()
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

void RCDL::ncn()
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

    /** read file **/

    char file[MAX_PATH];
    strcpy(file,RootDir);
    strcat(file,"data\\RCDrivingLicense\\");
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
        printf("Can't find %s\n Create File for user",file);
        players[i].LVL = 0;
        save(players[i].UCID);
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
                if (strncmp("LVL=",str,4)==0)
                {
                    players[i].LVL = atoi(str+4);
                }

                if (strncmp("Skill=",str,6)==0)
                {
                    players[i].Skill = atoi(str+6);
                }
                // Get Credits
                // Get Deposits
            }
        }



        readf.close();
    }
    FindClose(fff);

}

void RCDL::npl()
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
        }
    }
}

void RCDL::plp()
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
            memset(&players[i].Info,0,sizeof(CompCar));
            break;
        }
    }
}

void RCDL::pll()
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
            memset(&players[i].Info,0,sizeof(CompCar));
            break;
        }
    }
}

void RCDL::cnl()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            /** save file**/
            save(players[i].UCID);

            memset(&players[i],0,sizeof(struct DLPlayer));
            break;
        }
    }
}

void RCDL::crp()
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

void RCDL::mso()
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



    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!save", 5) == 0 )
    {
        cout << players[i].UName << " send !save" << endl;


        save(players[i].UCID);

    }
}

void RCDL::save (byte UCID)
{
    // Find player and set the whole player struct he was using to 0

     for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {

    char file[255];
    strcpy(file,RootDir);
    strcat(file,"data\\RCDrivingLicense\\");
    strcat(file,players[i].UName);
    strcat(file,".txt");

    ofstream writef (file,ios::out);
    writef << "LVL=" << players[i].LVL << endl;
    writef << "Skill=" << players[i].Skill << endl;
    writef.close();

    break;

        }
    }

}

void RCDL::mci()
{
    //cout << "pizza_mci" << endl;
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



                int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

                int X1 = players[j].Info.X/65536;
                int Y1 = players[j].Info.Y/65536;
                int Z1 = players[j].Info.Z/65536;

                //cout << X1 << endl;

                if (X1==0 and Y1==0 and Z1==0)
                {
                    X1=X;
                    Y1=Y;
                    Z1=Z;
                    memcpy(&players[j].Info,&pack_mci->Info[i],sizeof(CompCar));
                }

                //cout << X1 << endl;

                float Skill = sqrt(pow((X-X1),2)+pow((Y-Y1),2)+pow((Z-Z1),2));
                //cout << Skill << endl;
                if ((abs((int)Skill) > 10) and (S>30))
                {
                    players[j].Skill += abs((int)Skill);
                    memcpy(&players[j].Info,&pack_mci->Info[i],sizeof(CompCar));
                }

                /** next lvl **/

                float nextlvl = (pow(players[j].LVL,2)*0.5+100)*1000;

                if (players[j].Skill > nextlvl)
                {
                    players[j].LVL ++;
                    players[j].Skill = 0;
                    char Msg[64];
                    sprintf(Msg,"User:%s LVL=%d \tSkill=%d \n",players[j].UName, players[j].LVL, players[j].Skill);
                    //send_mst(Msg);
                }

                /** buttons **/

                //printf("User:%s LVL=%d \tSkill=%d \n",players[j].UName, players[j].LVL, players[j].Skill);
                btn_dl(&players[j]);


            }
        }
    }

}

void RCDL::btn_dl(struct DLPlayer *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    // BG
    pack.ClickID = 230;
    pack.BStyle = 32;
    pack.L = 100;
    pack.T = 5;
    pack.W = 35;
    pack.H = 8;
    strcat(pack.Text,"");
    insim->send_packet(&pack);
    // LVL
    pack.ClickID = 231;
    pack.BStyle = 64;
    pack.L = 101;
    pack.T = 5;
    pack.W = 33;
    pack.H = 4;
    //int life = 100;
    sprintf(pack.Text,"^7^CDrive Level: %d", splayer->LVL);
    insim->send_button(&pack);

    pack.ClickID = 232;
    pack.BStyle = 64;
    pack.L = 101;
    pack.T = 9;
    pack.W = 33;
    pack.H = 4;
    //int life = 100;
    sprintf(pack.Text,"^7^CSkill");
    insim->send_button(&pack);

    pack.ClickID = 233;
    pack.BStyle = 1;
    pack.L = 107;
    pack.T = 10;
    pack.W = 26;
    pack.H = 2;

    float nextlvl = (pow(splayer->LVL,2)*0.5+100)*1000;

    char Text[64];
    int skl = (splayer->Skill/nextlvl)*100;

    sprintf(Text,"^4|^7 %s take new level %d",splayer->PName, splayer->LVL);

    int sklend = 100 - skl;
    strcpy(pack.Text,"^2");

    for (int i=0; i< skl; i++)
    {
        strcat(pack.Text,"||");
    }

    if (sklend > 0)
    {
        strcat(pack.Text,"^7");

        for (int i=0; i<sklend; i++)
        {
            strcat(pack.Text,"||");
        }

    }

    insim->send_button(&pack);

}

void RCDL::send_mtc (byte UCID,char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strcpy(pack_mtc.Msg, Msg);
    insim->send_packet(&pack_mtc);
};

void RCDL::send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst);
};


void RCDL::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack);
};
/*
void RCDL::btn_lic (struct DLPlayer *splayer)
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
    pack.ClickID = 212;
    pack.BStyle = 32;
    pack.L = 92;
    pack.T = 5;
    pack.W = 8;
    pack.H = 4;


    pack.ClickID = 213;
    pack.BStyle = 32+64;
    pack.L = 100;
    pack.T = 5;
    pack.W = 48;
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
*/
