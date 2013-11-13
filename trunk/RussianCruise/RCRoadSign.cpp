#include "RCRoadSign.h"

RCRoadSign::RCRoadSign(const char* Dir)
{
    strcpy(RootDir, Dir);
}
RCRoadSign::~RCRoadSign()
{

}

int RCRoadSign::Init(MYSQL *conn, CInsim *InSim, void *Message, void * Light)
{


    dbconn = conn;
    if (!dbconn)
    {
        printf("RCRoadSign: Can't sctruct MySQL Connector\n");
        return -1;
    }

    insim = InSim;
    if (!insim)
    {
        printf ("RCRoadSign: Can't struct CInsim class");
        return -1;
    }

    lgh = (RCLight *)Light;
    if (!lgh)
    {
        printf ("Can'Top struct RCLight class");
        return -1;
    }

    return 0;
}

void RCRoadSign::ReadConfig(const char *Track)
{
    cout << "RCRoadSign::readconfig\n" ;
    strcpy(TrackName, Track);
    Sign.clear();
    char file[MAX_PATH];
    sprintf(file, "%s\\data\\RCRoadSign\\tracks\\%s.txt", RootDir, Track);
    // TODO: refactoring
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file, &fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "RCRoadSign: Can'Top find " << file << endl;
        return ;
    }
    FindClose(fff);

    ifstream readf (file, ios::in);
    int i=0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);
        if (strlen(str) > 0)
        {
            int pr = atoi(strtok(str, " "));
            if (pr!=0)
            {
                Signs sgn;
                sgn.ID = pr;
                sgn.X = atoi(strtok(NULL, " "));
                sgn.Y = atoi(strtok(NULL, " "));
                sgn.Heading = atoi(strtok(NULL, " "));
                Sign.push_back( sgn );
                //printf("%d %d %d %d\n", Sign[i].X, Sign[i].Y, Sign[i].Heading, Sign[i].ID);
                i++;
            }
        }
    }
    readf.close();

    sprintf(file, "%s\\data\\RCRoadSign\\sign.txt", RootDir);
    // TODO: refactoring
    HANDLE ff;
    // WIN32_FIND_DATA fd;
    ff = FindFirstFile(file, &fd);
    if (ff == INVALID_HANDLE_VALUE)
    {
        printf ("RCRoadSign: Can'Top find \n%s", file);
        return;
    }
    FindClose(ff);

    ifstream read (file, ios::in);
    while (read.good())
    {
        char str[128];
        read.getline(str, 128);

        if (strlen(str) > 0)
        {
            int pr = atoi(strtok(str, ";"));
            if (pr!=0)
            {
                SignName[i].ID = pr;
                sprintf(SignName[i].Name, "^C%s", strtok(NULL, ";"));

                //printf("%d %s\n", SignName[i].ID, SignName[i].Name);
                i++;
            }
        }
    }
    read.close();

    return;
}

void RCRoadSign::InsimNCN( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
    {
        return;
    }

    strcpy(players[packet->UCID].UName, packet->UName);
    strcpy(players[packet->UCID].PName, packet->PName);
}

void RCRoadSign::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[packet->PLID] = packet->UCID;
}

void RCRoadSign::InsimPLP( struct IS_PLP* packet )
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCRoadSign::InsimPLL( struct IS_PLL* packet )
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCRoadSign::InsimCNL( struct IS_CNL* packet )
{
    players.erase( packet->UCID );
}

void RCRoadSign::InsimCPR( struct IS_CPR* packet )
{
    strcpy( players[ packet->UCID ].PName, packet->PName );
}

void RCRoadSign::InsimMSO(struct IS_MSO* packet)
{
    byte UCID = packet->UCID;

    if ( UCID == 0 )
    {
        return;
    }

    if ( packet->UserType != MSO_PREFIX )
    {
        return;
    }

    char Msg[128];
    strcpy( Msg, packet->Msg + ((unsigned char)packet->TextStart));



    if ( strncmp(Msg, "!s_add", 6) == 0 and (strcmp(players[UCID].UName, "Lexanom") == 0 or strcmp(players[UCID].UName, "denis-takumi") == 0 or strcmp(players[UCID].UName, "Max1548") == 0))
    {
        char file[MAX_PATH], text[96];

        sprintf(file, "%s\\data\\RCRoadSign\\tracks\\%s.txt", RootDir, TrackName);

        int X = players[UCID].Info.X / 65536;
        int Y = players[UCID].Info.Y / 65536;
        int Dir = players[UCID].Info.Heading / 182;

        int ID=0;

        strtok(Msg, " ");
        int s = atoi(strtok(NULL, " "));

        if (s == 0)
        {
            sprintf(text, "^1Error: ^Cукажи ID");
            SendMTC(UCID, text);
            return;
        }
        else
        {
            ID = s;
        }

        sprintf(text, "%d %d %d %d", ID, X, Y, Dir);

        ofstream listSing( file , ios::app );
        listSing << text <<  endl;
        listSing.close();

        sprintf(text, "^2Sign added (ID = %d, Dir = %d)", ID, Dir);
        SendMTC(UCID, text);
    }
}

void RCRoadSign::ShowSign(byte UCID, byte ID, byte Count)
{
    byte ClickID = 90, Left = 145, Top = 0+27 * Count, Width = 46, Height = 0;
    if (lgh->GetOnLight(UCID))
    {
        Top+=44;
        Height=52;
    }
    //подвес
    SendButton(255, UCID, ClickID++, Left, 0+Height, 1, 15+27 * Count, 32, "");
    SendButton(255, UCID, ClickID++, Left, 0+Height, 1, 15+27 * Count, 32, "");
    SendButton(255, UCID, ClickID++, Left, 0+Height, 1, 15+27 * Count, 32, "");
    ClickID = 93+10 * Count;

    /** главна€ дорога 2.1 **/
    if (ID == 1)
    {
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБЯ");
        Width-=2;
        Top+=1;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^JБЯ");
        Width-=10;
        Top+=5;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБЯ");
        Width-=2;
        Top+=1;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^3^JБЯ");
    }
    /** уступи дорогу 2.4 **/
    if (ID == 2)
    {
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБ•");
        Width-=2;
        Top+=1;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^1^JБ•");
        Width-=12;
        Top+=6;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top-1, Width, Width, 0, "^7^JБ•");
    }
    /** иск. неровность 1.17 **/
    if (ID == 3)
    {
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБ£");
        Width-=2;
        Top+=1;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^1^JБ£");
        Width-=12;
        Top+=6;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top+1, Width, Width, 0, "^7^JБ£");
        Width-=14;
        Top+=16;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБ№");
        Width+=6;
        Top-=13;
        SendButton(255, UCID, ClickID++, Left-Width / 2+1, Top, Width-1, Width, 0, "^0^JБQ");
    }
    /** пешеходный переход 1.22 **/
    if (ID == 4)
    {
    }
    /** дорожные работы 1.25 **/
    if (ID == 5)
    {
    }
    /** проезд запрещен 3.1 **/
    if (ID == 6)
    {
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБЬ");
        Width-=2;
        Top+=1;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^JБЬ");
        Width-=4;
        Top+=2;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^1^JБЬ");
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top-1, Width, Width, 0, "^7^K£≠");
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top+1, Width, Width, 0, "^7^K£≠");
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^K£≠");
    }
    /** кольцева€ дорога 4.3 **/
    /** ограничение скорости 3.24 **/
    /** сто€нка 6.4 **/
    if (ID == 9)
    {
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБ°");
        Width-=2;
        Top+=1;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^JБ°");
        Width-=4;
        Top+=2;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 6, "^JБ°");
        Width-=10;
        Top+=5;
        SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^K£–");
    }
    /** тупик 6.8 **/
    if (ID == 10)
    {
    }
    /** поворот запрещен 3.18 **/
    if (ID == 11)
    {
    }
    /** разворот запрещен 3.19 **/
    if (ID == 12)
    {
    }
}

void RCRoadSign::InsimMCI ( struct IS_MCI* packet )
{
    for (int i = 0; i < packet->NumC; i++)
    {
        byte UCID = PLIDtoUCID[ packet->Info[i].PLID ];

        int X = packet->Info[i].X / 65536;
        int Y = packet->Info[i].Y / 65536;
        int H = packet->Info[i].Heading / 182;
        int S = ((int)packet->Info[i].Speed * 360) / (32768);

        int SignCount = 0;
        for ( auto& sign: Sign)
            if ( Distance(X, Y, sign.X, sign.Y ) < 20 and abs( H - sign.Heading ) <= 45 )
            {
                ShowSign(UCID, sign.ID, SignCount);
                players[UCID].OnSign = true;
                SignCount++;
            }

        if (SignCount == 0 and players[UCID].OnSign)
        {
            for (int f = 90; f < 110; f++)
                SendBFN(UCID, f);

            players[UCID].SignCount = 0;
            players[UCID].OnSign = false;
        }

        if (SignCount != players[UCID].SignCount and players[UCID].OnSign)
        {
            for (int f = 90 + 10 * SignCount; f < 110; f++)
                SendBFN(UCID, f);
            players[UCID].SignCount = SignCount;
        }

        if (S > 1)
            memcpy(&players[UCID].Info, &packet->Info[i], sizeof(struct CompCar) );
    }
}
