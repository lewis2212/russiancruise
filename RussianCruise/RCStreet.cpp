using namespace std;

#include "RCStreet.h"

RCStreet::RCStreet()
{

}

RCStreet::~RCStreet()
{

}

int RCStreet::init(const char *dir, void *CInSim, void *Message)
{
    //cout << "Init Streets" << endl;
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

    IfInited = true;
    return 0;
}

void RCStreet::readconfig(const char *Track)
{
    char file[255];
    sprintf(file,"%sdata\\RCStreet\\tracks\\%s.txt",RootDir,Track);


    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);

    int i = 0;
    while (readf.good())
    {

        char str[128];
        readf.getline(str,128);
        //cout << str << endl;

        if (strlen(str) > 0)
        {
            if (strcmp(str,"#Street")==0)
            {
                /**
                название
                ограничен скорости.
                кол. точек
                ...точки....
                */
                //онуляем позицию
                ZeroMemory(&Street[i],sizeof(streets));
                // читаем название улицы
                readf.getline(str,128);
                sprintf(Street[i].Street,"^C^7%s",str);

                readf.getline(str,128);
                Street[i].SpeedLimit = atoi(str);

                // читаем количество точек
                readf.getline(str,128);
                Street[i].PointCount = atoi(str);

                // объявляем массих точек Х and Y
                Street[i].StreetX = new int[Street[i].PointCount];
                Street[i].StreetY = new int[Street[i].PointCount];

                for (int k=0; k<Street[i].PointCount; k++)
                {
                    readf.getline(str,128);

                    Street[i].StreetX[k] = atoi(strtok (str,";"));
                    Street[i].StreetY[k] = atoi(strtok (NULL,";"));
                }

                i++;
            }

        } // if strlen > 0

    } //while readf.good()
    StreetNums = i;

    readf.close();
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
            memset(&players[i],0,sizeof(struct StrPlayer));
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

                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;



                for (int g=0; g<StreetNums; g++)
                {
                    if(Check_Pos(Street[g].PointCount,Street[g].StreetX,Street[g].StreetY,X,Y))
                        //if (Node >= Street[g].NodeBeg and Node <= Street[g].NodeEnd )
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

void RCStreet::btn_street (struct StrPlayer *splayer)
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
    pack.L = 5;
    pack.T = 158;
    pack.W = 60;
    pack.H = 6;
    sprintf(pack.Text, "%s ^2(^1%d ^C^7км/ч^2)",Street[splayer->StreetNum].Street,Street[splayer->StreetNum].SpeedLimit);
    insim->send_packet(&pack);
}

void RCStreet::send_mtc (byte UCID,const char* Msg)
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
}

void RCStreet::send_mst (const char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst,errmsg);
}

void RCStreet::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack,errmsg);
}


int RCStreet::CurentStreetNum(byte UCID)
{
    for (int i=0; i<MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            // printf("RCStreet: CurentStreetNum for %s == %d\n",players[i].UName,players[i].StreetNum);
            return players[i].StreetNum;
        }
    }
    return -1;
}
int RCStreet::CurentStreetInfo(void *StreetInfo, byte UCID)
{
    for (int i=0; i<MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            if (memcpy(StreetInfo,&Street[players[i].StreetNum],sizeof(streets)))
                return 1;
        }
    }

    return -1;
}

int RCStreet::CurentStreetInfoByNum(void *StreetInfo, int StrNum)
{
    if (memcpy(StreetInfo,&Street[StrNum],sizeof(streets)))
        return 1;
    return -1;
}
int RCStreet::StreetCount()
{
    return StreetNums;
}




