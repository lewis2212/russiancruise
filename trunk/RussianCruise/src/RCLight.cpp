using namespace std;
#include "RCLight.h"

RCLight::RCLight(const char* Dir)
{
	strcpy(RootDir,Dir);
}

RCLight::~RCLight()
{

}

bool RCLight::SetLight3( byte UCID, bool Key)
{
	if (players[UCID].Light3 and !Key)
		for (int f = 121; f < 139; f++)
			insim->SendBFN( UCID, f);

	players[UCID].Light3 = Key;

	return true;
}

int RCLight::init(MYSQL *conn, CInsim *InSim, void *Message, void *RCDLic)
{
	dbconn = conn;
	if (!dbconn)
	{
		printf("RCLight: Can't sctruct MySQL Connector\n");
		return -1;
	} // �������� ���� �� ���������

	insim = InSim; // ����������� ��������� ������� ������
	if (!insim) // ��������� �� �������������
	{
		printf ("Can't struct CInsim class");
		return -1;
	}

	msg = (RCMessage *)Message;
	if (!msg)
	{
		printf ("Can't struct RCMessage class");
		return -1;
	}

	dl = (RCDL *)RCDLic;
	if (!dl)
	{
		printf ("Can't struct RCDL class");
		return -1;
	}

	CCText("^3R"+ClassName+":\t^2inited");
	return 0;
}

void RCLight::ReadConfig(const char *Track)
{
	config.clear();
	Light.clear();

	strcpy(TrackName, Track);
	char file[255];
	sprintf(file, "%s/data/RCLight/tracks/%s.json", RootDir, Track);

	ifstream readf (file, ios::in);

	if(!readf.is_open())
	{
		CCText("  ^7RCLight    ^1ERROR: ^8file " + (string)file + " not found");
		return ;
	}

	bool readed = configReader.parse( readf, config, false );

	if ( !readed )
	{
		readf.close();
		// report to the user the failure and their locations in the document.
		cout  << "Failed to parse configuration\n"
				   << configReader.getFormattedErrorMessages();
		return;
	}
	readf.close();

	if(config.isArray())
	{
		for(auto lg:config)
		{
			Lights lgh;
			memset(&lgh,0,sizeof(Lights));

			lgh.ID = lg["ID"].asInt();
			lgh.Heading = lg["Heading"].asInt();

			lgh.X = new int[lg["X"].size()];
			lgh.Y = new int[lg["Y"].size()];

			for (int k = 0; k < lg["X"].size(); k++)
			{
				lgh.X[k] = lg["X"][k].asInt();
				lgh.Y[k] = lg["Y"][k].asInt();
			}

			lgh.PointCount = lg["X"].size();

			Light.push_back( lgh );

		}

	}

   /* int i = 0;
	while (readf.good())
	{
		char str[128];
		readf.getline(str, 128);
		if (strlen(str) > 1)
		{
			if (strstr(str, "#Light") != nullptr)
			{
				Json::Value jLight;
				readf.getline(str, 128);
				Light[i].ID = atoi(str);

				readf.getline(str, 128);
				Light[i].Heading = atoi(str);

				// ������ ���������� �����
				readf.getline(str, 128);
				Light[i].PointCount = atoi(str);

				// ��������� ������ ����� � and Y
				Light[i].X = new int[Light[i].PointCount];
				Light[i].Y = new int[Light[i].PointCount];

				for (int k = 0; k<Light[i].PointCount; k++)
				{
					readf.getline(str, 128);

					Light[i].X[k] = 0;
					Light[i].Y[k] = 0;

					Light[i].X[k] = atoi(strtok (str, ";"));
					Light[i].Y[k] = atoi(strtok (NULL, ";"));

					jLight["X"].append(Light[i].X[k]);
					jLight["Y"].append(Light[i].Y[k]);
				}

				jLight["ID"] = Light[i].ID;
				jLight["Heading"] = Light[i].Heading;

				config.append( jLight );

				i++;
			}
		}
	}
	LightsCount = i;
	readf.close();*/

	/*sprintf(file, "%s/data/RCLight/tracks/%s.json", RootDir, Track);
	ofstream f;
	f.open(file, ios::out);
	f << configWriter.write( config );
	f.close();*/

	CCText("  ^7RCLight\t^2OK");
}

void RCLight::InsimCNL( struct IS_CNL* packet )
{
	players.erase( packet->UCID );
}

void RCLight::InsimCPR( struct IS_CPR* packet )
{
	players[ packet->UCID ].PName = packet->PName;
}

void RCLight::InsimMCI ( struct IS_MCI* pack_mci )
{
	for (int i = 0; i < pack_mci->NumC; i++)
	{
		byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];
		/** streets  **/
		int X = pack_mci->Info[i].X / 65536;
		int Y = pack_mci->Info[i].Y / 65536;
		int H = pack_mci->Info[i].Heading / 182;
		int D = pack_mci->Info[i].Direction / 182;
		int S = pack_mci->Info[i].Speed*360 / 32768;

		int SvetKey = 0;

		int g = 0;
		for (auto lgh: Light)
		{
			if (Check_Pos(lgh.PointCount, lgh.X, lgh.Y, X, Y))
			{
				players[UCID].LightNum = g++;
				int HR = lgh.Heading - 80;
				int HL = lgh.Heading + 80;

				if (HR < 0)
				{
					HR = 360 + HR;
					if ((H > HR) or (H < HL))
					{
						players[UCID].Light = lgh.ID;
						SvetKey = lgh.ID;
					}
				}
				else if (HL > 359)
				{
					HL -= 359;
					if ((H > HR) or (H < HL))
					{
						players[UCID].Light = lgh.ID;
						SvetKey = lgh.ID;
					}
				}
				else
				{
					if ((H > HR) and (H < HL))
					{
						players[UCID].Light = lgh.ID;
						SvetKey = lgh.ID;
					}
				}
				// ������, ��� ��� ����� ���� ��������� ����������
				// break;
			}
		}

		if (SvetKey == 1)
		{
			Svetofor1( UCID );
			players[UCID].DiffDir = abs(Light[players[UCID].LightNum].Heading - D);
		}
		else if (SvetKey == 2)
		{
			Svetofor2( UCID );
			players[UCID].DiffDir = abs(Light[players[UCID].LightNum].Heading - D);
		}
		else
		{
			if (players[UCID].Light != 0)
			{
				if (players[UCID].RedLight and players[UCID].DiffDir < 45 and (pack_mci->Info[i].AngVel * 360 / 16384) < 70)
				{
					players[UCID].OnRed = true;
				}
				else
				{
					players[UCID].OnRed = false;
				}

				for (int f = 121; f < 139; f++)
				{
					insim->SendBFN( UCID, f);
				}
				players[UCID].Light = 0;
			}
		}

		if (players[UCID].Light3)
		{
			Svetofor3( UCID );
		}

		/** pit wrong route **/
		if (strstr(TrackName, "SO4X"))
		{
			int pit1x[10] = {210, 200, 190, 200};
			int pit1y[10] = {233, 230, 277, 281};

			int pit2x[10] = {229, 236, 247, 239};
			int pit2y[10] = {-97, -39, -40, -100};

			if ( Check_Pos( 4, pit1x, pit1y, X, Y ) )
			{
				if ( (D < 190 + 90) and (D > 190 - 90) )
				{
					if (players[UCID].WrongWay == 1)
					{
						players[UCID].WrongWay = 0;
						insim->SendBFN( UCID, 140);
						insim->SendBFN( UCID, 141);
					}
				}
				else
				{
					if (players[UCID].WrongWay == 0)
						players[UCID].WrongWay =1;

					WrongWay( UCID );

					if (S > 10)
						dl->RemSkill( UCID );
				}
			}
			else if ( Check_Pos( 4, pit2x, pit2y, X, Y ) )
			{
				if ( (D < 190 + 90) and (D > 190 - 90) )
				{
					if (players[UCID].WrongWay == 1)
					{
						players[UCID].WrongWay =0;
						insim->SendBFN( UCID, 140);
						insim->SendBFN( UCID, 141);
					}
				}
				else
				{
					if (players[UCID].WrongWay == 0)
						players[UCID].WrongWay =1;

					WrongWay( UCID );

					if (S > 10)
						dl->RemSkill( UCID );
				}
			}
			else
			{
				if (players[UCID].WrongWay == 1)
				{
					players[UCID].WrongWay =0;
						insim->SendBFN( UCID, 140);
						insim->SendBFN( UCID, 141);
				}
			}
		}
	}
}

int RCLight::CheckLight(byte UCID)
{
	if (players[UCID].Light>0)
		if (players[UCID].RedLight)
			return 2;
		else if (players[UCID].GreenLight)
			return 1;
	return 0;
}

bool RCLight::CheckOnRed(byte UCID)
{
	return players[UCID].OnRed;
}

void RCLight::OnRedFalse(byte UCID)
{
	players[UCID].OnRed = false;
}

void RCLight::InsimMSO( struct IS_MSO* packet )
{
	string Message = packet->Msg + packet->TextStart;

	if (Message == "!light" and players[packet->UCID].Admin)
	{
		if (LightWorks == true)
		{
			LightWorks = false;
			insim->SendMTC(packet->UCID, "^1| ^7^C��������� ^1���������");
		}
		else
		{
			LightWorks = true;
			insim->SendMTC(packet->UCID, "^1| ^7^C��������� ^2��������");
		}
	}
}

void RCLight::InsimNCN( struct IS_NCN* packet )
{
	if (packet->UCID == 0)
	{
		return;
	}
	players[ packet->UCID ].UName = packet->UName;
	players[ packet->UCID ].PName = packet->PName;
	players[packet->UCID].Admin = packet->Admin;
}

void RCLight::InsimNPL( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID ;
}

void RCLight::InsimPLP( struct IS_PLP* packet)
{
	//PLIDtoUCID.erase( packet->PLID );
}

void RCLight::InsimPLL( struct IS_PLL* packet )
{
	PLIDtoUCID.erase( packet->PLID );
}

bool RCLight::GetOnLight(byte UCID)
{
	if (players[UCID].Light != 0)
		return true;

	return false;
}

void RCLight::Svetofor1 ( byte UCID )
{
	byte ClickId = 121, l = 145, t = 12, w = 18;

	char cR [10], cY [10], cG [10];
	int R = 8, Y = 8, G = 8;

	if (red1 == 1)
	{
		R = 1;
		players[UCID].RedLight = true;
	}
	else
	{
		players[UCID].RedLight = false;
	}

	if (yell1 == 1)
	{
		Y = 3;
		players[UCID].RedLight = false;
		players[UCID].GreenLight = false;
	}

	if (green1 == 1)
	{
		G = 2;
		players[UCID].GreenLight = true;
	}

	sprintf(cR, "^%d^S��", R);
	sprintf(cY, "^%d^S��", Y);
	sprintf(cG, "^%d^S��", G);

	//������
	insim->SendButton(255, UCID, ClickId++, l - 3, t - 3, 6, 6, 0, "^0^C^H�_");
	insim->SendButton(255, UCID, ClickId++, l - 1, 0, 2, 13, 32, "");
	insim->SendButton(255, UCID, ClickId++, l - 1, 0, 2, 13, 32, "");

	//��������
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 1, w - 10, 13, 32, "");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 2 + 16 / 1.3, w - 10, 13, 32, "");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 3 + 2*16 / 1.3, w - 10, 13, 32, "");

	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 1, w - 10, 3*16 - 9, 32, "");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 1, w - 10, 3*16 - 9, 32, "");

	//������ �� �����
	insim->SendButton(255, UCID, ClickId++, l - 8, t, 16, 16 - 1, 0, "^0^C^S��               ��");
	insim->SendButton(255, UCID, ClickId++, l - 8, t + 16 - 3, 16, 16 - 1, 0, "^0^C^S��               ��");
	insim->SendButton(255, UCID, ClickId++, l - 8, t + 2*16 - 6, 16, 16 - 1, 0, "^0^C^S��               ��");

	//red
	insim->SendButton(255, UCID, ClickId++, (l - w / 2), t - 1, w, 16 + 1, 0, "  ^0^C^S��  ");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 4), t + 2, w - 8, 16 - 3, 7, cR);

	//yell
	insim->SendButton(255, UCID, ClickId++, (l - w / 2), (t + 16 / 1.3), w, 16 + 1, 0, "  ^0^C^S��  ");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 4), (t + 16 / 1.3 + 3), w - 8, 16 - 3, 7, cY);

	//green
	insim->SendButton(255, UCID, ClickId++, (l - w / 2), (t + 1 + 2 * 16 / 1.3), w, 16 + 1, 0, "  ^0^C^S��  ");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 4), (t + 1 + 2 * 16 / 1.3 + 3), w - 8, 16 - 3, 7, cG);
}

void RCLight::Svetofor2 ( byte UCID )
{
	byte ClickId = 121, l = 145, t = 12, w = 18;

	char cR [10], cY [10], cG [10];
	int R = 8, Y = 8, G = 8;

	if (red2 == 1)
	{
		R = 1;
		players[UCID].RedLight = true;
	}
	else
	{
		players[UCID].RedLight = false;
	}

	if (yell2 == 1)
	{
		Y = 3;
		players[UCID].RedLight = false;
		players[UCID].GreenLight = false;
	}

	if (green2 == 1)
	{
		G = 2;
		players[UCID].GreenLight = true;
	}

	sprintf(cR, "^%d^S��", R);
	sprintf(cY, "^%d^S��", Y);
	sprintf(cG, "^%d^S��", G);

	//������
	insim->SendButton(255, UCID, ClickId++, l - 3, t - 3, 6, 6, 0, "^0^C^H�_");
	insim->SendButton(255, UCID, ClickId++, l - 1, 0, 2, 13, 32, "");
	insim->SendButton(255, UCID, ClickId++, l - 1, 0, 2, 13, 32, "");

	//��������
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 1, w - 10, 13, 32, "");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 2 + 16 / 1.3, w - 10, 13, 32, "");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 3 + 2*16 / 1.3, w - 10, 13, 32, "");

	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 1, w - 10, 3*16 - 9, 32, "");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 5), t + 1, w - 10, 3*16 - 9, 32, "");

	//������ �� �����
	insim->SendButton(255, UCID, ClickId++, l - 8, t, 16, 16 - 1, 0, "^0^C^S��               ��");
	insim->SendButton(255, UCID, ClickId++, l - 8, t + 16 - 3, 16, 16 - 1, 0, "^0^C^S��               ��");
	insim->SendButton(255, UCID, ClickId++, l - 8, t + 2 * 16 - 6, 16, 16 - 1, 0, "^0^C^S��               ��");

	//red
	insim->SendButton(255, UCID, ClickId++, (l - w / 2), t - 1, w, 16 + 1, 0, "  ^0^C^S��  ");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 4), t + 2, w - 8, 16 - 3, 7, cR);

	//yell
	insim->SendButton(255, UCID, ClickId++, (l - w / 2), (t + 16 / 1.3), w, 16 + 1, 0, "  ^0^C^S��  ");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 4), (t + 16 / 1.3 + 3), w - 8, 16 - 3, 7, cY);

	//green
	insim->SendButton(255, UCID, ClickId++, (l - w / 2), (t + 1 + 2 * 16 / 1.3), w, 16 + 1, 0, "  ^0^C^S��  ");
	insim->SendButton(255, UCID, ClickId++, (l - w / 2 + 4), (t + 1 + 2 * 16 / 1.3 + 3), w - 8, 16 - 3, 7, cG);
}

void RCLight::Svetofor3 ( byte UCID )
{
	//if (police->GetCopRank(UCID) <= 2) return;

	const char* signal1 ="^0�";
	const char* signal2 ="^0�";
	const char* signal3 ="^0�";
	const char* signal11 ="^0�";
	const char* signal12 ="^0�";
	const char* signal13 ="^0�";

	if (red1 == 1)
		signal1 ="^1�";

	if (yell1 == 1)
		signal2 ="^3�";

	if (green1 == 1)
		signal3 ="^2�";

	if (red2 == 1)
		signal11 ="^1�";

	if (yell2 == 1)
		signal12 ="^3�";

	if (green2 == 1)
		signal13 ="^2�";

	byte id = 140;
	insim->SendButton(255, UCID, id++, 168, 112, 6, 17, 32, "");    //telo1

	insim->SendButton(255, UCID, id++, 165, 107, 12, 16, 1, signal1);
	insim->SendButton(255, UCID, id++, 165, 112, 12, 16, 1, signal2);
	insim->SendButton(255, UCID, id++, 165, 117, 12, 16, 1, signal3);

	insim->SendButton(255, UCID, id++, 162, 112, 6, 17, 32, "");    //telo2

	insim->SendButton(255, UCID, id++, 159, 107, 12, 16, 1, signal11);
	insim->SendButton(255, UCID, id++, 159, 112, 12, 16, 1, signal12);
	insim->SendButton(255, UCID, id++, 159, 117, 12, 16, 1, signal13);
}

void RCLight::WrongWay(byte UCID)
{
	insim->SendButton(255, UCID, 140, 0, 0, 200, 200, 0, "^1�");
	insim->SendButton(255, UCID, 141, 25, 26, 150, 150, 3, "-");
}

void RCLight::Event()
{
	if (!LightWorks)
	{
		if (gff)
		{
			red1 = 0;
			red2 = 0;
			yell1 = 1;
			yell2 = 1;
			green1 = 0;
			green2 = 0;
			gff = false;
		}
		else
		{
			red1 = 0;
			red2 = 0;
			yell1 = 0;
			yell2 = 0;
			green1 = 0;
			green2 = 0;
			gff = true;
		}
		return;
	}

	float svtime = time(&sstime)%40;

	if (svtime >= 0 and svtime<14)
	{
		red1 = 0;
		red2 = 1;
		yell1 = 0;
		yell2 = 0;
		green1 = 1;
		green2 = 0;
	}
	else if (svtime >= 14 and svtime<17)
	{
		if (gff)
		{
			red1 = 0;
			yell1 = 0;
			green1 = 1;
			gff = false;
		}
		else
		{
			red1 = 0;
			yell1 = 0;
			green1 = 0;
			gff = true;
		}
	}
	else if (svtime == 17)
	{
		gff = false;
		red1 = 0;
		red2 = 1;
		yell1 = 1;
		yell2 = 1;
		green1 = 0;
		green2 = 0;
	}
	else if (svtime >= 20 and svtime<34)
	{
		red1 = 1;
		red2 = 0;
		yell1 = 0;
		yell2 = 0;
		green1 = 0;
		green2 = 1;
	}
	else if (svtime >= 34 and svtime<37)
	{
		if (gff)
		{
			red2 = 0;
			yell2 = 0;
			green2 = 1;
			gff = false;
		}
		else
		{
			red2 = 0;
			yell2 = 0;
			green2 = 0;
			gff = true;
		}
	}
	else if (svtime == 37)
	{
		gff = false;
		red1 = 1;
		red2 = 0;
		yell1 = 1;
		yell2 = 1;
		green1 = 0;
		green2 = 0;
	}
}
