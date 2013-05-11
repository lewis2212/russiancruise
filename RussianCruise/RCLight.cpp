using namespace std;
#include "RCLight.h"

RCLight::RCLight()
{

}

RCLight::~RCLight()
{

}

bool RCLight::SetLight3( byte UCID, bool Key)
{
	if (players[ UCID ].Light3 and !Key)
		for (int f=190; f < 203; f++)
			SendBFN( UCID ,f);

	players[ UCID ].Light3 = Key;
}

int RCLight::init(const char *dir, void *CInSim, void *Message, void *RCDLic)
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

    dl = (RCDL *)RCDLic;
    if(!dl)
    {
        printf ("Can't struct RCDL class");
        return -1;
    }

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

                // читаем количество точек
                readf.getline(str,128);
                Light[i].PointCount = atoi(str);

                // объявляем массив точек Х and Y
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
        }
    }
    LightsCount = i;
    readf.close();
    return;
}

void RCLight::InsimCNL( struct IS_CNL* packet )
{
	players.erase( packet->UCID );
}

void RCLight::InsimCPR( struct IS_CPR* packet )
{
	strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCLight::InsimMCI ( struct IS_MCI* pack_mci )
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
			Svetofor1( UCID );
		}
		else if (SvetKey == 2)
		{
			Svetofor2( UCID );
		}
		else
		{
			if (players[ UCID ].Light != 0)
			{
				for (int f=170; f < 203; f++)
					SendBFN( UCID ,f);

				players[ UCID ].Light = 0;
			}
		}

		if (players[ UCID ].Light3)
			Svetofor3( UCID );

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
				//SendMST("in zone");
				if( (D < 190+90) and (D > 190-90) )
				{
					if (players[ UCID ].WrongWay == 1)
					{
						players[ UCID ].WrongWay =0;
						SendBFN( UCID ,203);
						SendBFN( UCID ,204);
					}
				}
				else
				{
					if (players[ UCID ].WrongWay == 0)
						players[ UCID ].WrongWay =1;
					WrongWay( UCID );
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
						SendBFN( UCID ,203);
						SendBFN( UCID ,204);
					}
				}
				else
				{
					if (players[ UCID ].WrongWay == 0)
						players[ UCID ].WrongWay =1;
					WrongWay( UCID );
					if (S > 10)
						dl->RemSkill( UCID );
				}
			}
			else
			{
				if (players[ UCID ].WrongWay == 1)
				{
					players[ UCID ].WrongWay =0;
					SendBFN( UCID ,203);
					SendBFN( UCID ,204);
				}
			}
		}
    }
}

void RCLight::InsimMSO( struct IS_MSO* packet )
{
}

void RCLight::InsimNCN( struct IS_NCN* packet )
{
    if (packet->UCID == 0) return;
    strcpy( players[ packet->UCID ].UName, packet->UName);
    strcpy( players[ packet->UCID ].PName, packet->PName);
}

void RCLight::InsimNPL( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID ;
}

void RCLight::InsimPLP( struct IS_PLP* packet)
{
	PLIDtoUCID.erase( packet->PLID );
}

void RCLight::InsimPLL( struct IS_PLL* packet )
{
	PLIDtoUCID.erase( packet->PLID );
}

void RCLight::Svetofor1 ( byte UCID )
{
	printf("%d %d\n",red1,red2);
	printf("%d %d\n",yell1,yell2);
	printf("%d %d\n",green1,green2);
	printf("\n");

	byte ClickId = 170, l = 145, t = 12, w = 18;

	char cR [10], cY [10], cG [10];
	int R=8, Y=8, G=8;
	if (red1 == 1) R=1;
	if (yell1 == 1) Y=3;
	if (green1 == 1) G=2;

	sprintf(cR,"^%d^SЎс",R);
	sprintf(cY,"^%d^SЎс",Y);
	sprintf(cG,"^%d^SЎс",G);

	//подвес
	SendButton(255, UCID, ClickId++, l-3, t-3, 6, 6, 0, "^0^C^HЎ_");
	SendButton(255, UCID, ClickId++, l-1, 0, 2, 13, 32, "");
	SendButton(255, UCID, ClickId++, l-1, 0, 2, 13, 32, "");

	//подложка
	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+1, w-10, 13, 32, "");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+2 + 16/1.3, w-10, 13, 32, "");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+3 + 2*16/1.3, w-10, 13, 32, "");

	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+1, w-10, 3*16-9, 32, "");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+1, w-10, 3*16-9, 32, "");

	//уголки по бокам
	SendButton(255, UCID, ClickId++, l-8, t, 16, 16-1, 0, "^0^C^SЁђ               ЁЏ");
	SendButton(255, UCID, ClickId++, l-8, t + 16-3, 16, 16-1, 0, "^0^C^SЁђ               ЁЏ");
	SendButton(255, UCID, ClickId++, l-8, t + 2*16-6, 16, 16-1, 0, "^0^C^SЁђ               ЁЏ");

	//red
	SendButton(255, UCID, ClickId++, (l - w / 2), t-1, w, 16+1, 0, "  ^0^C^SЎс  ");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 4), t+2, w-8, 16-3, 7, cR);

	//yell
	SendButton(255, UCID, ClickId++, (l - w / 2), (t + 16 / 1.3), w, 16+1, 0, "  ^0^C^SЎс  ");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 4), (t + 16 / 1.3 + 3), w-8, 16-3, 7, cY);

	//green
	SendButton(255, UCID, ClickId++, (l - w / 2), (t + 1 + 2 * 16 / 1.3), w, 16+1, 0, "  ^0^C^SЎс  ");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 4), (t + 1 + 2 * 16 / 1.3 + 3), w-8, 16-3, 7, cG);
}

void RCLight::Svetofor2 ( byte UCID )
{
    byte ClickId = 170, l = 145, t = 12, w = 18;

	char cR [10], cY [10], cG [10];
	int R=8, Y=8, G=8;
	if (red2 == 1) R=1;
	if (yell2 == 1) Y=3;
	if (green2 == 1) G=2;

	sprintf(cR,"^%d^SЎс",R);
	sprintf(cY,"^%d^SЎс",Y);
	sprintf(cG,"^%d^SЎс",G);

	//подвес
	SendButton(255, UCID, ClickId++, l-3, t-3, 6, 6, 0, "^0^C^HЎ_");
	SendButton(255, UCID, ClickId++, l-1, 0, 2, 13, 32, "");
	SendButton(255, UCID, ClickId++, l-1, 0, 2, 13, 32, "");

	//подложка
	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+1, w-10, 13, 32, "");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+2 + 16/1.3, w-10, 13, 32, "");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+3 + 2*16/1.3, w-10, 13, 32, "");

	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+1, w-10, 3*16-9, 32, "");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t+1, w-10, 3*16-9, 32, "");

	//уголки по бокам
	SendButton(255, UCID, ClickId++, l-8, t, 16, 16-1, 0, "^0^C^SЁђ               ЁЏ");
	SendButton(255, UCID, ClickId++, l-8, t + 16-3, 16, 16-1, 0, "^0^C^SЁђ               ЁЏ");
	SendButton(255, UCID, ClickId++, l-8, t + 2*16-6, 16, 16-1, 0, "^0^C^SЁђ               ЁЏ");

	//red
	SendButton(255, UCID, ClickId++, (l - w / 2), t-1, w, 16+1, 0, "  ^0^C^SЎс  ");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 4), t+2, w-8, 16-3, 7, cR);

	//yell
	SendButton(255, UCID, ClickId++, (l - w / 2), (t + 16 / 1.3), w, 16+1, 0, "  ^0^C^SЎс  ");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 4), (t + 16 / 1.3 + 3), w-8, 16-3, 7, cY);

	//green
	SendButton(255, UCID, ClickId++, (l - w / 2), (t + 1 + 2 * 16 / 1.3), w, 16+1, 0, "  ^0^C^SЎс  ");
	SendButton(255, UCID, ClickId++, (l - w / 2 + 4), (t + 1 + 2 * 16 / 1.3 + 3), w-8, 16-3, 7, cG);
}

void RCLight::Svetofor3 ( byte UCID )
{
	const char* signal1 ="^0•";
	const char* signal2 ="^0•";
	const char* signal3 ="^0•";
	const char* signal11 ="^0•";
	const char* signal12 ="^0•";
	const char* signal13 ="^0•";

	if (red1 == 1) signal1 ="^1•";
	if (yell1 == 1) signal2 ="^3•";
	if (green1 == 1) signal3 ="^2•";
	if (red2 == 1) signal11 ="^1•";
	if (yell2 == 1) signal12 ="^3•";
	if (green2 == 1) signal13 ="^2•";

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
    pack_btn.L = 168;
    pack_btn.T = 112;
    pack_btn.W = 6;
    pack_btn.H = 17;
    strcpy(pack_btn.Text,"");
    insim->send_packet(&pack_btn);

    /**********/
    pack_btn.BStyle = 1;
    pack_btn.ClickID = 196;
    pack_btn.L = 165;
    pack_btn.T = 107;
    pack_btn.W = 12;
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
    /**** telo 2 *******/
    pack_btn.BStyle = 32;
    pack_btn.ClickID = 199;
    pack_btn.L = 162;
    pack_btn.T = 112;
    pack_btn.W = 6;
    pack_btn.H = 17;
    strcpy(pack_btn.Text,"");
    insim->send_packet(&pack_btn);
    /*************************/
    pack_btn.BStyle = 1;

    pack_btn.ClickID = 200;
    pack_btn.L = 159;
    pack_btn.T = 107;
    pack_btn.W = 12;
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

void RCLight::WrongWay(byte UCID)
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
    strcpy(pack_btn.Text,"^1•");
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

void RCLight::Event()
{
	float svtime = time(&sstime)%40;

	if (svtime == 0)
	{
		red1=0;		red2=1;
		yell1=0;	yell2=0;
		green1=1;	green2=0;
	}
	else if (svtime >= 14 and svtime<17)
	{
		if (gff)
		{
			red1=0;
			yell1=0;
			green1=1;
			gff = false;
		}
		else
		{
			red1=0;
			yell1=0;
			green1=0;
			gff = true;
		}
	}
	else if (svtime == 17)
	{
		gff = false;
		red1=0;		red2=1;
		yell1=1;	yell2=1;
		green1=0;	green2=0;
	}
	else if (svtime == 20)
	{
		red1=1;		red2=0;
		yell1=0;	yell2=0;
		green1=0;	green2=1;
	}
	else if (svtime >= 34 and svtime<37)
	{
		if (gff)
		{
			red2=0;
			yell2=0;
			green2=1;
			gff = false;
		}
		else
		{
			red2=0;
			yell2=0;
			green2=0;
			gff = true;
		}
	}
	else if (svtime == 37)
	{
		gff = false;
		red1=1;		red2=0;
		yell1=1;	yell2=1;
		green1=0;	green2=0;
	}
}

