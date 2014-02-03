using namespace std;

#include "RCStreet.h"

RCStreet::RCStreet(const char* Dir)
{
    strcpy(RootDir,Dir);
    memset(&Street, 0, sizeof( struct streets ) * MAX_STREETS );
}

RCStreet::~RCStreet()
{
}

int RCStreet::init(MYSQL *conn, CInsim *InSim, void *Message)
{
    IfInited = false;

    dbconn = conn;
    if (!dbconn)
    {
        printf("RCStreet: Can't sctruct MySQL Connector\n");
        return -1;
    } // Копируем путь до программы

    insim = InSim; // Присваиваем указателю область памяти
    if (!insim) // Проверяем на существование
    {
        printf ("RCStreet: Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)Message;
    if (!msg)
    {
        printf ("Can't struct RCMessage class");
        return -1;
    }

    IfInited = true;
    return 0;
}

const char* RCStreet::GetStreetName(byte UCID, int StreetID)
{
    if (StreetID>=0)
    {
        if (msg->GetLang(UCID) == "eng")
            return Street[StreetID].StreetEn;
        else
            return Street[StreetID].StreetRu;
    }
    else return "NULL";
}

void RCStreet::ReadConfig(const char *Track)
{
    char file[255];
    sprintf(file, "%s\\data\\RCStreet\\tracks\\%s.txt", RootDir, Track);
    // TODO: refactoring
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file, &fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
    	CCText("  ^7RCStreet   ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }
    FindClose(fff);

    ifstream readf (file, ios::in);

    int i = 0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);

        if (strlen(str) > 0)
        {
            if (strcmp(str, "#Street") == 0)
            {
                /**
                название (рус)
                название (енг)
                ограничен скорости.
                кол. точек
                ...точки....
                */
                //онуляем позицию
                memset(&Street[i], 0, sizeof(streets));
                Street[i].StreetID = i;
                // читаем название улицы
                readf.getline(str, 128);
                sprintf(Street[i].StreetRu, "^C^7%s", str);
                // читаем название улицы
                readf.getline(str, 128);
                sprintf(Street[i].StreetEn, "^C^7%s", str);

                readf.getline(str, 128);
                Street[i].SpeedLimit = atoi(str);

                // читаем количество точек
                readf.getline(str, 128);
                Street[i].PointCount = atoi(str);

                if ( Street[i].StreetX != NULL )
                    delete[] Street[i].StreetX;

                if ( Street[i].StreetY != NULL )
                    delete[] Street[i].StreetY;
                // объявляем массих точек Х and Y
                Street[i].StreetX = new int[Street[i].PointCount];
                Street[i].StreetY = new int[Street[i].PointCount];

                for (int k=0; k<Street[i].PointCount; k++)
                {
                    readf.getline(str, 128);

                    Street[i].StreetX[k] = atoi(strtok (str, ";"));
                    Street[i].StreetY[k] = atoi(strtok (NULL, ";"));
                }

                i++;
            }
        }
    }
    StreetNums = i;

    readf.close();

    CCText("  ^7RCStreet\t^2OK");
}

void RCStreet::InsimCNL( struct IS_CNL* packet )
{
    players.erase(packet->UCID);
}

void RCStreet::InsimCPR( struct IS_CPR* packet )
{
    strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCStreet::InsimMCI ( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
        byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];

        int X = pack_mci->Info[i].X/65536;
        int Y = pack_mci->Info[i].Y/65536;

        for (int g = 0; g < StreetNums; g++)
        {
            if ( Check_Pos(Street[g].PointCount, Street[g].StreetX, Street[g].StreetY, X, Y) )
            {
                if (players[ UCID ].StreetNum != g)
                {
                    players[ UCID ].StreetNum = g;
                    break;
                }
            }
        }
        BtnStreet( UCID );
    }
}

void RCStreet::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
        return;
}


void RCStreet::InsimNCN( struct IS_NCN* packet )
{
    int i;

    if (packet->UCID == 0)
        return;

    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCStreet::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[ packet->PLID ] = packet->UCID;
    players[ packet->UCID ].StreetNum = DEFAULT_STREET_NUM;
}

void RCStreet::InsimPLP( struct IS_PLP* packet)
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCStreet::InsimPLL( struct IS_PLL* packet )
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCStreet::BtnStreet (byte UCID)
{
    if (players[ UCID ].StreetNum > StreetCount())
        return;

    SendButton(255, UCID, 50, 170, 8, 29, 6, 32, GetStreetName(UCID, players[UCID].StreetNum));
    SendButton(255, UCID, 51, 156, 2, 21, 19, 0, "^1^SЎр");
    SendButton(255, UCID, 52, 156, 7, 21, 8, 3, ToString(Street[ players[ UCID ].StreetNum ].SpeedLimit));
}

int RCStreet::CurentStreetNum(byte UCID)
{
    return players[ UCID ].StreetNum;
}

int RCStreet::CurentStreetInfo(void *StreetInfo, byte UCID)
{
    if ( players[ UCID ].StreetNum == DEFAULT_STREET_NUM )
        return -1;

    if (memcpy(StreetInfo, &Street[ players[ UCID ].StreetNum ], sizeof(streets)))
        return 1;
}

int RCStreet::CurentStreetInfoByNum(void *StreetInfo, int StrNum)
{
    if (memcpy(StreetInfo, &Street[StrNum], sizeof(streets)))
        return 1;

    return -1;
}

int RCStreet::StreetCount()
{
    return StreetNums;
}
