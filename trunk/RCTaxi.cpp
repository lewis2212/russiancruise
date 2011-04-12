using namespace std;

#include "RCTaxi.h"

RCTaxi::RCTaxi()
{

}

RCTaxi::~RCTaxi()
{

}

int RCTaxi::init(char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * street)
{
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

    bank = (RCBank *)Bank;
    if(!msg)
    {
        printf ("Can't struct RCBank class");
        return -1;
    }

    dl = (RCDL *)RCdl;
    if(!msg)
    {
        printf ("Can't struct RCDL class");
        return -1;
    }

    street = (RCStreet *)street;
    if(!msg)
    {
        printf ("Can't struct RCStreet class");
        return -1;
    }

    inited = 1;

    return 0;
}


void RCTaxi::readconfig(char *Track)
{
    //cout << "RCTaxi::readconfig\n" ;
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\tracks\\%s.txt",RootDir,Track);
    //cout << file << endl;


    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("Can't find \n%s",file);
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);


    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);

        //cout << str << endl;


        if (strlen(str) > 0)
        {

            if (strncmp(str,"/dealer",7)==0)
            {
                for (int i=0; i<4; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,",");
                    Y = strtok (NULL,",");
                    zone.dealX[i] = atoi(X);
                    zone.dealY[i] = atoi(Y);
                    //cout << point << ". X=" << X << "; Y=" << Y << ";\n";
                }
            } // if /street

            if (strncmp(str,"/shop",5)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);
                TrackInf.ShopCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,";");
                    Y = strtok (NULL,";");
                    TrackInf.XShop[i] = atoi(X);
                    TrackInf.YShop[i] = atoi(Y);
                }
            }



        } // if strlen > 0
    } //while readf.good()

    readf.close();

    /** READ PTH FILE **/

    sprintf(file,"%sdata\\RCTaxi\\pth\\%s.pth",RootDir,Track);
    //cout << file << endl;



    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("Can't find file \n %s",file);
        return;
    }
    FindClose(fff);

    HANDLE  hFile;

    //! открываем файл для чтения
    hFile = CreateFile(file, GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    //! проверяем на успешное открытие
    DWORD  dwBytesRead;
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Create file failed. \nThe last error code: %d\n",(int)GetLastError());
        return;
    }

    if ( !ReadFile(hFile,&pth,sizeof(PTH),&dwBytesRead,(LPOVERLAPPED)NULL) )
    {
        cerr << "Read \"PTH Header\" failed." << endl;
        return;
    }


    for (int j = 0; j < pth.num_nodes; j++)
    {
        if ( !ReadFile(hFile,&nodes[j],sizeof(PTH_NODES),&dwBytesRead,(LPOVERLAPPED)NULL) )
        {
            cerr << "Read \"PTH Nodes[" << j << "]\" failed." << endl;
            return;
        }

        if (dwBytesRead == 0)
            break;
    }

    /** закрываем дескриптор файла **/
    CloseHandle(hFile);

    //readpth


}

void RCTaxi::next_packet()
{
    switch (insim->peek_packet())
    {
    case ISP_MSO:
        taxi_mso();
        break;


    case ISP_NPL:
        taxi_npl();
        break;

    case ISP_NCN:
        taxi_ncn();
        break;

    case ISP_CNL:
        taxi_cnl();
        break;

    case ISP_PLL:
        taxi_pll();
        break;

    case ISP_PLP:
        taxi_plp();
        break;

    case ISP_CPR:
        taxi_crp();
        break;


    }
}

void RCTaxi::accept_user()
{
    if ( accept_time >= time(&acctime))
        return;
    //srand(time(&ptime));
    //int r = rand()%3 + 1;
    int taxi_time = 600/(NumP+1);
    accept_time += taxi_time;

    for (int i=0; i< 32; i++)
    {
        if ((players[i].Work == 1) and (players[i].WorkAccept == 0))
        {
            // accept player

            int CurStreet = street->CurentStreetNum(players[i].UCID);
            int DestStreet = 0;
            srand(time(NULL));

            while (DestStreet == CurStreet)
                DestStreet = rand()%(street->StreetCount());

            struct streets StreetInfo;
            memset(&StreetInfo,0,sizeof(streets));
            if (!street->CurentStreetInfo(&StreetInfo,DestStreet))
                return;

            int DestNode = rand()%(StreetInfo.NodeEnd) + (StreetInfo.NodeBeg);

            players[i].WorkNodeDestinaion = DestNode;

        }
    }
}


void RCTaxi::taxi_cnl ()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            save_user(&players[i]);
            memset(&players[i],0,sizeof(struct TaxiPlayer));
            break;
        }
    }
}

void RCTaxi::taxi_crp()
{
    int i;

    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cpr->UCID)
        {
            /** DO SOME CODE **/

            strcpy(players[i].PName, pack_cpr->PName);
            break;
        }
    }
}

void RCTaxi::taxi_mci ()
{
    //cout << "taxi_mci" << endl;

    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]

    {
        for (int j =0; j < MAX_PLAYERS; j++) // прогон по всему массиву players[j]
        {

            if (pack_mci->Info[i].PLID == players[j].PLID) // старая заплатка ( and players[j].PLID != 0 and players[j].UCID != 0)
            {
                /** DO SOME CODE **/

                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;

                // Если игрок находится в зоне приема на работу, выводим воспомогательные сообщения в чат

                int TX[4] = {-465,-465,-480,-480};
                int TY[4] = {450,420,420,450};
                if (Check_Pos(4,TX,TY,X,Y))
                {
                    cout << "in work place" << endl;
                    if (players[j].InZone == 0)
                    {
                        players[j].InZone = 1;
                        send_mtc(players[j].UCID,"Taxi Radriges");
                        send_mtc(players[j].UCID,"!deal");
                        send_mtc(players[j].UCID,"!undeal");
                        send_mtc(players[j].UCID,"!workstart");
                        send_mtc(players[j].UCID,"!workend");
                    }
                }
                else
                {
                    if (players[j].InZone == 1)
                    players[j].InZone = 0;
                }




                /** NOTE: don't use break **/
            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }

    /** thread xD **/
    accept_user();


    /** thread **/
}

void RCTaxi::taxi_mso ()
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

    char Message[96];
    strcpy(Message,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));



    if (strncmp(Message, "!deal", strlen("!deal")) == 0 )
    {
        /** DO SOME CODE **/
        if (players[i].Work !=0)
        {
            send_mtc(players[i].UCID,"Ti uze rabotaew");
            return;
        }

        time_t nt;
        int nowtime = time(&nt);
        if (players[i].FiredPenalty > nowtime)
        {
            send_mtc(players[i].UCID,"uvolen do 'tut vremya'");
            return;
        }

        send_mtc(players[i].UCID,"Ti prinyat");
        players[i].Work = 1;

    }

    if (strncmp(Message, "!undeal", strlen("!undeal")) == 0 )
    {
        /** DO SOME CODE **/
        if (players[i].Work ==0)
        {
            send_mtc(players[i].UCID,"Ti uze ne rabotaew");
            return;
        }

        send_mtc(players[i].UCID,"dogovor rastorgnut");
        players[i].Work = 0;
    }

    if (strncmp(Message, "!workstart", strlen("!workstart")) == 0 )
    {
        /** DO SOME CODE **/
    }

    if (strncmp(Message, "!workend", strlen("!workend")) == 0 )
    {
        /** DO SOME CODE **/
    }

}


void RCTaxi::taxi_ncn()
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

    read_user(&players[i]);

    /** DO SOME CODE **/


}

void RCTaxi::taxi_npl()
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
            /** DO SOME CODE **/
            break;
        }
    }
}

void RCTaxi::taxi_plp()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_plp->PLID)
        {
            /** DO SOME CODE **/
            players[i].PLID = 0;
            /** DO SOME CODE **/
            break;
        }
    }
}

void RCTaxi::taxi_pll()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_pll->PLID)
        {
            /** DO SOME CODE **/
            players[i].PLID = 0;
            /** DO SOME CODE **/
            break;
        }
    }
}

void RCTaxi::read_user(struct TaxiPlayer *splayer)
{
    /** read file **/

    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\users\\%s.txt",RootDir,splayer->UName);
    //strcat(file,"tracks\\.txt");

    // Read Data From File

    // Try Find New File
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf("Can't find %s\n Create File for user",file);
        // create
        splayer->Work= 0;
        splayer->FiredPenalty= 0;
        splayer->PenaltyCount = 0;
        splayer->PassCount = 0;
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

                if (strncmp("Work=",str,strlen("Work="))==0)
                {
                    splayer->Work = atoi(str+strlen("Work="));
                }

                if (strncmp("FiredPenalty=",str,strlen("FiredPenalty="))==0)
                {
                    splayer->FiredPenalty= atoi(str+strlen("FiredPenalty="));
                }

                if (strncmp("PenaltyCount=",str,strlen("PenaltyCount="))==0)
                {
                    splayer->PenaltyCount = atoi(str+strlen("PenaltyCount="));
                }

                if (strncmp("PassCount=",str,strlen("PassCount="))==0)
                {
                    splayer->PassCount = atoi(str+strlen("PassCount="));
                }


            }
        }



        readf.close();
    }
    FindClose(fff);
}

void RCTaxi::save_user(struct TaxiPlayer *splayer)
{
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\users\\%s.txt",RootDir,splayer->UName);
    //printf(file);

    ofstream writef (file,ios::out);
    writef << "Work=" << splayer->Work << endl;
    writef << "FiredPenalty=" << splayer->FiredPenalty << endl;
    writef << "PenaltyCount=" << splayer->PenaltyCount << endl;
    writef << "PassCount=" << splayer->PassCount << endl;
    writef.close();
}


void RCTaxi::send_mtc (byte UCID,char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strcpy(pack_mtc.Msg, Msg);
    insim->send_packet(&pack_mtc);
}

void RCTaxi::send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst);
}

void RCTaxi::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack);
}
