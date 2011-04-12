using namespace std;

#include "RCStreet.h"

RCStreet::RCStreet()
{

}

RCStreet::~RCStreet()
{

}

int RCStreet::init(char *dir, void *CInSim, void *Message)
{
    cout << "Init Streets" << endl;
    IfInited = false;
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

    char file[255];
    sprintf(file,"%sdata\\RCStreet\\tracks\\%s.txt",RootDir,"SO4");


    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
        return -1;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);

    int i = 0;
    while (readf.good())
    {

        char str[128];
        readf.getline(str,128);
        cout << str << endl;

        if (strlen(str) > 0)
        {

            char * street;
            char * node1;
            char * node2;
            char * speed;
            street = strtok (str,";");
            node1 = strtok (NULL,";");
            node2 = strtok (NULL,";");
            speed = strtok (NULL,";");


            ZeroMemory(&Street[i],sizeof(streets));
            strcpy(Street[i].Street,"^C^7");
            strcat(Street[i].Street,street);
            Street[i].NodeBeg = atoi(node1);
            Street[i].NodeEnd = atoi(node2);
            Street[i].SpeedLimit = atoi(speed);

            i++;

        } // if strlen > 0

    } //while readf.good()
    StreetNums = i;



    readf.close();
    IfInited = true;
    return 0;
}

void RCStreet::next_packet()
{
    switch (insim->peek_packet())
    {
    case ISP_MSO:
        street_mso();
        break;


    case ISP_NPL:
        street_npl();
        break;

    case ISP_NCN:
        street_ncn();
        break;

    case ISP_CNL:
        street_cnl();
        break;

    case ISP_PLL:
        street_pll();
        break;

    case ISP_PLP:
        street_plp();
        break;

    case ISP_CPR:
        street_crp();
        break;


    }
}


void RCStreet::street_cnl ()
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

void RCStreet::street_crp()
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

void RCStreet::street_mci ()
{
    //cout << "Street_mci" << endl;
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++)

    {
        for (int j =0; j < MAX_PLAYERS; j++)
        {

            if (pack_mci->Info[i].PLID == players[j].PLID and players[j].PLID != 0 and players[j].UCID != 0)

            {
                /** streets  **/

                int Node = pack_mci->Info[i].Node;


                for (int g=0; g<StreetNums; g++)
                {
                    if (Node >= Street[g].NodeBeg and Node <= Street[g].NodeEnd )
                    {
                        if (players[j].StreetNum != g)
                        {
                            players[j].StreetNum = g;
                            btn_street(&players[j]);
                        }
                    }
                }


                /**  steets **/

            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }
}

void RCStreet::street_mso ()
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


    }

}


void RCStreet::street_ncn()
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


}

void RCStreet::street_npl()
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

void RCStreet::street_plp()
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

void RCStreet::street_pll()
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

void RCStreet::btn_street (struct Player *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.BStyle = 1;
    pack.ClickID = 50;
    pack.L = 10;
    pack.T = 158;
    pack.W = 33;
    pack.H = 6;
    strcpy(pack.Text,Street[splayer->StreetNum].Street);
    insim->send_packet(&pack);

    pack.BStyle = 64;
    pack.ClickID = 51;
    pack.L = 30;
    pack.T = 130;
    pack.W = 80;
    pack.H = 80;
    strcpy(pack.Text,"^1•");
    insim->send_packet(&pack);

    pack.ClickID = 52;
    pack.L = 35;
    pack.T = 140;
    pack.W = 61;
    pack.H = 61;
    strcpy(pack.Text,"^7•");
    insim->send_packet(&pack);

    pack.ClickID = 53;
    pack.BStyle = 1;
    pack.L = 44;
    pack.T = 166;
    pack.W = 15;
    pack.H = 14;
    sprintf(pack.Text,"^0%d",Street[splayer->StreetNum].SpeedLimit);
    insim->send_packet(&pack);
}

void RCStreet::send_mtc (byte UCID,char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strcpy(pack_mtc.Msg, Msg);
    insim->send_packet(&pack_mtc);
}

void RCStreet::send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst);
}

void RCStreet::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack);
}

char *RCStreet::CurentStreet (byte UCID)
{
    for (int i=0;i<MAX_PLAYERS;i++)
    {
        if (players[i].UCID == UCID)
        {
            return Street[players[i].StreetNum].Street;
        }
    }
    return "";
}
int RCStreet::CurentStreetNum(byte UCID)
{
    for (int i=0;i<MAX_PLAYERS;i++)
    {
        if (players[i].UCID == UCID)
        {
            return players[i].StreetNum;
        }
    }
    return -1;
}
int RCStreet::CurentStreetInfo(void *StreetInfo, int StreetNum)
{
    if (memcpy(StreetInfo,&Street[StreetNum],sizeof(streets)))
        return 1;
    return 0;
}
int RCStreet::StreetCount()
{
    return StreetNums;
}




