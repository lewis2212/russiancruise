#include "RCRoadSign.h"

RCRoadSign::RCRoadSign(const char* Dir)
{
    ClassName = "RCRoadSign";
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

    CCText("^3R"+ClassName+":\t^2inited");
    return 0;
}

bool RCRoadSign::ReadConfig(const char *Track)
{
    strcpy(TrackName, Track);
    Sign.clear();
    char file[MAX_PATH];
    sprintf(file, "%s/data/RCRoadSign/tracks/%s.txt", RootDir, Track);

    ifstream readf (file, ios::in);

    if (!readf.is_open())
    {
        CCText("  ^7RCRoadSign ^1ERROR: ^8file " + (string)file + " not found");
        return true;
    }

    int i=0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);
        if (strlen(str) > 1)
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
                i++;
            }
        }
    }
    readf.close();

    sprintf(file, "%s/data/RCRoadSign/sign.txt", RootDir);

    ifstream read (file, ios::in);

    if (read.is_open() == false)
    {
        CCText("  ^7RCRoadSign ^1ERROR: ^8file " + (string)file + " not found");
        return true;
    }

    while (read.good())
    {
        char str[128];
        read.getline(str, 128);

        if (strlen(str) > 1)
        {
            int pr = atoi(strtok(str, ";"));
            if (pr!=0)
            {
                SignName[i].ID = pr;
                sprintf(SignName[i++].Name, "^C%s", strtok(NULL, ";"));
            }
        }
    }
    read.close();

    CCText("  ^7RCRoadSign\t^2OK");
    return true;
}

bool RCRoadSign::InsimNCN( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
        return true;

    strcpy(players[packet->UCID].UName, packet->UName);
    strcpy(players[packet->UCID].PName, packet->PName);
    return true;
}

bool RCRoadSign::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[packet->PLID] = packet->UCID;
    return true;
}

bool RCRoadSign::InsimPLP( struct IS_PLP* packet )
{
    PLIDtoUCID.erase( packet->PLID );
    return true;
}

bool RCRoadSign::InsimPLL( struct IS_PLL* packet )
{
    PLIDtoUCID.erase( packet->PLID );
    return true;
}

bool RCRoadSign::InsimCNL( struct IS_CNL* packet )
{
    players.erase( packet->UCID );
    return true;
}

bool RCRoadSign::InsimCPR( struct IS_CPR* packet )
{
    strcpy( players[ packet->UCID ].PName, packet->PName );
    return true;
}

bool RCRoadSign::InsimMSO(struct IS_MSO* packet)
{
    byte UCID = packet->UCID;

    if ( UCID == 0 )
        return true;

    if ( packet->UserType != MSO_PREFIX )
        return true;

    char Msg[128];
    strcpy( Msg, packet->Msg + ((unsigned char)packet->TextStart));

    if ( strncmp(Msg, "!s_add", 6) == 0 and (strcmp(players[UCID].UName, "Lexanom") == 0 or strcmp(players[UCID].UName, "denis-takumi") == 0 or strcmp(players[UCID].UName, "Max1548") == 0))
    {
        char file[MAX_PATH], text[96];

        sprintf(file, "%s/data/RCRoadSign/tracks/%s.txt", RootDir, TrackName);

        int X = players[UCID].Info.X / 65536;
        int Y = players[UCID].Info.Y / 65536;
        int Dir = players[UCID].Info.Heading / 182;

        int ID=0;

        strtok(Msg, " ");
        int s = atoi(strtok(NULL, " "));

        if (s == 0)
        {
            sprintf(text, "^1Error: ^Cукажи ID");
            insim->SendMTC(UCID, text);
            return true;
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
        insim->SendMTC(UCID, text);
    }
}

void RCRoadSign::ShowSign(byte UCID, byte ID, byte Count)
{
    byte ClickID = 151, Left = 145, Top = 0+27 * Count, Width = 46, Height = 0;
    if (lgh->GetOnLight(UCID))
    {
        Top+=44;
        Height=52;
    }
    //подвес
    insim->SendButton(255, UCID, ClickID++, Left, 0+Height, 1, 15+27 * Count, 32, "");
    insim->SendButton(255, UCID, ClickID++, Left, 0+Height, 1, 15+27 * Count, 32, "");
    insim->SendButton(255, UCID, ClickID++, Left, 0+Height, 1, 15+27 * Count, 32, "");
    ClickID = 154+10 * Count;

    /** главна€ дорога 2.1 **/
    if (ID == 1)
    {
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБЯ");
        Width-=2;
        Top+=1;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^JБЯ");
        Width-=10;
        Top+=5;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБЯ");
        Width-=2;
        Top+=1;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^3^JБЯ");
        insim->SendBFN(UCID, ClickID++);
        insim->SendBFN(UCID, ClickID++);
    }
    /** уступи дорогу 2.4 **/
    if (ID == 2)
    {
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБ•");
        Width-=2;
        Top+=1;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^1^JБ•");
        Width-=12;
        Top+=6;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top-1, Width, Width, 0, "^7^JБ•");
        insim->SendBFN(UCID, ClickID++);
        insim->SendBFN(UCID, ClickID++);
        insim->SendBFN(UCID, ClickID++);
    }
    /** иск. неровность 1.17 **/
    if (ID == 3)
    {
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБ£");
        Width-=2;
        Top+=1;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^1^JБ£");
        Width-=12;
        Top+=6;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top+1, Width, Width, 0, "^7^JБ£");
        Width-=14;
        Top+=16;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБ№");
        Width+=6;
        Top-=13;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2+1, Top, Width-1, Width, 0, "^0^JБQ");
        insim->SendBFN(UCID, ClickID++);
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
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБЬ");
        Width-=2;
        Top+=1;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^JБЬ");
        Width-=4;
        Top+=2;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^1^JБЬ");
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top-1, Width, Width, 0, "^7^K£≠");
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top+1, Width, Width, 0, "^7^K£≠");
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^K£≠");
    }
    /** кольцева€ дорога 4.3 **/
    /** ограничение скорости 3.24 **/
    /** сто€нка 6.4 **/
    if (ID == 9)
    {
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^0^JБ°");
        Width-=2;
        Top+=1;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^JБ°");
        Width-=4;
        Top+=2;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 6, "^JБ°");
        Width-=10;
        Top+=5;
        insim->SendButton(255, UCID, ClickID++, Left-Width / 2, Top, Width, Width, 0, "^7^K£–");
        insim->SendBFN(UCID, ClickID++);
        insim->SendBFN(UCID, ClickID++);
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

bool RCRoadSign::InsimMCI ( struct IS_MCI* packet )
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
            if (Distance(X, Y, sign.X, sign.Y ) < 25 and abs( H - sign.Heading ) <= 45 )
            {
                if (sign.ID != players[UCID].OnSign)
                    ShowSign(UCID, sign.ID, SignCount);
                players[UCID].OnSign = sign.ID;
                SignCount++;
            }

        if (SignCount == 0 and players[UCID].OnSign)
        {
            for (int f = 151; f < 170; f++)
                insim->SendBFN(UCID, f);

            players[UCID].SignCount = 0;
            players[UCID].OnSign = 0;
        }

        if (SignCount != players[UCID].SignCount and players[UCID].OnSign)
        {
            for (int f = 151 + 10 * SignCount; f < 170; f++)
                insim->SendBFN(UCID, f);
            players[UCID].SignCount = SignCount;
        }

        if (S > 1)
            players[UCID].Info = packet->Info[i];
    }
    return true;
}
