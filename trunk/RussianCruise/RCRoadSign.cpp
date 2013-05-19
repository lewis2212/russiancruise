#include "RCRoadSign.h"

RCRoadSign::RCRoadSign(){}
RCRoadSign::~RCRoadSign(){}

int RCRoadSign::Init(const char *dir,void *CInSim, void *Message, void * Light)
{
	strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        printf ("Can't struct CInsim class");
        return -1;
    }
    lgh = (RCLight *)Light;
    if(!lgh)
    {
        printf ("Can't struct RCLight class");
        return -1;
    }
    return 0;
}

void RCRoadSign::ReadConfig(const char *Track)
{
	cout << "RCRoadSign::readconfig\n" ;
	strcpy(TrackName, Track);
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCRoadSign\\tracks\\%s.txt",RootDir,Track);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "RCRoadSign: Can't find " << file << endl;
        return ;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);
    int i=0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);
        if (strlen(str) > 0)
        {
        	int pr = atoi(strtok(str," "));
        	if (pr!=0)
			{
                Signs sgn;
                sgn.ID = pr;
                sgn.X = atoi(strtok(NULL," "));
                sgn.Y = atoi(strtok(NULL," "));
                sgn.Heading = atoi(strtok(NULL," "));
				Sign.push_back( sgn );
				//printf("%d %d %d %d\n",Sign[i].X,Sign[i].Y,Sign[i].Heading,Sign[i].ID);
				i++;
			}
        }
    }
    readf.close();

    sprintf(file,"%sdata\\RCRoadSign\\sign.txt",RootDir);

    HANDLE ff;
    // WIN32_FIND_DATA fd;
    ff = FindFirstFile(file,&fd);
    if (ff == INVALID_HANDLE_VALUE)
    {
        printf ("RCRoadSign: Can't find \n%s",file);
        return;
    }
    FindClose(ff);

    ifstream read (file,ios::in);
    while (read.good())
    {
        char str[128];
        read.getline(str,128);

        if (strlen(str) > 0)
        {
        	int pr = atoi(strtok(str,";"));
        	if (pr!=0)
			{
				SignName[i].ID = pr;
				sprintf(SignName[i].Name,"^C%s",strtok(NULL,";"));

				//printf("%d %s\n",SignName[i].ID,SignName[i].Name);
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
        return;
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
	players.erase(packet->UCID);
}

void RCRoadSign::InsimCPR( struct IS_CPR* packet )
{
	strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCRoadSign::InsimMSO(struct IS_MSO* packet)
{
	byte UCID = packet->UCID;
	if (UCID == 0)
        return;
	if (packet->UserType != MSO_PREFIX)
        return;

	char Msg[128];
    strcpy(Msg,packet->Msg + ((unsigned char)packet->TextStart));

    if (strncmp(Msg, "!s_add", 6) == 0)
    {
        char file[MAX_PATH],text[96];

        sprintf(file,"%sdata\\RCRoadSign\\tracks\\%s.txt",RootDir,TrackName);

        int X = players[UCID].Info.X/65536;
        int Y = players[UCID].Info.Y/65536;
        int Dir = players[UCID].Info.Direction/182;

        int ID=0;

        strtok(Msg," ");
		int s = atoi(strtok(NULL," "));
		if (s == 0)
		{
			sprintf(text,"^1Error: ^Cукажи ID");
			SendMTC(UCID,text);
			return;
		}
		else
			ID = s;

        sprintf(text,"%d %d %d %d",ID,X,Y,Dir);

		ofstream listSing( file , ios::app );
		listSing << text <<  endl;
		listSing.close();

		sprintf(text,"^2Sign added (ID = %d, Dir = %d)",ID,Dir);
        SendMTC(UCID,text);
    }
}

void RCRoadSign::ShowSign(byte UCID, byte ID, byte Count)
{
	byte id = 90, l = 122, t = 0+27*Count, w = 46, h=0;
	if (lgh->GetOnLight(UCID)) {t+=44;h=52;}

	//подвес
	//SendButton(255, UCID, id++, l, 0+h, 1, 15+27*Count, 32, "");
	//SendButton(255, UCID, id++, l, 0+h, 1, 15+27*Count, 32, "");
	//SendButton(255, UCID, id++, l, 0+h, 1, 15+27*Count, 32, "");
	id = 93+10*Count;

	/** главна€ дорога 2.1 **/
	if(ID==1)
	{
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^0^JБЯ");
		w-=2;t+=1;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^7^JБЯ");
		w-=10;t+=5;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^0^JБЯ");
		w-=2;t+=1;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^3^JБЯ");
	}
	/** уступи дорогу 2.4 **/
	if(ID==2)
	{
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^0^JБ•");
		w-=2;t+=1;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^1^JБ•");
		w-=12;t+=6;
		SendButton(255, UCID, id++, l-w/2, t-1, w, w, 0, "^7^JБ•");
	}
	/** иск. неровность 1.17 **/
	if(ID==3)
	{
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^0^JБ£");
		w-=2;t+=1;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^1^JБ£");
		w-=12;t+=6;
		SendButton(255, UCID, id++, l-w/2, t+1, w, w, 0, "^7^JБ£");
		w-=14;t+=16;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^0^JБ№");
		w+=6;t-=13;
		SendButton(255, UCID, id++, l-w/2+1, t, w-1, w, 0, "^0^JБQ");
	}
	/** пешеходный переход 1.22 **/
	if(ID==4)
	{
	}
	/** дорожные работы 1.25 **/
	if(ID==5)
	{
	}
	/** проезд запрещен 3.1 **/
	if(ID==6)
	{
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^0^JБЬ");
		w-=2;t+=1;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^7^JБЬ");
		w-=4;t+=2;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^1^JБЬ");
		SendButton(255, UCID, id++, l-w/2, t-1, w, w, 0, "^7^K£≠");
		SendButton(255, UCID, id++, l-w/2, t+1, w, w, 0, "^7^K£≠");
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^7^K£≠");
	}
	/** кольцева€ дорога 4.3 **/
	/** ограничение скорости 3.24 **/
	/** сто€нка 6.4 **/
	if(ID==9)
	{
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^0^JБ°");
		w-=2;t+=1;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^7^JБ°");
		w-=4;t+=2;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 6, "^JБ°");
		w-=10;t+=5;
		SendButton(255, UCID, id++, l-w/2, t, w, w, 0, "^7^K£–");
	}
	/** тупик 6.8 **/
	if(ID==10)
	{
	}
	/** поворот запрещен 3.18 **/
	if(ID==11)
	{
	}
	/** разворот запрещен 3.19 **/
	if(ID==12)
	{
	}
}

void RCRoadSign::InsimMCI ( struct IS_MCI* packet )
{
    for (int i = 0; i < packet->NumC; i++)
    {
		byte UCID = PLIDtoUCID[ packet->Info[i].PLID ];

		int X = packet->Info[i].X/65536;
		int Y = packet->Info[i].Y/65536;
		int H = packet->Info[i].Heading/182;
		int S = ((int)packet->Info[i].Speed*360)/(32768);

		int SignCount = 0;
		for ( auto& sign: Sign)
		{
			if ( Distance(X, Y, sign.X, sign.Y ) < 50 and abs( H - sign.Heading ) < 40 )
			{
				ShowSign(UCID, sign.ID, SignCount);
				players[UCID].OnSign = true;
				SignCount++;
				break;
			}
		}

		if ( SignCount != players[UCID].SignCount and players[UCID].OnSign)
		{
			players[UCID].OnSign = false;
			for (int f = 90 + 10 * SignCount; f < 160; f++)
				SendBFN(UCID, f);
			players[UCID].SignCount = SignCount;

		}

		if (S>1)
		memcpy(&players[UCID].Info, &packet->Info[i], sizeof(struct CompCar) );
    }
}
