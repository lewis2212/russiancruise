using namespace std;

#include "RCLight.h"

const char* signal1 ="^0�";
const char* signal2 ="^0�";
const char* signal3 ="^0�";
const char* signal11 ="^0�";
const char* signal12 ="^0�";
const char* signal13 ="^0�";

time_t sstime;

pthread_t svet1_tid; // Thread ID
pthread_t svet2_tid; // Thread ID

void *thread_svet1(void* params)
{
    for (;;)
    {
        int svtime = time(&sstime)%40;

        if (svtime < 10)
        {
            signal1="^0�";
            signal2="^0�";
            signal3="^2�";
        }
        else if (svtime == 10)
        {
            signal1="^0�";
            signal2="^0�";
            signal3="^0�";
        }
        else if (svtime == 11)
        {
            signal1="^0�";
            signal2="^0�";
            signal3="^2�";
        }
        else if (svtime == 12)
        {
            signal1="^0�";
            signal2="^0�";
            signal3="^0�";
        }
        else if (svtime == 13)
        {
            signal1="^0�";
            signal2="^0�";
            signal3="^2�";
        }
        else if (svtime == 14)
        {
            signal1="^0�";
            signal2="^0�";
            signal3="^0�";
        }
        else if (svtime == 15)
        {
            signal1="^0�";
            signal2="^0�";
            signal3="^2�";
        }
        else if (svtime == 16)
        {
            signal1="^0�";
            signal2="^0�";
            signal3="^0�";
        }
        else if ((svtime > 16) and (svtime < 20))
        {
            signal1="^0�";
            signal2="^3�";
            signal3="^0�";
        }
        else if ((svtime >= 20) and (svtime < 37))
        {
            signal1="^1�";
            signal2="^0�";
            signal3="^0�";
        }
        else if ((svtime >= 37))
        {
            signal1="^1�";
            signal2="^3�";
            signal3="^0�";
        }


        Sleep(1000);
    }
    return 0;
}

void *thread_svet2( void* params)
{
    for (;;)
    {
        int svtime = (time(&sstime)+20)%40;

        if (svtime < 10)
        {
            signal11="^0�";
            signal12="^0�";
            signal13="^2�";
        }
        else if (svtime == 10)
        {
            signal11="^0�";
            signal12="^0�";
            signal13="^0�";
        }
        else if (svtime == 11)
        {
            signal11="^0�";
            signal12="^0�";
            signal13="^2�";
        }
        else if (svtime == 12)
        {
            signal11="^0�";
            signal12="^0�";
            signal13="^0�";
        }
        else if (svtime == 13)
        {
            signal11="^0�";
            signal12="^0�";
            signal13="^2�";
        }
        else if (svtime == 14)
        {
            signal11="^0�";
            signal12="^0�";
            signal13="^0�";
        }
        else if (svtime == 15)
        {
            signal11="^0�";
            signal12="^0�";
            signal13="^2�";
        }
        else if (svtime == 16)
        {
            signal11="^0�";
            signal12="^0�";
            signal13="^0�";
        }
        else if ((svtime > 16) and (svtime < 20))
        {
            signal11="^0�";
            signal12="^3�";
            signal13="^0�";
        }
        else if ((svtime >= 20) and (svtime < 37))
        {
            signal11="^1�";
            signal12="^0�";
            signal13="^0�";
        }
        else if ((svtime >= 37))
        {
            signal11="^1�";
            signal12="^3�";
            signal13="^0�";
        }


        Sleep(1000);
    }
    return 0;
}


RCLight::RCLight()
{
    if (pthread_create(&svet1_tid,NULL,thread_svet1,NULL) < 0)
        printf("Can't start `thread_svet1` Thread\n");

    Sleep(1000);
    if (pthread_create(&svet2_tid,NULL,thread_svet2,NULL) < 0)
        printf("Can't start `thread_svet2` Thread\n");

    Sleep(1000);
}

RCLight::~RCLight()
{
    pthread_detach(svet1_tid);
    pthread_detach(svet2_tid);
	//delete[] players;
}

bool RCLight::SetLight3(byte UCID,bool Key)
{
	if (players[ UCID ].Light3 and !Key)
	{
		for (int f=190; f < 203; f++)
			send_bfn( UCID ,f);
	}

	players[ UCID ].Light3 = Key;
}

int RCLight::init(const char *dir, void *CInSim, void *Message, void *RCDLic)
{
    IfInited = false;
    strcpy(RootDir,dir); // �������� ���� �� ���������

    insim = (CInsim *)CInSim; // ����������� ��������� ������� ������
    if(!insim) // ��������� �� �������������
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

    dl = (RCDL *)RCDLic;
    if(!dl)
    {
        printf ("Can't struct RCDL class");
        return -1;
    }

    IfInited = true;


    return 0;
}

void RCLight::readconfig(const char *Track)
{

    strcpy(TrackName, Track);
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


        if (strlen(str) > 0)
        {
            if (strcmp(str,"#Light")==0)
            {

                readf.getline(str,128);
                Light[i].ID = atoi(str);

                readf.getline(str,128);
                Light[i].Heading = atoi(str);

                // ������ ���������� �����
                readf.getline(str,128);
                Light[i].PointCount = atoi(str);

                // ��������� ������ ����� � and Y
                Light[i].X = new int[Light[i].PointCount];
                Light[i].Y = new int[Light[i].PointCount];

                for (int k=0; k<Light[i].PointCount; k++)
                {
                    readf.getline(str,128);

                    Light[i].X[k] = 0;
                    Light[i].Y[k] = 0;

                    Light[i].X[k] = atoi(strtok (str,";"));
                    Light[i].Y[k] = atoi(strtok (NULL,";"));
                }
                i++;
            }
        } // if strlen > 0
    } //while readf.good()
    LightsCount = i;

    readf.close();

    return;
}


void RCLight::insim_cnl( struct IS_CNL* packet )
{
	players.erase( packet->UCID );
}

void RCLight::insim_cpr( struct IS_CPR* packet )
{
	strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCLight::insim_mci ( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
		byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];
		/** streets  **/

		int X = pack_mci->Info[i].X/65536;
		int Y = pack_mci->Info[i].Y/65536;
		int H = pack_mci->Info[i].Heading/182;
		int D = pack_mci->Info[i].Direction/182;
		int S = pack_mci->Info[i].Speed*360/32768;

		int SvetKey = 0;

		for (int g=0; g<LightsCount; g++)
		{
			if(Check_Pos(Light[g].PointCount,Light[g].X,Light[g].Y,X,Y))
			{
				int HR = Light[g].Heading-80;
				int HL = Light[g].Heading+80;

				char Text[64];
				sprintf(Text,"in zone");
				//send_mtc(255,Text);

				if (HR < 0)
				{
					HR = 360 + HR;
					if ((H > HR) or (H < HL))
					{
						players[ UCID ].Light = Light[g].ID;
						SvetKey = Light[g].ID;
					}
				}
				else if (HL > 359)
				{
					HL -= 359;
					if ((H > HR) or (H < HL))
					{
						players[ UCID ].Light = Light[g].ID;
						SvetKey = Light[g].ID;
					}
				}
				else
				{
					if ((H > HR) and (H < HL))
					{
						players[ UCID ].Light = Light[g].ID;
						SvetKey = Light[g].ID;
					}
				}


			}
		}

		if (SvetKey == 1)
		{
			btn_svetofor1( UCID );
		}
		else if (SvetKey == 2)
		{
			btn_svetofor2( UCID );
		}
		else
		{
			if (players[ UCID ].Light != 0)
			{
				for (int f=190; f < 203; f++)
					send_bfn( UCID ,f);

				players[ UCID ].Light = 0;
				//out << "clear svetofor" << endl;
			}
		}

		if (players[ UCID ].Light3)
			btn_svetofor3( UCID );


		/**  steets **/

		/** pit wrong route **/
		if (strstr(TrackName,"SO4X"))
		{
			int pit1x[10] = {210,200,190,200};
			int pit1y[10] = {233,230,277,281};

			int pit2x[10] = {229,236,247,239};
			int pit2y[10] = {-97,-39,-40,-100};

			//printf ("test wrong route\n");
			if ( Check_Pos( 4, pit1x, pit1y, X, Y ) )
			{
				//send_mst("in zone");
				if( (D < 190+90) and (D > 190-90) )
				{
					if (players[ UCID ].WrongWay == 1)
					{
						players[ UCID ].WrongWay =0;
						send_bfn( UCID ,203);
						send_bfn( UCID ,204);
					}
				}
				else
				{
					if (players[ UCID ].WrongWay == 0)
						players[ UCID ].WrongWay =1;
					btn_wrong_way( UCID );
					if (S > 10)
						dl->RemSkill( UCID );


				}
			}
			else if ( Check_Pos( 4, pit2x, pit2y, X, Y ) )
			{
				if( (D < 190+90) and (D > 190-90) )
				{
					if (players[ UCID ].WrongWay == 1)
					{
						players[ UCID ].WrongWay =0;
						send_bfn( UCID ,203);
						send_bfn( UCID ,204);
					}
				}
				else
				{
					if (players[ UCID ].WrongWay == 0)
						players[ UCID ].WrongWay =1;
					btn_wrong_way( UCID );
					if (S > 10)
						dl->RemSkill( UCID );
				}
			}
			else
			{
				if (players[ UCID ].WrongWay == 1)
				{
					players[ UCID ].WrongWay =0;
					send_bfn( UCID ,203);
					send_bfn( UCID ,204);
				}
			}

		}// if SO4X

    }
}

void RCLight::insim_mso( struct IS_MSO* packet )
{

}

void RCLight::insim_ncn( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
        return;

    strcpy( players[ packet->UCID ].UName, packet->UName);
    strcpy( players[ packet->UCID ].PName, packet->PName);

}

void RCLight::insim_npl( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID ;
	//players[ packet->UCID ].PLID = packet->PLID;
}

void RCLight::insim_plp( struct IS_PLP* packet)
{
	//players[ PLIDtoUCID[ packet->PLID ] ].PLID = 0;
	PLIDtoUCID.erase( packet->PLID );
}

void RCLight::insim_pll( struct IS_PLL* packet )
{
	//players[ PLIDtoUCID[ packet->PLID ] ].PLID = 0;
	PLIDtoUCID.erase( packet->PLID );
}

void RCLight::btn_svetofor1 ( byte UCID )
{

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
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

void RCLight::btn_svetofor2 ( byte UCID )
{

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
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

void RCLight::btn_svetofor3 ( byte UCID )
{

    struct IS_BTN pack_btn;
    memset(&pack_btn, 0, sizeof(struct IS_BTN));
    pack_btn.Size = sizeof(struct IS_BTN);
    pack_btn.Type = ISP_BTN;
    pack_btn.ReqI = 1;
    pack_btn.UCID = UCID;
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

void RCLight::btn_wrong_way(byte UCID)
{
    struct IS_BTN pack_btn;
    memset(&pack_btn, 0, sizeof(struct IS_BTN));
    pack_btn.Size = sizeof(struct IS_BTN);
    pack_btn.Type = ISP_BTN;
    pack_btn.ReqI = 1;
    pack_btn.UCID = UCID;
    pack_btn.Inst = 0;
    pack_btn.BStyle = 0;
    pack_btn.TypeIn = 0;
    /**** telo1 **/
    pack_btn.ClickID = 203;
    pack_btn.L = 0;
    pack_btn.T = 0;
    pack_btn.W = 200;
    pack_btn.H = 200;
    strcpy(pack_btn.Text,"^1�");
    insim->send_packet(&pack_btn);
    /**** telo2 **/
    pack_btn.ClickID = 204;
    pack_btn.L = 25;
    pack_btn.T = 26;
    pack_btn.W = 150;
    pack_btn.H = 150;
    strcpy(pack_btn.Text,"^7-");
    insim->send_packet(&pack_btn);
    /**********/
}

