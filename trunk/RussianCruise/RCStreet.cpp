using namespace std;

#include "RCStreet.h"

RCStreet::RCStreet()
{

	players = new StrPlayer[MAX_PLAYERS];
	memset(players, 0, sizeof( StrPlayer ) * MAX_PLAYERS );
}

RCStreet::~RCStreet()
{
	delete[] players;
}

int RCStreet::init(const char *dir, void *CInSim, void *Message)
{
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



void RCStreet::insim_cnl( struct IS_CNL* packet )
{
    int i;
    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {
            memset(&players[i],0,sizeof(struct StrPlayer));
            break;
        }
    }
}

void RCStreet::insim_cpr( struct IS_CPR* packet )
{
    int i;
    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {

            strcpy(players[i].PName, packet->PName);
            break;
        }
    }
}

void RCStreet::insim_mci ( struct IS_MCI* pack_mci )
{
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

void RCStreet::insim_mso( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
        return;
}


void RCStreet::insim_ncn( struct IS_NCN* packet )
{
    int i;

    if (packet->UCID == 0)
        return;


    for (i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == 0)
            break;

    if (i == MAX_PLAYERS)
        return;

    strcpy(players[i].UName, packet->UName);
    strcpy(players[i].PName, packet->PName);
    players[i].UCID = packet->UCID;
}

void RCStreet::insim_npl( struct IS_NPL* packet )
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {
            players[i].PLID = packet->PLID;
            players[i].StreetNum = 250;
        }
    }
}

void RCStreet::insim_plp( struct IS_PLP* packet)
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == packet->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCStreet::insim_pll( struct IS_PLL* packet )
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == packet->PLID)
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
    pack.L = 140;
    pack.T = 1;
    pack.W = 60;
    pack.H = 6;
    sprintf(pack.Text, "%s ^2(^1%d ^C^7км/ч^2)",Street[splayer->StreetNum].Street,Street[splayer->StreetNum].SpeedLimit);
    insim->send_packet(&pack);
}

int RCStreet::CurentStreetNum(byte UCID)
{
    for (int i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == UCID)
            return players[i].StreetNum;

    return -1;
}

int RCStreet::CurentStreetInfo(void *StreetInfo, byte UCID)
{
    for (int i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == UCID)
            if (memcpy(StreetInfo,&Street[players[i].StreetNum],sizeof(streets)))
                return 1;

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
