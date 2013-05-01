using namespace std;

#include "RCPizza.h"

time_t  ptime;

pthread_t tid; // Thread ID


RCPizza::RCPizza()
{
	memset(&PStore, 0, sizeof( Store ) );
	memset(&TrackInf, 0, sizeof( pizza_info ) );
	memset(&zone, 0, sizeof( place ) );
}

RCPizza::~RCPizza()
{

}

int RCPizza::init( const char *dir, void *CInSim, void *GetMessage, void *Bank, void *Energy, void *DrLic)
{
    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        printf ("RCPizza: Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)GetMessage;
    if(!msg)
    {
        printf ("RCPizza: Can't struct RCMessage class");
        return -1;
    }
    bank = (RCBank *)Bank;
    if(!bank)
    {
        printf ("RCPizza: Can't struct RCBank class");
        return -1;
    }
    nrg = (RCEnergy *)Energy;
    if(!nrg)
    {
        printf ("RCPizza: Can't struct RCEnergy class");
        return -1;
    }

    dl = (RCDL *)DrLic;
    if(!dl)
    {
        printf ("RCPizza: Can't struct RCDL class");
        return -1;
    }

    ginfo_time = time(&ptime)+60;

    return 0;
}

void RCPizza::Deal( byte UCID )
{

    if (players[ UCID ].WorkType == WK_NULL)
    {
        if (CarsInWork <= NumCars)
        {
            players[ UCID ].WorkType = WK_PIZZA;
            players[ UCID ].WorkAccept = 0;
            send_mtc( UCID ,msg->GetMessage(  UCID , "4000" ));
            CarsInWork ++;
        }
        else
        {
            send_mtc( UCID ,"^C^3| ^7Нет вакансий");
            return;
        }
    }
    else if (players[ UCID ].WorkType == WK_PIZZA)
    {
        send_mtc( UCID ,msg->GetMessage(  UCID , "4001" ));
    }
    else
    {
        send_mtc( UCID ,msg->GetMessage(  UCID , "4002" ));
    }
}

void RCPizza::Take( byte UCID )
{
    if (players[ UCID ].WorkType != WK_PIZZA)
    {
        send_mtc( UCID ,msg->GetMessage(  UCID , "4002" ));
        return;
    }

    if (strcmp(players[ UCID ].CName,"UF1")!=0)
    {
        send_mtc( UCID , "^3^C| ^7Нужна машина ^2UF1");
        return;
    }

    if (players[ UCID ].WorkAccept == 1)
    {
        /**
        *	====	Готовим пиццу	====
        *
        *   Одна порция пиццы - 1 кг муки 0.6 кг вода, 0.9 томатов, 0.4 сыра
        *   Себестоимость за порцию = 3+ 3 +40 +83 = 132 руб
        *   Окончательная цена 320 руб
        *   оплата за изготовление 188 руб
        *   оплата за доставку 240 руб.
        **/
        PStore.Muka -= 0.25; // 3 rur
        PStore.Voda -= 0.2;    // 3
        PStore.Ovoshi -= 0.5;   //40
        PStore.Cheese -= 0.15; // 83

        if (players[ UCID ].WorkPlayerAccept == 0) // даем обычный заказ
        {
            srand(time(NULL));
            int place = rand()%zone.NumPoints;

            int worktime = time(&ptime);
            players[ UCID ].WorkTime = worktime + PIZZA_WORK_TIME;
            players[ UCID ].WorkDestinaion = place;
            players[ UCID ].WorkAccept = 2;
            send_mtc( UCID ,msg->GetMessage(  UCID , "4200" ));
            char Text[64];
            sprintf(Text, "^3| %s",zone.point[place].Place);
            send_mtc( UCID, Text);
            strcpy(players[ UCID ].WorkDest,zone.point[place].Place);

        }
        else if (players[ UCID ].WorkPlayerAccept != 0) // заказал игрок
        {
            int worktime = time(&ptime);
            players[ UCID ].WorkTime = worktime + PIZZA_WORK_TIME;
            players[ UCID ].WorkDestinaion = players[ UCID ].WorkPlayerAccept;
            players[ UCID ].WorkAccept = 2;
            char text[96];
            strcpy(text,msg->GetMessage(  UCID , "4201" ));
            strcat(text,players[ players[ UCID ].WorkPlayerAccept ].PName);

            strcpy( players[ UCID ].WorkDest, players[ players[ UCID ].WorkPlayerAccept ].PName);

            send_mtc( UCID ,text);
        }

    }
    else if (players[ UCID ].WorkAccept == 2)
    {
        send_mtc( UCID ,msg->GetMessage(  UCID , "4202" )); // Отвези сначала этот заказ
    }
    else
    {
        send_mtc( UCID ,msg->GetMessage(  UCID , "4203" )); // Wait until i call you
    }

}

void RCPizza::Done( byte UCID )
{

    if ((players[ UCID ].WorkType == WK_PIZZA) and (players[ UCID ].WorkAccept == 2))
    {
        int i;

        players[ UCID ].WorkDestinaion =0;
        players[ UCID ].WorkAccept = 0;
        players[ UCID ].WorkPlayerAccept = 0;
        int cash = 248 + 50 * abs( 1 - (players[ UCID ].WorkTime - time(&ptime) ) / PIZZA_WORK_TIME );
        bank->AddCash( UCID , cash, true); // цена за доставку 248 руб.
        Capital += 420 - 50 * abs( 1 - (players[ UCID ].WorkTime - time(&ptime) ) / PIZZA_WORK_TIME ) ;

        dl->AddSkill( UCID );

        players[ UCID ].WorkCountDone ++;
        send_mtc( UCID ,msg->GetMessage(  UCID , "4300" ));
        send_bfn( UCID ,210);
        btn_information_clear( UCID );
        players[ UCID ].WorkZone =0;
    }

}

void RCPizza::Undeal( byte UCID ,const char *Reason)
{
    if (players[ UCID ].WorkType == WK_PIZZA)
    {
        if (players[ UCID ].WorkAccept == 3)
            ShopAccepted = false;

        send_bfn( UCID ,210);
        send_bfn( UCID ,211);
        send_mtc( UCID ,msg->GetMessage(  UCID , "4100" ));
        send_mtc( UCID ,Reason);
        players[ UCID ].WorkType = WK_NULL;
        players[ UCID ].WorkDestinaion =0;
        players[ UCID ].WorkAccept = 0;
        CarsInWork --;
    }

}

void RCPizza::readconfig(const char *Track)
{

    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCPizza\\%s.txt",RootDir,Track);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("RCPizza: Can't find\n%s",file);
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);


    int point = 0;

    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);

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

            if (strncmp(str,"point",5)==0)
            {
                readf.getline(str,64);
                strncpy(zone.point[point].Place,str,strlen(str));
                readf.getline(str,128);

                char * X;
                char * Y;
                X = strtok (str,",");
                Y = strtok (NULL,",");

                zone.point[point].X = atoi(X);
                zone.point[point].Y = atoi(Y);

                point ++;
                zone.NumPoints = point;
            } // if /street


        } // if strlen > 0
    } //while readf.good()
    readf.close();

     /*** READ STORE DATA ***/

    sprintf(file,"%sdata\\RCPizza\\_Store.txt",RootDir);
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("RCPizza: Can't find _Store.txt");
        return;
    }
    FindClose(fff);

    ifstream ReadStore (file,ios::in);
    while (ReadStore.good())
    {
        char str[128];
        ReadStore.getline(str,128);

        if (strlen(str) > 0)
        {
            if (strncmp(str,"Muka=",strlen("Muka="))==0)
            {
                PStore.Muka = atof(str+strlen("Muka="));
            }

            if (strncmp(str,"Voda=",strlen("Voda="))==0)
            {
                PStore.Voda = atof(str+strlen("Voda="));
            }

            if (strncmp(str,"Ovoshi=",strlen("Ovoshi="))==0)
            {
                PStore.Ovoshi = atof(str+strlen("Ovoshi="));
            }

            if (strncmp(str,"Cheese=",strlen("Cheese="))==0)
            {
                PStore.Cheese = atof(str+strlen("Cheese="));
            }
        }
    }
    ReadStore.close();

    sprintf(file,"%sdata\\RCPizza\\_Pizza.txt",RootDir);
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("RCPizza: Can't find _Pizza.txt");
        return;
    }
    FindClose(fff);

    ifstream ReadInfo (file,ios::in);
    while (ReadInfo.good())
    {
        char str[128];
        ReadInfo.getline(str,128);

        if (strlen(str) > 0)
        {
            if (strncmp(str,"Capital=",strlen("Capital="))==0)
            {
                Capital = atoi(str+strlen("Capital="));
            }

            if (strncmp(str,"NumCars=",strlen("NumCars="))==0)
            {
                NumCars = atoi(str+strlen("NumCars="));
            }

        }
    }
    ReadInfo.close();
}


void RCPizza::insim_ncn( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
        return;

    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);

}

void RCPizza::insim_npl( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID;
	strcpy(players[ packet->UCID ].CName ,packet->CName);
	NumP ++;
}

void RCPizza::insim_plp( struct IS_PLP* packet)
{
	PLIDtoUCID.erase(  packet->PLID );
	NumP --;
}

void RCPizza::insim_pll( struct IS_PLL* packet )
{
 	PLIDtoUCID.erase(  packet->PLID );
	NumP --;
}

void RCPizza::insim_cnl( struct IS_CNL* packet )
{
	if ( players[ packet->UCID ].WorkAccept == 3 )
		ShopAccepted = false;

	if ( players[ packet->UCID ].WorkType == WK_PIZZA )
		CarsInWork --;

	players.erase( packet->UCID );
    /** Save Pizza Info **/

    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCPizza\\_Store.txt",RootDir);
    ofstream writef (file,ios::out);
    writef << "Muka=" << PStore.Muka << endl;
    writef << "Voda=" << PStore.Voda << endl;
    writef << "Ovoshi=" << PStore.Ovoshi << endl;
    writef << "Cheese=" << PStore.Cheese << endl;
    writef.close();

    sprintf(file,"%sdata\\RCPizza\\_Pizza.txt",RootDir);
    ofstream WriteInfo (file,ios::out);
    WriteInfo << "Capital=" << Capital << endl;
    WriteInfo << "NumCars=" << NumCars << endl;
    WriteInfo.close();

}

void RCPizza::insim_cpr( struct IS_CPR* packet )
{
	strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCPizza::insim_mci ( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
    	byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];

		int X = pack_mci->Info[i].X/65536;
		int Y = pack_mci->Info[i].Y/65536;
		int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

		// проверка если юзер попал в зону доставки и скорость равна нулю
		if ( check_pos( UCID ) == 1 )
		{
			if (players[ UCID ].Zone != 4 )
			{
				players[ UCID ].Zone = 4;

				send_mtc( UCID ,msg->GetMessage(  UCID , "1600" )); // pizza u Jony
				if (players[ UCID ].WorkType != WK_PIZZA)
					send_mtc( UCID ,msg->GetMessage(  UCID , "1601" )); // deal
				else
					send_mtc( UCID ,msg->GetMessage(  UCID , "1602" )); // undeal
				if (players[ UCID ].WorkAccept != 0)
				{
					send_mtc( UCID ,msg->GetMessage(  UCID , "1603" )); // take
					Take( UCID );
				}

			}
		}

		else
		{
			players[ UCID ].Zone = 0;
		}
		/** Zones (PitSave, shop, etc) **/

		if ((Distance(X,Y,zone.point[players[ UCID ].WorkDestinaion].X,zone.point[players[ UCID ].WorkDestinaion].Y) < 10) and (S <5))
			Done( UCID );

		byte PL_UCID = players[ UCID ].WorkPlayerAccept;

		if ( PL_UCID != 0 and (players[ UCID ].WorkAccept == 2))
		{
			int PL_X = players[ PL_UCID ].Info.X/65536;
			int PL_Y = players[ PL_UCID ].Info.Y/65536;

			if (Distance(X, Y, PL_X, PL_Y) < 10) // если растояние до игрока меньше 10 метров
			{
				Done( UCID );
				bank->RemCash(PL_UCID ,800);
				nrg->AddEnergy( PL_UCID , 8000);
				send_mtc( PL_UCID ,msg->GetMessage(  PL_UCID , "1604" ) );
				players[ PL_UCID ].Pizza = 0;
			}


		}

		if ((players[ UCID ].WorkAccept ==3) && (Check_Pos(TrackInf.ShopCount,TrackInf.XShop,TrackInf.YShop,X,Y))) // если игроку сказали заказать продукты и он приехал в магазин
		{
			players[ UCID ].WorkDestinaion =0;
			players[ UCID ].WorkAccept = 0;
			players[ UCID ].WorkPlayerAccept = 0;
			send_bfn( UCID ,210);
			send_bfn( UCID ,211);
			send_mtc( UCID ,"^C^3| ^7Спасибо за заказ продуктов.");

			// проверяем склад на продукты

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

		memcpy(&players[ UCID ].Info, &pack_mci->Info[i], sizeof(struct CompCar) );
    }
}

void RCPizza::insim_mso( struct IS_MSO* packet )
{

    if (packet->UCID == 0)
        return;

    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!pstat", 6) == 0 )
    {
        //cout << players[i].UName << " send !deal" << endl;
        char Text[64];
        sprintf(Text,"^C^3| ^7Капитал Пиццерии составляет %d Руб.",Capital);
        send_mtc( packet->UCID ,Text);
        sprintf(Text,"^C^3| ^7Продукты на складе.");
        send_mtc( packet->UCID ,Text);
        sprintf(Text,"^C^3| ^7Мука: %1.2f Кг.",PStore.Muka);
        send_mtc( packet->UCID ,Text);
        sprintf(Text,"^C^3| ^7Вода: %1.2f Кг.",PStore.Voda);
        send_mtc( packet->UCID ,Text);
        sprintf(Text,"^C^3| ^7Овощи: %1.2f Кг.",PStore.Ovoshi);
        send_mtc( packet->UCID ,Text);
        sprintf(Text,"^C^3| ^7Сыр: %1.2f Кг.",PStore.Cheese);
        send_mtc( packet->UCID ,Text);
        sprintf(Text,"^C^3| ^7Работники: %d/%d.",CarsInWork,NumCars);
        send_mtc( packet->UCID ,Text);


        for ( auto& plit: players )
        {
            if (players[ plit.first ].WorkType == WK_PIZZA)
            {
                sprintf(Text,"%s Accept = %d Count = %d",players[  plit.first ].PName, players[  plit.first ].WorkAccept, players[  plit.first ].WorkCountDone);
                send_mtc( packet->UCID ,Text);
            }

        }

    }

    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!deal", 5) == 0 )
    {

        if ( ( check_pos( packet->UCID ) == 1) and (players[ packet->UCID ].WorkType == 0))
        {
            if (dl->GetLVL( packet->UCID ) > 19)
            {
                send_mtc( packet->UCID ,"^6| ^C^7Большим дядькам большую работу. Нужен уровень ниже 20.");
                return;
            }
            if (strcmp(players[ packet->UCID ].CName,"UF1") == 0 )
            {
                Deal( packet->UCID );
            }
            else
            {
                send_mtc( packet->UCID , "^3^C| ^7Нужна машина ^2UF1");
            }
        }


    }

    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!undeal", 7) == 0 )
    {
        if ((check_pos( packet->UCID ) == 1) and (players[ packet->UCID ].WorkType == WK_PIZZA))
            Undeal( packet->UCID ,msg->GetMessage(  packet->UCID , "2200" ));

    }

    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!take", 5) == 0)
    {

        if (check_pos( packet->UCID ) == 1)
            Take( packet->UCID );
    }

    //!pizza
    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!tofu", 5) == 0)
    {
        if (nrg->GetEnergy( packet->UCID ) > 80)
        {
            send_mtc( packet->UCID ,"Your power must be less than 80%"); // заказ принят
            return;
        }

        if (players[ packet->UCID ].WorkType == WK_PIZZA)
        {
            send_mtc( packet->UCID ,"You can't order a tofu because you work here."); // заказ принят
            return;
        }

        if (players[ packet->UCID ].Pizza == 0)
        {
            players[ packet->UCID ].Pizza = 1;
            send_mtc( packet->UCID ,msg->GetMessage(  packet->UCID , "2204" )); // заказ принят
        }

    }

}

void RCPizza::btn_work ( byte UCID )
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID =  UCID ;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 210;
    pack.BStyle = 32;
    pack.L = 136;
    pack.T = 1;
    pack.W = 10;
    pack.H = 8;

    char min_c[3];
    char sec_c[3];

    int time2 = players[ UCID ].WorkTime - time(&ptime);
    int min = time2/60;
    int sec = time2%60;

    sprintf(pack.Text,"^2%02d:%02d",min, sec);

    insim->send_packet(&pack);
}

// функции-утилиты

int RCPizza::check_pos( byte UCID )
{
    int PLX = players[ UCID ].Info.X/65536;
    int PLY = players[ UCID ].Info.Y/65536;

    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
        return 1;

    return 0;
}

bool RCPizza::IfWork (byte UCID)
{
	if (players[ UCID ].WorkType != 0)
		return true;

    return false;
}

void RCPizza::Event()
{

    /** вывод кнопки с часиками и скрытие ее если таймер пришел в ноль **/

	for ( auto& plit: players )
	{

		if (players[ plit.first ].WorkAccept != 0)
		{
			//cout << "player[ plit.first ].UCID = " << (int) plit.first  <<endl;
			int nowtime = time(&ptime);
			if (players[ plit.first ].WorkTime <= nowtime)
			{
				send_bfn( plit.first ,210);
				send_bfn( plit.first ,211);

				if (players[ plit.first ].WorkAccept == 3) // не успел заказать продукты
					ShopAccepted = false;

				if (players[ plit.first ].WorkAccept != 0)
				{
					send_mtc( plit.first ,msg->GetMessage(  plit.first , "4101" ));
					players[ plit.first ].WorkType = WK_NULL;
					players[ plit.first ].WorkAccept = 0;
					if ( players[ plit.first ].WorkPlayerAccept != 0)
					{
						for ( auto& plit2: players )
						{
							if ( plit2.first == players[ plit.first ].WorkPlayerAccept)
								players[ plit2.first ].Pizza = 0;
							break;
						}
					}
					players[ plit.first ].WorkPlayerAccept = 0;
					CarsInWork --;
				}
			}
		}

		if (players[ plit.first ].WorkAccept != 0)
		{
			btn_work( plit.first );
			btn_information( plit.first ,players[ plit.first ].WorkDest);
		}
	}
	/** конец цикла вывода кнопки с часиками и скрытие ее если таймер пришел в ноль **/

	/** тут заказ пиццы игроком **/

	if ( ginfo_time <= time(&ptime))
	{
		//srand(time(&ptime));
		//int r = rand()%3 + 1;
		int pizza_time = 600/(NumP+1);
		ginfo_time += pizza_time;

		if ((PStore.Muka > 5) && (PStore.Voda > 5) && (PStore.Ovoshi > 5) && (PStore.Cheese > 5))
		{
			for ( auto& plit: players )
			{
				if ( ( plit.first  !=0) and (players[ plit.first ].WorkType != 0) and (players[ plit.first ].WorkAccept == 0))
				{
					/** прогон пользователей на предмет заказа **/

					for ( auto& plit2: players )
					{
						if (  ( plit2.first  !=  plit.first ) and ( players[ plit2.first ].Pizza == 1 ) )
						{
							send_mtc( plit.first , msg->GetMessage(  plit.first , "2201" ));
							send_mtc( plit.first , msg->GetMessage(  plit.first , "2202" ));
							players[ plit.first ].WorkAccept = 1;
							players[ plit.first ].WorkPlayerAccept = plit2.first;
							players[ plit.first ].WorkZone =0;
							int worktime = time(&ptime);
							players[ plit.first ].WorkTime = worktime+60*6;

							players[ plit2.first ].Pizza = 2;

							break; // чтобы оповещал только одного игрока
						}
					}
					break;
				}
			}

			/** тут заказ пиццы ботом **/

			for ( auto& plit: players )
			{
				if ( ( plit.first  !=0) and (players[ plit.first ].WorkType == WK_PIZZA) and (players[ plit.first ].WorkAccept == 0))
				{
					send_mtc( plit.first ,msg->GetMessage(  plit.first , "2201" ));
					send_mtc( plit.first ,msg->GetMessage(  plit.first , "2202" ));

					strcpy(players[ plit.first ].WorkDest,msg->GetMessage(  plit.first , "2201" )+4); // +4 == remove slash

					players[ plit.first ].WorkAccept =1;
					players[ plit.first ].WorkPlayerAccept =0;
					players[ plit.first ].WorkZone =0;
					int worktime = time(&ptime);
					players[ plit.first ].WorkTime = worktime+60*6;
					break; // чтобы оповещал только одного игрока
				}
			}
		}
	}

	/*** проверка склада на наличие продуктов, и если не хватает то просить работника зказать продукты ***/

	if ((PStore.Muka < 10) || (PStore.Voda < 10) || (PStore.Ovoshi < 10) || (PStore.Cheese < 10))
	{
		if  (ShopAccepted == false)
		{
			for ( auto& plit: players )
			{
				if ( ( plit.first  !=0)
						and (players[ plit.first ].WorkType != 0)
						and (players[ plit.first ].WorkAccept == 0))
				{
					cout << players[ plit.first ].UName << " accepted for shop\n";

					send_mtc( plit.first ,"^C^3| ^7Закажите продукты в магазине");
					strcpy(players[ plit.first ].WorkDest,"^C^7Закажите продукты в магазине"); // +4 == remove slash

					players[ plit.first ].WorkAccept =3;
					players[ plit.first ].WorkPlayerAccept =0;
					players[ plit.first ].WorkZone =0;
					int worktime = time(&ptime);
					players[ plit.first ].WorkTime = worktime+60*6;
					ShopAccepted = true;
					break; // чтобы оповещал только одного игрока
				}
			}
		}
	}


}
