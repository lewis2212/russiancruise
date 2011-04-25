using namespace std;

#include "RCLight.h"

char* signal1 ="^0Х";
char* signal2 ="^0Х";
char* signal3 ="^0Х";
char* signal11 ="^0Х";
char* signal12 ="^0Х";
char* signal13 ="^0Х";

time_t sstime;

pthread_t svet1_tid; // Thread ID
pthread_t svet2_tid; // Thread ID

void *thread_svet1(void* params)
{
    cout << "\tthread \"Svetofor 1\" started" << endl;
    for (;;)
    {
        int svtime = time(&sstime)%40;
        //out << "svtime = " << svtime << endl;

        if (svtime < 10)
        {
            signal1="^C^0Х";
            signal2="^C^0Х";
            signal3="^C^2Х";
        }
        else if (svtime == 10)
        {
            signal1="^C^0Х";
            signal2="^C^0Х";
            signal3="^C^0Х";
        }
        else if (svtime == 11)
        {
            signal1="^C^0Х";
            signal2="^C^0Х";
            signal3="^C^2Х";
        }
        else if (svtime == 12)
        {
            signal1="^C^0Х";
            signal2="^C^0Х";
            signal3="^C^0Х";
        }
        else if (svtime == 13)
        {
            signal1="^C^0Х";
            signal2="^C^0Х";
            signal3="^C^2Х";
        }
        else if (svtime == 14)
        {
            signal1="^C^0Х";
            signal2="^C^0Х";
            signal3="^C^0Х";
        }
        else if (svtime == 15)
        {
            signal1="^C^0Х";
            signal2="^C^0Х";
            signal3="^C^2Х";
        }
        else if (svtime == 16)
        {
            signal1="^C^0Х";
            signal2="^C^0Х";
            signal3="^C^0Х";
        }
        else if ((svtime > 16) and (svtime < 20))
        {
            signal1="^C^0Х";
            signal2="^C^3Х";
            signal3="^C^0Х";
        }
        else if ((svtime >= 20) and (svtime < 37))
        {
            signal1="^C^1Х";
            signal2="^C^0Х";
            signal3="^C^0Х";
        }
        else if ((svtime >= 37))
        {
            signal1="^C^1Х";
            signal2="^C^3Х";
            signal3="^C^0Х";
        }


        Sleep(1000);
    }
    return 0;
}

void *thread_svet2( void* params)
{
    cout << "\tthread \"Svetofor 2\" started" << endl;
    for (;;)
    {
        int svtime = (time(&sstime)+20)%40;
        //out << "svtime = " << svtime << endl;

        if (svtime < 10)
        {
            signal11="^C^0Х";
            signal12="^C^0Х";
            signal13="^C^2Х";
        }
        else if (svtime == 10)
        {
            signal11="^C^0Х";
            signal12="^C^0Х";
            signal13="^C^0Х";
        }
        else if (svtime == 11)
        {
            signal11="^C^0Х";
            signal12="^C^0Х";
            signal13="^C^2Х";
        }
        else if (svtime == 12)
        {
            signal11="^C^0Х";
            signal12="^C^0Х";
            signal13="^C^0Х";
        }
        else if (svtime == 13)
        {
            signal11="^C^0Х";
            signal12="^C^0Х";
            signal13="^C^2Х";
        }
        else if (svtime == 14)
        {
            signal11="^C^0Х";
            signal12="^C^0Х";
            signal13="^C^0Х";
        }
        else if (svtime == 15)
        {
            signal11="^C^0Х";
            signal12="^C^0Х";
            signal13="^C^2Х";
        }
        else if (svtime == 16)
        {
            signal11="^C^0Х";
            signal12="^C^0Х";
            signal13="^C^0Х";
        }
        else if ((svtime > 16) and (svtime < 20))
        {
            signal11="^C^0Х";
            signal12="^C^3Х";
            signal13="^C^0Х";
        }
        else if ((svtime >= 20) and (svtime < 37))
        {
            signal11="^C^1Х";
            signal12="^C^0Х";
            signal13="^C^0Х";
        }
        else if ((svtime >= 37))
        {
            signal11="^C^1Х";
            signal12="^C^3Х";
            signal13="^C^0Х";
        }


        Sleep(1000);
    }
    return 0;
}


RCLight::RCLight()
{


    if (pthread_create(&svet1_tid,NULL,thread_svet1,NULL) < 0)
    {
        printf("Can't start `thread_svet1` Thread\n");
    }
    Sleep(1000);
    if (pthread_create(&svet2_tid,NULL,thread_svet2,NULL) < 0)
    {
        printf("Can't start `thread_svet2` Thread\n");
    }
    Sleep(1000);
}

RCLight::~RCLight()
{
    pthread_detach(svet1_tid);
    pthread_detach(svet2_tid);
}

int RCLight::init(char *dir, void *CInSim, void *Message)
{
    //cout << "Init Streets" << endl;
    IfInited = false;
    strcpy(RootDir,dir); //  опируем путь до программы


    insim = (CInsim *)CInSim; // ѕрисваиваем указателю область пам€ти
    if(!insim) // ѕровер€ем на существование
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

void RCLight::readconfig(char *Track)
{
    //cout << "Init Streets" << endl;

    char file[255];
    sprintf(file,"%sdata\\RCLight\\tracks\\%s.txt",RootDir,Track);


    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "RCLight: Can't find " << file << endl;
        return ;
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
            if (strcmp(str,"#Light")==0)
            {
                //онул€ем позицию
                //ZeroMemory(&Light[i],sizeof(Lights));

                readf.getline(str,128);
                Light[i].ID = atoi(str);

                readf.getline(str,128);
                Light[i].Heading = atoi(str);

                // читаем количество точек
                readf.getline(str,128);
                Light[i].PointCount = atoi(str);

                //printf("i=%d, ID = %d , Head = %d, PointCount = %d\n",i,Light[i].ID,Light[i].Heading,Light[i].PointCount);

                // объ€вл€ем массих точек ’ and Y
                Light[i].X = new int[Light[i].PointCount];
                Light[i].Y = new int[Light[i].PointCount];

                for (int k=0; k<Light[i].PointCount; k++)
                {
                    readf.getline(str,128);

                    Light[i].X[k] = 0;
                    Light[i].Y[k] = 0;

                    Light[i].X[k] = atoi(strtok (str,";"));
                    Light[i].Y[k] = atoi(strtok (NULL,";"));

                    //printf("Light[%d].X[%d] = %d; Light[%d].Y[%d] = %d\n",i,k,Light[i].X[k],i,k,Light[i].Y[k]);
                }

                i++;
            }

        } // if strlen > 0

    } //while readf.good()
    LightsCount = i;

    readf.close();

    return;
}

void RCLight::next_packet()
{
    switch (insim->peek_packet())
    {
    case ISP_MSO:
        mso();
        break;


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


    }
}


void RCLight::cnl ()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            memset(&players[i],0,sizeof(struct LghPlayer));
            break;
        }
    }
}

void RCLight::crp()
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

void RCLight::mci ()
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
                int H = pack_mci->Info[i].Heading/182;

                int SvetKey = 0;

                for (int g=0; g<LightsCount; g++)
                {
                    if(Check_Pos(Light[g].PointCount,Light[g].X,Light[g].Y,X,Y))
                    {
                        int H1 = Light[g].Heading-80;
                        int H2 = Light[g].Heading+80;



                        if (H2 > 360)
                            H2 -= 360;

                        //a printf("Heading = %d\n",H);

                        if (H1 < 0)
                        {
                            H1 = 360 - H1;
                            if ((H > H1) or (H < H2))
                            {
                                players[j].Light = Light[g].ID;
                                SvetKey = Light[g].ID;
                            }
                        }


                        if ((H > H1) and (H < H2))
                        {
                            players[j].Light = Light[g].ID;
                            SvetKey = Light[g].ID;
                        }

                    }
                }

                if (SvetKey == 1)
                {
                    btn_svetofor1(&players[j]);
                }
                else if (SvetKey == 2)
                {
                    btn_svetofor2(&players[j]);
                }
                else
                {
                    if (players[j].Light != 0)
                    {
                        for (int f=190; f < 203; f++)
                            send_bfn(players[j].UCID,f);

                        players[j].Light = 0;
                        //out << "clear svetofor" << endl;
                    }
                }


                /**  steets **/

            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }
}

void RCLight::mso ()
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


void RCLight::ncn()
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

void RCLight::npl()
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

void RCLight::plp()
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

void RCLight::pll()
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

void RCLight::send_mtc (byte UCID,char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strcpy(pack_mtc.Msg, Msg);
    insim->send_packet(&pack_mtc);
}

void RCLight::send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst);
}

void RCLight::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack);
}

void RCLight::btn_svetofor1 (struct LghPlayer *splayer)
{

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.BStyle = 32;
    pack.TypeIn = 0;

    pack.ClickID = 190;
    pack.L = 92;
    pack.T = 15;
    pack.W = 16;
    pack.H = 40;
    insim->send_packet(&pack);

    pack.BStyle = 16;
    pack.ClickID = 191;
    pack.L = 93;
    pack.T = 16;
    pack.W = 14;
    pack.H = 38;
    insim->send_packet(&pack);



    pack.BStyle = 1;
    pack.ClickID = 192;
    pack.L = 85;
    pack.T = 10;
    pack.W = 30;
    pack.H = 30;
    strcpy(pack.Text,signal1);
    insim->send_packet(&pack);


    pack.ClickID = 193;
    pack.T = 10+10;
    strcpy(pack.Text,signal2);
    insim->send_packet(&pack);

    pack.ClickID = 194;
    pack.T = 10+20;
    strcpy(pack.Text,signal3);
    insim->send_packet(&pack);
}

void RCLight::btn_svetofor2 (struct LghPlayer *splayer)
{

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.BStyle = 32;
    pack.TypeIn = 0;

    pack.ClickID = 190;
    pack.L = 92;
    pack.T = 15;
    pack.W = 16;
    pack.H = 40;
    insim->send_packet(&pack);

    pack.BStyle = 16;
    pack.ClickID = 191;
    pack.L = 93;
    pack.T = 16;
    pack.W = 14;
    pack.H = 38;
    insim->send_packet(&pack);



    pack.BStyle = 1;
    pack.ClickID = 192;
    pack.L = 85;
    pack.T = 10;
    pack.W = 30;
    pack.H = 30;
    strcpy(pack.Text,signal11);
    insim->send_packet(&pack);


    pack.ClickID = 193;
    pack.T = 10+10;
    strcpy(pack.Text,signal12);
    insim->send_packet(&pack);

    pack.ClickID = 194;
    pack.T = 10+20;
    strcpy(pack.Text,signal13);
    insim->send_packet(&pack);
}

void RCLight::btn_svetofor3 (struct LghPlayer *splayer)
{

    struct IS_BTN pack_btn;
    memset(&pack_btn, 0, sizeof(struct IS_BTN));
    pack_btn.Size = sizeof(struct IS_BTN);
    pack_btn.Type = ISP_BTN;
    pack_btn.ReqI = 1;
    pack_btn.UCID = splayer->UCID;
    pack_btn.Inst = 0;
    pack_btn.BStyle = 32;
    pack_btn.TypeIn = 0;
    /**** telo1 **/
    pack_btn.ClickID = 195;
    pack_btn.L = 148;
    pack_btn.T = 110;
    pack_btn.W = 8;
    pack_btn.H = 20;
    strcpy(pack_btn.Text,"");
    insim->send_packet(&pack_btn);
    /**********/


    pack_btn.BStyle = 1;
    pack_btn.ClickID = 196;
    pack_btn.L = 144;
    pack_btn.T = 107;
    pack_btn.W = 16;
    pack_btn.H = 16;
    strcpy(pack_btn.Text,signal1);
    insim->send_packet(&pack_btn);


    pack_btn.ClickID = 197;
    pack_btn.T = 112;
    strcpy(pack_btn.Text,signal2);
    insim->send_packet(&pack_btn);

    pack_btn.ClickID = 198;
    pack_btn.T = 117;
    strcpy(pack_btn.Text,signal3);
    insim->send_packet(&pack_btn);
    /********************************/
    pack_btn.BStyle = 32;
    /**** telo 2 *******/
    pack_btn.ClickID = 199;
    pack_btn.L = 157;
    pack_btn.T = 110;
    pack_btn.W = 8;
    pack_btn.H = 20;
    strcpy(pack_btn.Text,"");
    insim->send_packet(&pack_btn);
    /*************************/
    pack_btn.BStyle = 1;

    pack_btn.ClickID = 200;
    pack_btn.L = 153;
    pack_btn.T = 107;
    pack_btn.W = 16;
    pack_btn.H = 16;
    strcpy(pack_btn.Text,signal11);
    insim->send_packet(&pack_btn);


    pack_btn.ClickID = 201;
    pack_btn.T = 112;
    strcpy(pack_btn.Text,signal12);
    insim->send_packet(&pack_btn);

    pack_btn.ClickID = 202;
    pack_btn.T = 117;
    strcpy(pack_btn.Text,signal13);
    insim->send_packet(&pack_btn);
}
