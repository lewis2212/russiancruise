#include "RCPizza.h"

time_t  ptime;
pthread_t tid; // Thread ID

RCPizza::RCPizza(const char* Dir)
{
    strcpy(RootDir,Dir);
    memset(&PStore, 0, sizeof(Store));
    memset(&TrackInf, 0, sizeof(pizza_info));
    memset(&zone, 0, sizeof(place));
}

RCPizza::~RCPizza()
{

}

int RCPizza::init(DBMySQL *db, CInsim *InSim, void *RCMessageClass, void *Bank, void *Energy, void *DrLic, void * STreet)
{
    this->db = db;
    if (!this->db)
    {
        printf("RCPizza: Can't sctruct MySQL Connector\n");
        return -1;
    }

    insim = InSim;
    if (!insim)
    {
        printf ("RCPizza: Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)RCMessageClass;
    if (!msg)
    {
        printf ("RCPizza: Can't struct RCMessage class");
        return -1;
    }

    bank = (RCBank *)Bank;
    if (!bank)
    {
        printf ("RCPizza: Can't struct RCBank class");
        return -1;
    }

    nrg = (RCEnergy *)Energy;
    if (!nrg)
    {
        printf ("RCPizza: Can't struct RCEnergy class");
        return -1;
    }

    dl = (RCDL *)DrLic;
    if (!dl)
    {
        printf ("RCPizza: Can't struct RCDL class");
        return -1;
    }

    street = (RCStreet *)STreet;
    if (!street)
    {
        printf ("Can't struct RCStreet class");
        return -1;
    }

    ginfo_time = time(&ptime)+60;

    CCText("^3R"+ClassName+":\t^2inited");
    return 0;
}

void RCPizza::Deal(byte UCID)
{

    if (players[UCID].WorkType == WK_NULL)
    {
        if (CarsInWork <= NumCars)
        {
            players[UCID].WorkType = WK_PIZZA;
            players[UCID].WorkAccept = 0;
            players[UCID].FreeEat = false;
            insim->SendMTC(UCID, msg->_(UCID, "4000"));
            CarsInWork ++;
        }
        else
        {
            insim->SendMTC(UCID, msg->_(UCID, "4104"));
            return;
        }
    }
    else if (players[UCID].WorkType == WK_PIZZA)
    {
        insim->SendMTC(UCID, msg->_(UCID, "4001"));
    }
    else
    {
        insim->SendMTC(UCID, msg->_(UCID, "4002"));
    }
}

void RCPizza::Take(byte UCID)
{
    if (players[UCID].WorkType != WK_PIZZA)
    {
        insim->SendMTC(UCID, msg->_(UCID, "4002"));
        return;
    }

    if (players[UCID].CName != "UF1")
    {
        insim->SendMTC(UCID, msg->_(UCID, "4102"));
        return;
    }

    if (players[UCID].WorkAccept == 1)
    {
        /**
        *	====	������� �����	====
        *
        *   ���� ������ ����� - 1 �� ���� 0.6 �� ����, 0.9 �������, 0.4 ����
        *   ������������� �� ������ = 3+ 3 +40 +83 = 132 ���
        *   ������������� ���� 320 ���
        *   ������ �� ������������ 188 ���
        *   ������ �� �������� 240 ���.
        **/

        PStore.Muka -= 0.25; // 3 rur
        PStore.Voda -= 0.2;    // 3
        PStore.Ovoshi -= 0.5;   //40
        PStore.Cheese -= 0.15; // 83

        if (players[UCID].WorkPlayerAccept == 0) // ���� ������� �����
        {
            srand(time(NULL));
            int place = rand()%zone.NumPoints;

            int worktime = time(&ptime);
            players[UCID].WorkTime = worktime + PIZZA_WORK_TIME;
            players[UCID].WorkDestinaion = place;
            players[UCID].WorkAccept = 2;

            char str[64];

            int StreetID = -1;
            for (int g = 0; g < street->StreetNums; g++)
			{
                if (Check_Pos(street->Street[g].PointCount, street->Street[g].StreetX, street->Street[g].StreetY, zone.point[place].X, zone.point[place].Y))
                {
                    StreetID = g;
                    break;
                }
			}

            sprintf(str, msg->_(UCID, "4200"), msg->_(UCID, zone.point[place].PlaceName), street->GetStreetName(UCID, StreetID));
            insim->SendMTC(UCID, "^3| " +  (string)str);

            strcpy(players[UCID].WorkDest, str);
        }
        else if (players[UCID].WorkPlayerAccept != 0) // ������� �����
        {
            int worktime = time(&ptime);

            players[UCID].WorkTime = worktime + PIZZA_WORK_TIME;
            players[UCID].WorkDestinaion = players[UCID].WorkPlayerAccept;
            players[UCID].WorkAccept = 2;
            char text[128];
            sprintf(text, msg->_(UCID, "4201"), players[players[UCID].WorkPlayerAccept].PName.c_str());
            strcpy(players[UCID].WorkDest, text+4);
            insim->SendMTC(UCID, text);
        }
    }

    else if (players[UCID].WorkAccept == 2)
        insim->SendMTC(UCID, msg->_(UCID, "4202")); // ������ ������� ���� �����
    else
        insim->SendMTC(UCID, msg->_(UCID, "4203")); // Wait until i call you
}

void RCPizza::Done(byte UCID)
{
    if ((players[UCID].WorkType == WK_PIZZA) and (players[UCID].WorkAccept == 2))
    {
        insim->SendMTC(UCID, msg->_(UCID, "4300"));

        players[UCID].WorkPlayerAccept = 0;
        players[UCID].WorkDestinaion = 0;
        players[UCID].WorkCountDone ++;
        players[UCID].WorkAccept = 0;
        players[UCID].WorkZone = 0;
        ClearButtonInfo(UCID);

        if((players[UCID].WorkCountDone % 10) == 0)
        {
            players[UCID].FreeEat = true;
            insim->SendMTC(UCID, msg->_(UCID, "4213"));
        }

        int cash = 50 * abs(1 - (players[UCID].WorkTime - time(&ptime)) / PIZZA_WORK_TIME);
        bank->AddCash(UCID, 248 + cash, true);
        Capital += 420 - cash;

        dl->AddSkill(UCID);
        insim->SendBFN(UCID, 212);
    }
}

void RCPizza::ReadConfig(const char *Track)
{
    char file[MAX_PATH];
    sprintf(file, "%s/data/RCPizza/%s", RootDir, Track);

	msg->ReadLangDir(file);

    sprintf(file, "%s/data/RCPizza/%s/points.txt", RootDir, Track);

    ifstream readf;
    readf.open(file, ios::in);

    if(!readf.is_open())
    {
        CCText("  ^7RCPizza    ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }
    int point = 0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);

        if (strlen(str) > 1)
        {
            if (strstr(str, "/dealer") != NULL)
            {
                for (int i=0; i<4; i++)
                {
                    readf.getline(str, 128);
                    char * X;
                    char * Y;
                    X = strtok (str, ", ");
                    Y = strtok (NULL, ", ");
                    zone.dealX[i] = atoi(X);
                    zone.dealY[i] = atoi(Y);
                }
            }

            if (strstr(str, "/shop") != NULL)
            {
                readf.getline(str, 128);
                int count = atoi(str);
                TrackInf.ShopCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str, 128);
                    char * X;
                    char * Y;
                    X = strtok (str, ";");
                    Y = strtok (NULL, ";");
                    TrackInf.XShop[i] = atoi(X);
                    TrackInf.YShop[i] = atoi(Y);
                }
            }

            if (strstr(str, "point") != NULL)
            {
                readf.getline(str, 64);
                strncpy(zone.point[point].PlaceName, str, strlen(str));

                readf.getline(str, 128);

                char * X;
                char * Y;
                X = strtok (str, ", ");
                Y = strtok (NULL, ", ");

                zone.point[point].X = atoi(X);
                zone.point[point].Y = atoi(Y);

                point ++;
                zone.NumPoints = point;
            }
        }
    }
    readf.close();

    /*** READ STORE DATA ***/
    sprintf(file, "%s/data/RCPizza/_Store.txt", RootDir);

    ifstream ReadStore(file, ios::in);

    if(!ReadStore.is_open())
    {
        CCText("  ^7RCPizza    ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }

    while (ReadStore.good())
    {
        char str[128];
        ReadStore.getline(str, 128);

        if (strlen(str) > 1)
        {
            if (strstr(str, "Muka=") != NULL)
                PStore.Muka = atof(str+strlen("Muka="));

            if (strstr(str, "Voda=") != NULL)
                PStore.Voda = atof(str+strlen("Voda="));

            if (strstr(str, "Ovoshi=") != NULL)
                PStore.Ovoshi = atof(str+strlen("Ovoshi="));

            if (strstr(str, "Cheese=") != NULL)
                PStore.Cheese = atof(str+strlen("Cheese="));
        }
    }
    ReadStore.close();

    sprintf(file, "%s/data/RCPizza/_Pizza.txt", RootDir);

    ifstream ReadInfo (file, ios::in);

    if (!ReadInfo.is_open())
    {
    	CCText("  ^7RCPizza    ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }

    while (ReadInfo.good())
    {
        char str[128];
        ReadInfo.getline(str, 128);

        if (strlen(str) > 1)
        {
            if (strstr(str, "Capital=") != NULL)
                Capital = atoi(str+strlen("Capital="));

            if (strstr(str, "NumCars=") != NULL)
                NumCars = atoi(str+strlen("NumCars="));
        }
    }
    ReadInfo.close();

    CCText("  ^7RCPizza\t^2OK");
}


void RCPizza::InsimNCN(struct IS_NCN* packet)
{
    NumP = packet->Total;
    players[packet->UCID].UName = packet->UName;
    players[packet->UCID].PName = packet->PName;
    players[packet->UCID].Admin = packet->Admin;
}

void RCPizza::InsimCNL(struct IS_CNL* packet)
{
    if (players[packet->UCID].WorkAccept == 3)
        ShopAccepted = false;

    if (players[packet->UCID].WorkType == WK_PIZZA)
        CarsInWork--;

    if (players[packet->UCID].Pizza == 1)
	{
        for (auto& p: players)
		{
            if (players[p.first].WorkPlayerAccept == packet->UCID)
            {
                players[p.first].WorkPlayerAccept = 0;
                break;
            }
		}
	}

    players.erase(packet->UCID);
    NumP = packet->Total;

    /** Save Pizza Info **/
    char file[MAX_PATH];
    sprintf(file, "%s/data/RCPizza/_Store.txt", RootDir);
    ofstream writef (file, ios::out);
    writef << "Muka=" << PStore.Muka << endl;
    writef << "Voda=" << PStore.Voda << endl;
    writef << "Ovoshi=" << PStore.Ovoshi << endl;
    writef << "Cheese=" << PStore.Cheese << endl;
    writef.close();
    sprintf(file, "%s/data/RCPizza/_Pizza.txt", RootDir);
    ofstream WriteInfo (file, ios::out);
    WriteInfo << "Capital=" << Capital << endl;
    WriteInfo << "NumCars=" << NumCars << endl;
    WriteInfo.close();
}

void RCPizza::InsimNPL(struct IS_NPL* packet)
{
    PLIDtoUCID[packet->PLID] = packet->UCID;
    players[packet->UCID].CName = packet->CName;
}

void RCPizza::InsimPLP(struct IS_PLP* packet)
{
}

void RCPizza::InsimPLL(struct IS_PLL* packet)
{
    PLIDtoUCID.erase(packet->PLID);
}

void RCPizza::InsimCPR(struct IS_CPR* packet)
{
    players[packet->UCID].PName = packet->PName;
}

void RCPizza::InsimMCI (struct IS_MCI* pack_mci)
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
        byte UCID = PLIDtoUCID[pack_mci->Info[i].PLID];

        int X = pack_mci->Info[i].X / 65536;
        int Y = pack_mci->Info[i].Y / 65536;
        int S = ((int)pack_mci->Info[i].Speed * 360) / (32768);

        // �������� ���� ���� ����� � ���� �������� � �������� ����� ����
        if (check_pos(UCID) == 1)
        {
            if (players[UCID].Zone != 4)
            {
                players[UCID].Zone = 4;
                insim->SendMTC(UCID, msg->_(UCID, "1600")); // pizza u Jony

                if (players[UCID].FreeEat)
					insim->SendMTC(UCID, msg->_(UCID, "FreeEat")); // omnomnom

                if (players[UCID].WorkType != WK_PIZZA)
                    insim->SendMTC(UCID, msg->_(UCID, "1601")); // deal
                else
                    insim->SendMTC(UCID, msg->_(UCID, "1602")); // undeal

                if (players[UCID].FreeEat)
					insim->SendMTC(UCID, msg->_(UCID, "FreeEat")); // undeal

                if (players[UCID].WorkAccept != 0)
                    Take(UCID);
            }
        }
        else
            players[UCID].Zone = 0;


        /** Zones (PitSave, shop, etc) **/
        if (Distance(X, Y, zone.point[players[UCID].WorkDestinaion].X, zone.point[players[UCID].WorkDestinaion].Y) < 10 and S < 5)
        {
            Done(UCID);
        }

        byte PL_UCID = players[UCID].WorkPlayerAccept;

        if (PL_UCID != 0 and players[UCID].WorkAccept == 2 and S < 5 and players[PL_UCID].Info.Speed / 65536 < 5)
        {
            int PL_X = players[PL_UCID].Info.X / 65536;
            int PL_Y = players[PL_UCID].Info.Y / 65536;

            if (Distance(X, Y, PL_X, PL_Y) < 10) // ���� ��������� �� ������ ������ 10 ������
            {
                Done(UCID);
                bank->RemCash(PL_UCID, 800);
                nrg->AddEnergy(PL_UCID, 8000);
                insim->SendMTC(PL_UCID, msg->_(PL_UCID, "1604"));
                players[PL_UCID].Pizza = 0;
            }
        }

        if ((players[UCID].WorkAccept == 3) && (Check_Pos(TrackInf.ShopCount, TrackInf.XShop, TrackInf.YShop, X, Y))) // ���� ������ ������� �������� �������� � �� ������� � �������
        {
            players[UCID].WorkDestinaion =0;
            players[UCID].WorkAccept = 0;
            players[UCID].WorkPlayerAccept = 0;
            //insim->SendBFN(UCID, 212);
            insim->SendMTC(UCID, msg->_(UCID, "4205"));

            // ��������� ����� �� ��������
            if (PStore.Muka < 10)
            {
                PStore.Muka += 99;
                Capital -= 99 * 12;
                bank->AddToBank(99 * 12);
            }

            if (PStore.Voda < 10)
            {
                PStore.Voda += 99;
                Capital -= 99 * 10;
                bank->AddToBank(99 * 10);
            }

            if (PStore.Ovoshi < 10)
            {
                PStore.Ovoshi += 99;
                Capital -= 99 * 80;
                bank->AddToBank(99 * 80);
            }

            if (PStore.Cheese < 10)
            {
                PStore.Cheese += 99;
                Capital -= 99 * 560;
                bank->AddToBank(99 * 560);
            }

            ShopAccepted = false;
        }

        players[UCID].Info = pack_mci->Info[i];
    }
}

void RCPizza::InsimMSO(struct IS_MSO* packet)
{
    if (packet->UCID == 0)
        return;

    char Message[128];
    strcpy(Message,packet->Msg + packet->TextStart);

    if (strncmp(Message, "!pstat", 6) == 0)
    {
        char Text[96];
        sprintf(Text, msg->_(packet->UCID, "4206"), Capital);
        insim->SendMTC(packet->UCID, Text);
        sprintf(Text, msg->_(packet->UCID, "4207"), PStore.Voda, PStore.Muka, PStore.Ovoshi, PStore.Cheese);
        insim->SendMTC(packet->UCID, Text);

        if (CarsInWork==0)
        {
            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "4208"));
        }
        else
        {
            sprintf(Text, msg->_(packet->UCID, "4209"), CarsInWork, NumCars);
            insim->SendMTC(packet->UCID, Text);

            for (auto& p: players)
			{
                if (players[p.first].WorkType == WK_PIZZA)
                {
                    sprintf(Text, msg->_(packet->UCID, "4210"), players[p.first].PName.c_str(), players[p.first].WorkDest, players[p.first].WorkCountDone);
                    insim->SendMTC(packet->UCID, Text);
                }
			}
        }
    }

    if (strncmp(Message, "!eat", 4) == 0
    and check_pos(packet->UCID) == 1 and players[packet->UCID].WorkType == WK_PIZZA)
    {
        if (players[packet->UCID].FreeEat)
        {
            nrg->AddEnergy(packet->UCID, 80000);
            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "4211"));
            players[packet->UCID].FreeEat = false;
        }
        else
            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "4212"));
    }

    if (strncmp(Message, "!deal", 5) == 0)
	{
        if ((check_pos(packet->UCID) == 1) and (players[packet->UCID].WorkType == 0))
        {
            if (dl->GetLVL(packet->UCID) > 19)
            {
                insim->SendMTC(packet->UCID, msg->_(packet->UCID, "4103"));
                return;
            }

            if (players[packet->UCID].CName == "UF1")
            {
                Deal(packet->UCID);
            }
            else
            {
                insim->SendMTC(packet->UCID, msg->_(packet->UCID, "4102"));
            }
        }
	}

    if (strncmp(Message, "!undeal", 7) == 0)
    {
        if (check_pos(packet->UCID) == 1 and players[packet->UCID].WorkType == WK_PIZZA)
        {
            if (players[packet->UCID].WorkAccept == 3)
            {
                ShopAccepted = false;
            }

            insim->SendBFN(packet->UCID, 212);
            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "4100"));
            players[packet->UCID].WorkType = WK_NULL;
            players[packet->UCID].WorkDestinaion =0;
            players[packet->UCID].WorkAccept = 0;
            players[packet->UCID].FreeEat = false;
            CarsInWork --;
        }
    }

    //!pizza
    if (strncmp(Message, "!tofu", 5) == 0)
    {
        if (nrg->GetEnergy(packet->UCID) > 80)
        {
            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "NotLesEn"));
            return;
        }

        if (players[packet->UCID].WorkType == WK_PIZZA)
        {
            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "NoTofuWhWork"));
            return;
        }

        if (players[packet->UCID].Pizza == 0)
        {
            players[packet->UCID].Pizza = 1;
            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "DelTofu")); // ����� ������
        }
        else
            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "UAreDel")); //�� ��� �������
    }

    //!test
    if (strncmp(Message, "!test", 5) == 0)
    {
        for (int i=0;i<zone.NumPoints;i++)
        {
            char str[64];

            int StreetID = -1;
            for (int g = 0; g < street->StreetNums; g++)
			{
                if (Check_Pos(street->Street[g].PointCount, street->Street[g].StreetX, street->Street[g].StreetY, zone.point[i].X, zone.point[i].Y))
                {
                    StreetID = g;
                    break;
                }
			}

            sprintf(str, msg->_(packet->UCID, "4200"), msg->_(packet->UCID, zone.point[i].PlaceName), street->GetStreetName(packet->UCID, StreetID));
            insim->SendMTC(packet->UCID, "^3| " +  (string)str);
        }
    }
}

void RCPizza::btn_work (byte UCID)
{
    char str[10];
    int time2 = players[UCID].WorkTime - time(&ptime);
    sprintf(str, "^2%02d:%02d", time2 / 60, time2%60);
    insim->SendButton(255, UCID, 212, 130, 1, 10, 8, 32, str);
}

int RCPizza::check_pos(byte UCID)
{
    int X = players[UCID].Info.X / 65536;
    int Y = players[UCID].Info.Y / 65536;

    if (Check_Pos(4, zone.dealX, zone.dealY, X, Y))
    {
        return 1;
    }
    return 0;
}

bool RCPizza::IfWork (byte UCID)
{
    if (players[UCID].WorkType != 0)
    {
        return true;
    }

    return false;
}

void RCPizza::Event()
{
    /** ����� ������ � �������� � ������� �� ���� ������ ������ � ���� **/
    for (auto& plit: players)
    {
        if (players[plit.first].WorkAccept != 0)
        {
            int nowtime = time(&ptime);
            if (players[plit.first].WorkTime <= nowtime)
            {
                insim->SendBFN(plit.first, 212);

                if (players[plit.first].WorkAccept == 3) // �� ����� �������� ��������
                    ShopAccepted = false;

                if (players[plit.first].WorkAccept != 0)
                {
                    insim->SendMTC(plit.first, msg->_(plit.first, "4101"));
                    players[plit.first].WorkType = WK_NULL;
                    players[plit.first].WorkAccept = 0;

                    if (players[plit.first].WorkPlayerAccept != 0)
					{
                        for (auto& plit2: players)
						{
                            if (plit2.first == players[plit.first].WorkPlayerAccept)
							{
                                players[plit2.first].Pizza = 0;
								break;
							}
						}
					}

                    players[plit.first].WorkPlayerAccept = 0;
                    CarsInWork --;
                    ClearButtonInfo(plit.first);
                }
            }
        }

        if (players[plit.first].WorkAccept != 0)
        {
            btn_work(plit.first);
            ButtonInfo(plit.first, players[plit.first].WorkDest);
        }
    }

    /** ����� ����� ������� **/
    if (ginfo_time <= time(&ptime))
    {
        int pizza_time = 600 / (NumP + 1);
        ginfo_time += pizza_time;

        if ((PStore.Muka > 5) && (PStore.Voda > 5) && (PStore.Ovoshi > 5) && (PStore.Cheese > 5))
        {
            for (auto& plit: players)
            {
                if ((plit.first  !=0) and (players[plit.first].WorkType != 0) and (players[plit.first].WorkAccept == 0))
                {
                    /** ������ ������������� �� ������� ������ **/
                    for (auto& plit2: players)
                    {
                        if (plit2.first != plit.first and players[plit2.first].Pizza == 1)
                        {
                            insim->SendMTC(plit.first, msg->_(plit.first, "2201"));
                            strcpy(players[plit.first].WorkDest, msg->_(plit.first, "2202"));
                            players[plit.first].WorkAccept = 1;
                            players[plit.first].WorkPlayerAccept = plit2.first;
                            players[plit.first].WorkZone =0;
                            int worktime = time(&ptime);
                            players[plit.first].WorkTime = worktime+60 * 6;

                            players[plit2.first].Pizza = 2;
                            break; // ����� �������� ������ ������ ������
                        }
                    }
                    break;
                }
            }

            /** ����� ����� ����� **/
            for (auto& plit: players)
            {
                if ((plit.first  !=0) and (players[plit.first].WorkType == WK_PIZZA) and (players[plit.first].WorkAccept == 0))
                {
                    insim->SendMTC(plit.first, msg->_(plit.first, "2201"));
                    strcpy(players[plit.first].WorkDest, msg->_(plit.first, "2202"));

                    players[plit.first].WorkAccept =1;
                    players[plit.first].WorkPlayerAccept =0;
                    players[plit.first].WorkZone =0;
                    int worktime = time(&ptime);
                    players[plit.first].WorkTime = worktime+60 * 6;
                    break; // ����� �������� ������ ������ ������
                }
            }
        }
    }

    /** ����� ��������� � �������� **/
    if ((PStore.Muka < 10 || PStore.Voda < 10 || PStore.Ovoshi < 10 || PStore.Cheese < 10) and ShopAccepted == false)
    {
        for (auto& plit: players)
        {
            if (plit.first !=0 and players[plit.first].WorkType != 0 and players[plit.first].WorkAccept == 0)
            {
                char str[96];
                strcpy(players[plit.first].WorkDest, msg->_(plit.first, "4204"));
                sprintf(str, "^3| %s", msg->_(plit.first, "4204"));
                insim->SendMTC(plit.first, str);

                players[plit.first].WorkAccept = 3;
                players[plit.first].WorkPlayerAccept = 0;
                players[plit.first].WorkZone = 0;
                int worktime = time(&ptime);
                players[plit.first].WorkTime = worktime+60 * 6;
                ShopAccepted = true;
                break; // ����� �������� ������ ������ ������
            }
        }
    }
}
