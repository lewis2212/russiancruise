using namespace std;

#include "RCBank.h"

time_t  btime;


RCBank::RCBank()
{

}

RCBank::~RCBank()
{

}

int RCBank::init(char *dir,void *CInSim, void *Message)
{
    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        cout << "Can't struct CInsim class" << endl;;
        return -1;
    }

    msg = (RCMessage *)Message;
    if(!msg)
    {
        cout << "Can't struct RCMessage class" << endl;
        return -1;
    }

    return 0;
}

void RCBank::bank_ncn()
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

    char file[MAX_PATH];
    strcpy(file,RootDir);
    strcat(file,"data\\RCBank\\");
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
        players[i].Cash = 1000;
        bank_save(i);
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
            if (strncmp("Cash=",str,5)==0)
            {
                players[i].Cash = atof(str+5);
            }
            // Get Credits
            // Get Deposits
        }
    }


    readf.close();
    }
    FindClose(fff);



    return ;



    //cout << players[i].UName << endl;
    //cout << players[i].PName << endl;
    //cout << (int)players[i].UCID << endl;
}



void RCBank::bank_npl()
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
            //strcpy(players[i].CName ,pack_npl->CName);
        }
    }
}

void RCBank::bank_plp()
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

void RCBank::bank_pll()
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

void RCBank::bank_cnl ()
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
            bank_save(i);
            memset(&players[i],0,sizeof(struct BankPlayer));
            break;
        }
    }
}

void RCBank::bank_save (int j)
{
    // Find player and set the whole player struct he was using to 0

            char file[255];
            strcpy(file,RootDir);
            strcat(file,"data\\RCBank\\");
            strcat(file,players[j].UName);
            strcat(file,".txt");

            ofstream writef (file,ios::out);
            writef << "Cash=" << players[j].Cash << endl;
            writef.close();


    }

void RCBank::bank_crp()
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

void RCBank::btn_cash (int i)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = players[i].UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.BStyle = 32;
    //
    pack.ClickID = 162;
    pack.L = 85;
    pack.T = 1;
    pack.W = 15;
    pack.H = 4;

    char cash[10];
    itoa((int)players[i].Cash,cash,10);
    if (players[i].Cash > 0)
        strcpy(pack.Text,"^2");
    else
        strcpy(pack.Text,"^1");
    strcat(pack.Text,cash);
    strcat(pack.Text,"^7 RUR.");
    insim->send_packet(&pack);
}

// функции-утилиты

int RCBank::check_pos(struct BankPlayer *splayer)
{
    int PLX = splayer->Info.X/65536;
    int PLY = splayer->Info.Y/65536;

    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
    {
        return 1;
    }

    return 0;
}

void RCBank::send_mtc (byte UCID,char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strcpy(pack_mtc.Msg, Msg);
    insim->send_packet(&pack_mtc);
};

void RCBank::send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst);
};


void RCBank::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack);
};


