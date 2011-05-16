using namespace std;

#include "RCBank.h"

time_t  btime;
pthread_t bid; // Thread ID

void *bankthread (void *arg)
{
    RCBank *bnk = (RCBank *)arg; //struct our RCBank class in thread
    if(!bnk)
    {
        printf ("Can't start bankthread");
        return 0;
    }

    cout << "\tthread \"Bank\" started" << endl;


    for (;;)
    {
        for (int i=0; i<MAX_PLAYERS; i++)
        {
            if (bnk->GetPlayerUCID(i) != 0)
                bnk->btn_cash(i);
        }
        Sleep(2000);
    }
}


RCBank::RCBank()
{

}

RCBank::~RCBank()
{

}

byte RCBank::GetPlayerUCID (int i)
{
    if (players[i].UCID != 0)
        return players[i].UCID;

    return 0;
}

int RCBank::GetCash(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            return (int)players[i].Cash;
        }
    }
    return 0;
}

bool    RCBank::AddCash(byte UCID, int Cash)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            players[i].Cash += Cash;
            return true;
        }
    }
    return false;
}

bool    RCBank::RemCash(byte UCID, int Cash)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            players[i].Cash -= Cash;
            return true;
        }
    }
    return false;
}

bool RCBank::AddToBank(int Cash)
{
    BankFond += Cash;
    return true;
}

bool RCBank::RemFrBank(int Cash)
{
    BankFond -= Cash;
    return true;
}

int RCBank::init(char *dir,void *CInSim, void *GetMessage,void *Bank)
{

    pthread_cancel(bid);
    if (pthread_create(&bid,NULL,bankthread,Bank) < 0)
        return -1;

    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        cout << "Can't struct CInsim class" << endl;;
        return -1;
    }

    msg = (RCMessage *)GetMessage;
    if(!msg)
    {
        cout << "Can't struct RCMessage class" << endl;
        return -1;
    }

    return 0;
}

/** Функции повторители основного ядра  **/

void RCBank::next_packet()
{
    switch (insim->peek_packet())
    {
    case ISP_NPL:
        bank_npl();
        break;

    case ISP_NCN:
        bank_ncn();
        break;

    case ISP_CNL:
        bank_cnl();
        break;

    case ISP_PLL:
        bank_pll();
        break;

    case ISP_PLP:
        bank_plp();
        break;

    case ISP_CPR:
        bank_crp();
        break;

    }
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
        bank_save(players[i].UCID);
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
            bank_save(players[i].UCID);
            memset(&players[i],0,sizeof(struct BankPlayer));
            break;
        }
    }
}

void RCBank::bank_save (byte UCID)
{
    // Find player and set the whole player struct he was using to 0

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {

            char file[255];
            strcpy(file,RootDir);
            strcat(file,"data\\RCBank\\");
            strcat(file,players[i].UName);
            strcat(file,".txt");

            ofstream writef (file,ios::out);
            writef << "Cash=" << players[i].Cash << endl;
            writef.close();

            char fileb[255];
            strcpy(fileb,RootDir);
            strcat(fileb,"data\\RCBank\\_RC_Bank_Capital_.txt");
            ofstream writeb (fileb,ios::out);
            writeb << "Capital=" << BankFond << endl;
            writeb.close();
        }
    }



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
    pack.L = 70;
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
    insim->send_packet(&pack,errmsg);

    if (strcmp(players[i].UName,"denis-takumi") == 0)
    {
        pack.ClickID = 163;
        pack.L = 65;
        pack.T = 5;
        pack.W = 20;
        pack.H = 4;

        char cash[10];
        sprintf(cash,"%.0f",BankFond);
        //itoa((int)BankFond,cash,10);
        if (BankFond > 0)
            strcpy(pack.Text,"^2");
        else
            strcpy(pack.Text,"^1");
        strcat(pack.Text,cash);
        strcat(pack.Text,"^7 RUR.");
        insim->send_packet(&pack,errmsg);
    }
}

void RCBank::readconfig(char *Track)
{
    char file[255];
    strcpy(file,RootDir);
    strcat(file,"data\\RCBank\\_RC_Bank_Capital_.txt");

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

        if (strlen(str) > 0)
        {
            if (strncmp(str,"Capital=",8)==0)
            {
                BankFond = atof(str+8);
            }
        }
    }

    readf.close();

    /**char file[255];
    strcpy(file,RootDir);
    strcat(file,"data\\RCBank\\maps\\");
    strcat(file,Track);
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
                TrackInf.BankCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,";");
                    Y = strtok (NULL,";");
                    TrackInf.XBank[i] = atoi(X);
                    TrackInf.YBank[i] = atoi(Y);
                }
            }

        } // if strlen > 0
    } //while readf.good()

    readf.close();**/
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
    char errmsg[64];
    ZeroMemory(&errmsg,64);
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strncpy(pack_mtc.Text, Msg,strlen(Msg));
    if (!insim->send_packet(&pack_mtc,errmsg))
        cout << errmsg << endl;
};

void RCBank::send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst,errmsg);
};


void RCBank::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack,errmsg);
};


