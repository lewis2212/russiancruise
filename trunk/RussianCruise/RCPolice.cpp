#include "RCPolice.h"
RCPolice::RCPolice()
{
    memset( &fines, 0, sizeof( fine ) * MAX_FINES );
}

RCPolice::~RCPolice()
{

}

int RCPolice::init(const char *dir, void *CInSim, void *Message, void *Bank, void *RCdl, void * STreet, void *Energy, void *Light)
{
    strcpy(RootDir, dir);
    insim = (CInsim *)CInSim;
    if (!insim)
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
    bank = (RCBank *)Bank;
    if (!bank)
    {
        printf ("Can't struct RCBank class");
        return -1;
    }
    dl = (RCDL *)RCdl;
    if (!dl)
    {
        printf ("Can't struct RCDL class");
        return -1;
    }
    street = (RCStreet *)STreet;
    if (!street)
    {
        printf ("Can't struct RCStreet class");
        return -1;
    }
    nrg = (RCEnergy *)Energy;
    if (!nrg)
    {
        printf ("Can't struct RCEnergy class");
        return -1;
    }
    lgh = (RCLight *)Light;
    if (!lgh)
    {
        printf ("Can't struct RCLight class");
        return -1;
    }
    return 0;
}

void RCPolice::InsimNCN( struct IS_NCN* packet )
{
    if ( packet->UCID == 0 )
        return;

    strcpy(players[packet->UCID].UName, packet->UName);
    strcpy(players[packet->UCID].PName, packet->PName);

    //читаем штрафы
    ReadUserFines(packet->UCID);
}

void RCPolice::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[packet->PLID] = packet->UCID;
    strcpy(players[packet->UCID].CName , packet->CName);

    for ( auto& p: ArestPlayers )
    {
        if (strcmp(ArestPlayers[p.first].UName, players[packet->UCID].UName) == 0)
        {
            time_t now = time(NULL);
            if (now <= ArestPlayers[p.first].ArestTime)
            {
                char str[96];
                sprintf(str, "/spec %s", ArestPlayers[p.first].UName);
                SendMST(str);

                sprintf(str, "^2| ^7^CВы арестованы. До конца ареста осталось ^1%d ^7минут", (int)((ArestPlayers[p.first].ArestTime-now)/60+1));
                SendMTC(packet->UCID, str);
            }
            break;
        }
    }

    if ((strncmp("^4[^C^7ДПС^4]", players[packet->UCID].PName, 13)==0 || strncmp("^4[^C^7ГАИ^4]", players[packet->UCID].PName, 13)==0) and !players[packet->UCID].cop)
    {
        char file[255];
        sprintf(file, "%sdata\\RCPolice\\CopList.txt", RootDir);
        char line[32];
        ifstream rCops( file , ios::in );

        players[packet->UCID].Rank = 0;
        while ( rCops.good() )
        {
            rCops.getline(line, 32);

            if (strcmp(strtok (line, ", "), players[packet->UCID].UName))
            {
                continue;
            }

            players[packet->UCID].Rank = atoi(strtok (NULL, ", "));
            break;
        }
        rCops.close();

        if (players[packet->UCID].Rank > 0)
        {
            players[packet->UCID].cop = true;
            SendMTC(packet->UCID, "^2| ^7^CВы заступили на смену");

            if (!((strncmp("DPS", packet->SName, 3)==0)
                    || (strncmp("dps", packet->SName, 3)==0)
                    || (strncmp("POLICE", packet->SName, 6)==0)
                    || (strncmp("police", packet->SName, 6)==0)))
            {
                SendMTC(packet->UCID, "^2| ^C^7В названии скина не найдено указателей на принадлежность к ДПС");
            }

            players[packet->UCID].StartWork = time(NULL)-1;
        }
        else
        {
            SendMTC(packet->UCID, msg->_(packet->UCID , "CopFalse"));

            char text[96];
            sprintf(text, "/spec %s", players[packet->UCID].UName);
            SendMST(text);
        }
    }

    if (players[packet->UCID].cop)
    {
        dl->Lock(packet->UCID);
        nrg->Lock(packet->UCID);
        lgh->SetLight3(packet->UCID, true);
    }
}

void RCPolice::InsimPLP( struct IS_PLP* packet )
{
    byte UCID = PLIDtoUCID[packet->PLID];

    if (players[UCID].Sirena)
    {
        players[UCID].Sirena = false;
    }

    lgh->SetLight3(UCID, false);
    dl->Unlock(UCID);
    nrg->Unlock(UCID);
    PLIDtoUCID.erase(packet->PLID);
}

void RCPolice::InsimPLL( struct IS_PLL* packet )
{
    byte UCID = PLIDtoUCID[packet->PLID];

    if (players[UCID].Sirena)
    {
        players[UCID].Sirena = false;
    }

    lgh->SetLight3(UCID, false);
    dl->Unlock(UCID);
    nrg->Unlock(UCID);

    PLIDtoUCID.erase(packet->PLID);
}

void RCPolice::InsimCNL( struct IS_CNL* packet )
{
    for (int i = 0; i < 32; i++)
    {
        if (DTPvyzov[0][i] > 0)
        {
            if (DTPvyzov[2][i] == packet->UCID)
            {
                DTPvyzov[1][i] = time(NULL) + 2*60;
                DTPvyzov[2][i] = 0;
                break;
            }

            if (DTPvyzov[0][i] == packet->UCID)
            {
                players[DTPvyzov[0][i]].DTP = 0;

                if (DTPvyzov[2][i] != 0)
                {
                    players[DTPvyzov[2][i]].DTPstatus = 0;
                }

                DTPvyzov[0][i] = 0;
                DTPvyzov[1][i] = 0;
                DTPvyzov[2][i] = 0;
                SendBFN(255, 135+i);
                break;
            }
        }
    }

    CopPayRoll(packet->UCID, false);

    SaveUserFines(packet->UCID);
    players.erase(packet->UCID);
}

void RCPolice::InsimCPR( struct IS_CPR* packet )
{
    strcpy(players[packet->UCID].PName, packet->PName);

    if (players[packet->UCID].cop and !(strncmp("^4[^C^7ДПС^4]", players[packet->UCID].PName, 13)==0 || strncmp("^4[^C^7ГАИ^4]", players[packet->UCID].PName, 13)==0))
    {
        SendMTC(packet->UCID, "^2| ^7^CВы больше не инспектор ДПС");
        CopPayRoll(packet->UCID, false);
        SendBFN(packet->UCID, 70);
        players[packet->UCID].cop = false;
        players[packet->UCID].StartWork = 0;
        players[packet->UCID].Rank = 0;

        for (int i=214; i<=222; i++)
        {
            SendBFN(packet->UCID, i);
        }
    }
}

void RCPolice::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
    {
        return;
    }

    if ( packet->UserType != MSO_PREFIX )
    {
        return;
    }

    byte UCID = packet->UCID;

    char Msg[128];
    strcpy(Msg, packet->Msg + ((unsigned char)packet->TextStart));

    if (!strcmp(Msg, "!911") or !strcmp(Msg, "!dtp") or !strcmp(Msg, "!^Cдтп"))
    {
        if (players[UCID].cop)
        {
            return;
        }

        if (GetCopCount() <= 0)
        {
            SendMTC(UCID, "^2| ^7^CНет свободных ДПС");
            return;
        }

        if (players[UCID].Pogonya > 0)
        {
            SendMTC(UCID, "^2| ^1^CВы не можете вызвать ДПС во время погони");
            return;
        }

        if (players[UCID].DTP != 0)
        {
            SendMTC(UCID, "^2| ^1^CВы уже вызвали ДПС, ожидайте");
            return;
        }

        if (players[UCID].Info.Speed*360/32768 > 0)
        {
            SendMTC(UCID, "^2| ^1^CВы не можете вызвать ДПС во время движения");
            return;
        }

        players[UCID].DTP = 255;

        for (int i = 0; i < 32; i++)
        {
            if (DTPvyzov[0][i] <= 0)
            {
                DTPvyzov[0][i] = (int)UCID; 		//кто вызвал
                DTPvyzov[1][i] = time(NULL) + 5*60;	//когда вызвал

                SendMTC(UCID, "^2| ^C^7Ваша заявка будет рассмотрена в течение 5 минут");
                SendMTC(UCID, "^2| ^C^7Не покидайте место ДТП до прибытия инспектора ДПС");

                char str[96];
                sprintf(str,"^2| ^1^CПоступил вызов на ДТП от %s", players[UCID].PName);
                SendMTCToCop(str, 1, 2, 4);
                return;
            }
        }

        SendMTC(UCID, "^2| ^C^7Очередь заявок переполнена");
    }

    if (!strcmp(Msg, "!fines") or !strcmp(Msg, "!^Cштрафы"))
    {
        int j=0;
        for (int i=0; i<MAX_FINES; i++)
        {
            if (players[ packet->UCID ].fines[i].fine_id > 0 and players[ packet->UCID ].fines[i].fine_id < MAX_FINES )
            {
                char Text[128], Date[128];
                int fine_id = players[ packet->UCID ].fines[i].fine_id;

                sprintf(Text, "^2| ^7ID = %d. %.64s (^2%d RUR^7)", fine_id , GetFineName(packet->UCID, fine_id) , fines[fine_id].cash );

                if (strlen(players[packet->UCID].fines[i].CopPName.c_str()) > 0)
                {
                    strcat(Text, " - ");
                    strcat(Text, players[packet->UCID].fines[i].CopPName.c_str());
                }
                else if (strlen(players[packet->UCID].fines[i].CopName.c_str()) > 0)
                {
                    strcat(Text, " - ");
                    strcat(Text, players[packet->UCID].fines[i].CopName.c_str());
                }

                struct tm * timeinfo;
                time_t t = (time_t)players[packet->UCID].fines[i].fine_date;
                timeinfo = localtime (&t);
                strftime (Date, 128, ", ^2%d.%m.%y", timeinfo);
                strcat(Text, Date);
                SendMTC( packet->UCID , Text);
                j++;
            }
        }

        if (j == 0)
        {
            SendMTC(packet->UCID , msg->_(  packet->UCID , "NoFines" ));
        }
    }

    if (strncmp(Msg, "!pay", strlen("!pay")) == 0 or strncmp(Msg, "!^Cоплатить", strlen("!^Cоплатить")) == 0)
    {
        char _2[128];
        strcpy(_2, Msg);

        char * comand;
        char * id;

        comand = strtok (_2, " ");
        id = strtok (NULL, " ");

        int id_i = atoi(id);

        if ((!id) or (id_i < 1))
        {
            SendMTC( packet->UCID , msg->_(  packet->UCID , "2105" ));
            return;
        }

        if (bank->InBank( packet->UCID ))
        {
            if (fines[id_i].cash<bank->GetCash( packet->UCID ))
            {
                int j=0;
                for (j=0; j<MAX_FINES; j++)
                {
                    if (players[ packet->UCID ].fines[j].fine_id == id_i)
                    {
                        players[ packet->UCID ].fines[j].fine_id = 0;
                        players[ packet->UCID ].fines[j].fine_date = 0;

                        int cop = 0;
                        for ( auto& play: players)
                        {
                            if ( players[ play.first ].cop and packet->UCID != play.first)
                            {
                                if (dl->Islocked(  play.first  ))
                                {
                                    dl->Unlock( play.first );
                                    dl->AddSkill( play.first , 0.05);
                                    dl->Lock( play.first );
                                }
                                else
                                {
                                    dl->AddSkill( play.first , 0.05);
                                }

                                bank->AddCash( play.first , (fines[id_i].cash)*0.05, true);
                                cop++;
                            }
                        }

                        SendMTC( packet->UCID , msg->_(  packet->UCID , "PayDone" ));
                        bank->RemCash( packet->UCID , fines[id_i].cash);
                        bank->AddToBank((fines[id_i].cash)-((fines[id_i].cash)*0.05)*cop);

                        if (dl->Islocked( packet->UCID ))
                        {
                            dl->Unlock( packet->UCID );
                            dl->RemSkill( packet->UCID );
                            dl->Lock( packet->UCID );
                        }
                        else
                        {
                            dl->RemSkill( packet->UCID );
                        }

                        break;
                    }

                    if (j == MAX_FINES)
                    {
                        SendMTC( packet->UCID, msg->_(  packet->UCID , "NoFine" ));
                    }
                }
            }
            else
            {
                SendMTC(packet->UCID, msg->_(packet->UCID, "NoManyPay"));
            }
        }
        else
        {
            SendMTC(packet->UCID, msg->_(packet->UCID, "NoOnBank"));
        }
    }

    if ((strncmp(Msg, "!sirena", 7) == 0 ) or (strncmp(Msg, "!^Cсирена", 9) == 0 ))
    {
        if (players[packet->UCID].cop)
        {
            if (players[packet->UCID].Sirena)
            {
                SendMTC(packet->UCID, msg->_(packet->UCID, "2100"));
                players[packet->UCID].Sirena = false;
            }
            else
            {
                SendMTC(packet->UCID, msg->_(packet->UCID, "2101"));
                players[packet->UCID].Sirena = true;
            }
        }
    }

    if ((strncmp(Msg, "!radar", 6) == 0 ) or (strncmp(Msg, "!^Cрадар", 8) == 0 ))
    {
        if (players[packet->UCID].cop)
        {
            if (players[packet->UCID].Rank == 1 || players[packet->UCID].Rank == 3)
            {
                SendMTC(UCID, "^2| ^C^7Вы не можете использовать радар");
                return;
            }

            if (players[UCID].DTPstatus <= 0)
            {
                if (players[packet->UCID].Radar)
                {
                    SendMTC(packet->UCID, msg->_(packet->UCID, "RadarOff"));
                    players[packet->UCID].Radar = false;
                }
                else
                {
                    SendMTC(packet->UCID, msg->_(packet->UCID, "RadarOn"));
                    players[packet->UCID].Radar = true;
                }

            }
            else SendMTC(UCID, "^2| ^C^7Вы не можете использовать радар во время разбора ДТП");
        }
    }

    if (strncmp(Msg, "!kick", 4) == 0 )
    {
        if (players[packet->UCID].Rank < 3)
        {
            return;
        }

        char user[16];
        strcpy(user, Msg+5);

        if (strlen(user)>0)
        {
            if (players[ packet->UCID ].cop )
            {
                char Kick[64];
                sprintf(Kick, "/kick %s", user);
                SendMST(Kick);

                SYSTEMTIME sm;
                GetLocalTime(&sm);
                char log[MAX_PATH];
                sprintf(log, "%slogs\\cop\\kick(%d.%d.%d).txt", RootDir, sm.wYear, sm.wMonth, sm.wDay);
                ofstream readf (log, ios::app);
                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  players[ packet->UCID ].UName << " kick " << user << endl;
                readf.close();
            }
        }
    }

    if ( strcmp( players[ packet->UCID ].UName , "denis-takumi") == 0 or strcmp( players[ packet->UCID ].UName , "Lexanom") == 0 )
    {
        char file[255];
        sprintf(file, "%sdata\\RCPolice\\cops.txt", RootDir);

        char line[32];
        if (strncmp(Msg, "!cop_add", 8) == 0 )
        {
            char param[16];
            strcpy(param, Msg+9);

            if (strlen(param)>0)
            {
                ofstream wCops( file , ios::app );
                wCops << param <<  endl;
                wCops.close();
            }
        }

        if (strncmp(Msg, "!cop_del", 8) == 0 )
        {
            char param[16];
            strcpy(param, Msg+9);
            printf("%s\n", param);
            if (strlen(param)>0)
            {
                vector<string>cops;

                ifstream rCops( file , ios::in );

                while ( rCops.good() )
                {
                    memset(&line, 0, 32);
                    rCops.getline(line, 32);

                    if (strlen(line) >0)
                    {
                        cops.push_back(line);
                    }
                }
                rCops.close();

                ofstream wCops( file , ios::out );

                for ( int i = 0; i < cops.size(); i++)
                {
                    if ( cops[i] != param )
                        wCops << cops[i].c_str() << endl;

                }
                wCops.close();

            }
        }
        if (strncmp(Msg, "!cop_list", 9) == 0 )
        {
            ifstream rCops( file , ios::in );

            while ( rCops.good() )
            {

                memset(&line, 0, 32);
                rCops.getline(line, 32);
                if (strlen(line) >0)
                {
                    SendMTC(packet->UCID, line);
                }
            }
            rCops.close();
        }
    }
}

void RCPolice::CopPayRoll(byte UCID, bool FullWork = true)
{
    if (!players[UCID].cop)
    {
		return;
    }

    int TM = (time(NULL) - players[UCID].StartWork)%1800;

    if (players[UCID].DoneCount == 0)
    {
		return;
    }

    if (FullWork and TM != 0)
    {
		return;
    }

    if (TM < 30 and !FullWork)
    {
		return;
    }

    if (players[UCID].DoneCount>10)
        players[UCID].DoneCount = 10;

    int Cash = players[UCID].DoneCount*500;

    char str[96];

    if (FullWork)
	{
        sprintf(str, "^2| ^7^CВаш заработок за прошедшую смену: %d ^3RUR", Cash);
    }
    else
    {
        Cash = Cash/2;
        players[UCID].DoneCount = players[UCID].DoneCount/2;
        sprintf(str, "^2| ^7^CВы не дождались конца смены, заработок: %d ^3RUR", Cash);
    }

    SendMTC(UCID, str);

    bank->RemFrBank(Cash);
    bank->AddCash(UCID, Cash, false);

    //прибавка скилла
    if (dl->Islocked(UCID))
    {
        dl->Unlock(UCID);
        dl->AddSkill(UCID, players[UCID].DoneCount);
        dl->Lock(UCID);
    }
    else
	{
		dl->AddSkill(UCID, players[UCID].DoneCount);
	}

    players[UCID].DoneCount = 0;
}

void RCPolice::ShowFinesPanel( byte UCID, byte UCID2 )
{
    char Text[128];
    byte c = GetFineCount();

    byte
    id=84, 				//стартовый ид кнопок
    id2=110,
    l=100, 				//не менять
    t=90,				//не менять
    hButton=5, 			//высота строки
    w=100, 				//ширина поля
    h=10+c*hButton; 	//высота поля

    sprintf(Text, "^C^7Панель выписки штрафов (^8%s^7)", players[UCID2].PName);

    SendButton(255, UCID, 81, l-w/2, t-h/2, w, h+8, 32, ""); 				//фон
    SendButton(255, UCID, 82, l-w/2, t-h/2, w, h+8, 32, "");				//фон
    SendButton(255, UCID, 83, l-w/2, t-h/2, w, 10, 64, Text); 				//заголовок
    SendButton(254, UCID, 80, l-7, t-h/2+h+1, 14, 6, 16+ISB_CLICK, "^2OK"); //закрывашка

    if (strcmp(players[UCID].UName, "Lexanom") == 0 or (players[UCID].Rank > 2 and players[UCID2].Pogonya == 2))
    {
        SendButton(UCID2,UCID,79,l+w/2-15,t-h/2+h+1,14,6,16+8+5, "^CАрестовать", 2 );
    }

    for (int i = 0; i < c; i++)
    {
        byte fid = fines[i+1].id;
        if (fid==0)
        {
			fid = 255;
        }

    	int st = 7;
        for (int i=0; i<MAX_FINES; i++)
		{
			if (FineAllow[players[UCID].Rank][i] == fid)
			{
				st = 8+3;
				break;
			}
		}

        sprintf(Text, "^2%02d. ^8%s", fid, GetFineName(UCID, i+1));

        SendButton(fid, UCID, id++, l-w/2+1, t-h/2+10+hButton*i, w-12, hButton, 16+64+st, Text);
        SendButton(fid, UCID, id2++, l+w/2-11, t-h/2+10+hButton*i, 10, hButton, 16+st, "^CОтмена");
    }
}

void RCPolice::InsimBTC( struct IS_BTC* packet )
{
    if (packet->ReqI == 200)
    {
        int i = packet->ClickID-135;
        char str[96];

        if (players[packet->UCID].DTPstatus == 2 and DTPvyzov[2][i] == packet->UCID)
        {
            sprintf(str,"^2| %s ^7^Cзакрыл вашу заявку на рассмотрение ДТП", players[packet->UCID].PName);
            SendMTC(DTPvyzov[0][i], str);

            sprintf(str,"^2| ^7^CЗаявка ^8%s ^7^Cна рассмотрение ДТП закрыта", players[DTPvyzov[0][i]].PName);
            SendMTC(packet->UCID, str);

            players[DTPvyzov[0][i]].DTP = 0;
            if (DTPvyzov[2][i] != 0)
                players[DTPvyzov[2][i]].DTPstatus = 0;

            DTPvyzov[0][i] = 0;
            DTPvyzov[1][i] = 0;
            DTPvyzov[2][i] = 0;
            SendBFN(255, packet->ClickID);

            if (players[packet->UCID].cop and players[packet->UCID].DTPfines>0)
            {
                players[packet->UCID].DoneCount++;
            }
        }
        else if (DTPvyzov[2][i] <= 0)
        {
            if (players[packet->UCID].DTPstatus>0)
            {
                SendMTC(packet->UCID, "^2| ^C^7Нельзя принимать несколько заявок одновременно");
                return;
            }

            DTPvyzov[2][i] = packet->UCID;
            DTPvyzov[1][i] = 0;

            sprintf(str,"^2| %s ^7^Cпринял вашу заявку на рассмотрение ДТП", players[packet->UCID].PName);
            SendMTC(DTPvyzov[0][i], str);

            sprintf(str,"^2| %s ^7^Cпринял заявку ^8%s ^7^Cна рассмотрение ДТП", players[packet->UCID].PName, players[DTPvyzov[0][i]].PName);
            SendMTCToCop(str, 1, 2, 4);

            players[DTPvyzov[2][i]].DTPstatus = 1;
            players[DTPvyzov[2][i]].DTPfines = 0;
            players[DTPvyzov[0][i]].DTP = packet->UCID;
        }
    }

    if ( packet->ClickID == 130 and packet->ReqI == 254 )
    {
        players[packet->UCID].ThisFineCount = 0;

        for (int i=128; i < 165; i++)
        {
            SendBFN(packet->UCID, i);
        }

        for (int j=0; j < 20; j++)
        {
            strcpy(players[packet->UCID].ThisFine[j], "");
        }
    }

    if ( packet->ClickID == 80 and packet->ReqI == 254 ) //погоня офф
    {
        for(int i=79; i<165; i++)
        {
            SendBFN(packet->UCID,i);
        }
    }

    if ( packet->ClickID >= 84 and packet->ClickID < 110 and packet->ReqI != 254 and packet->ReqI != 255) //выписка штрафа
    {
        if (players[packet->UCID].DTPstatus == 2)
        {
            players[packet->UCID].DTPfines++;
        }

        SYSTEMTIME sm;
        GetLocalTime(&sm);
        char fine_c[255];
        sprintf(fine_c, "%slogs\\fines\\fine(%d.%d.%d).txt", RootDir, sm.wYear, sm.wMonth, sm.wDay);

        for ( auto& play: players )
        {
            if  ( players[packet->UCID].BID2 == players[play.first].BID)
            {
                char Msg[128];
                sprintf(Msg, msg->_(play.first, "GiveFine"), players[packet->UCID].PName, GetFineName(play.first, packet->ReqI));
                SendMTC( play.first, Msg);

                sprintf(Msg, msg->_(  packet->UCID , "AddFine" ), players[play.first].PName, GetFineName(packet->UCID, packet->ReqI));

                SendMTC( packet->UCID , Msg);

                for (int j=0; j<MAX_FINES; j++)
                {
                    if (players[ play.first ].fines[j].fine_id == 0)
                    {
                        players[ play.first ].fines[j].fine_id = packet->ReqI;
                        players[ play.first ].fines[j].fine_date = int( time( NULL ) );
                        players[ play.first ].fines[j].CopName = players[ packet->UCID ].UName;
                        players[ play.first ].fines[j].CopPName = players[ packet->UCID ].PName;
                        break;
                    }
                }
                ofstream readf (fine_c, ios::app);
                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[ packet->UCID ].UName << " get fine ID = " << packet->ReqI << " to "  << players[ play.first ].UName << endl;
                readf.close();

                if (players[play.first].ThisFineCount==0)
                {
                    SendBFNAll(play.first);
                }

                for (int j=0; j<20; j++)
                {
                    if ( strlen(players[play.first].ThisFine[j]) == 0 )
                    {
                        sprintf(players[play.first].ThisFine[j], "^7%d. %s (^2ID = %d^7)   -   %s", j+1, GetFineName(play.first, packet->ReqI), fines[packet->ReqI].id, players[ packet->UCID ].PName);
                        players[play.first].ThisFineCount++;
                        break;
                    }
                }
                break;
            }
        }
    }

    if ( packet->ClickID >= 110 and packet->ClickID < 130 and packet->ReqI != 255 and packet->ReqI != 254) //выписка штрафа
    {
        if (players[packet->UCID].DTPstatus == 2)
        {
            players[packet->UCID].DTPfines--;
        }

        SYSTEMTIME sm;
        GetLocalTime(&sm);
        char fine_c[255];
        sprintf(fine_c, "%slogs\\fines\\fine(%d.%d.%d).txt", RootDir, sm.wYear, sm.wMonth, sm.wDay);

        for ( auto& play: players )
        {
            if  (players[ packet->UCID ].BID2 == players[ play.first ].BID)
            {
                for (int j=0; j<MAX_FINES; j++)
                {
                    if ( players[ play.first ].fines[j].fine_id == packet->ReqI )
                    {
                        char Msg[128];
                        sprintf(Msg, msg->_( play.first, "DeletedFine"), players[packet->UCID].PName, GetFineName(play.first, packet->ReqI));

                        SendMTC( play.first , Msg);

                        sprintf(Msg, msg->_(packet->UCID, "DelFine"), players[ play.first ].PName, GetFineName(packet->UCID, packet->ReqI));
                        SendMTC( packet->UCID , Msg);

                        players[ play.first ].fines[j].fine_id = 0;
                        players[ play.first ].fines[j].fine_date = 0;
                        ofstream readf (fine_c, ios::app);
                        readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[ packet->UCID ].UName << " cancle fine ID = " << packet->ReqI << " to "  << players[ play.first ].UName << endl;
                        readf.close();

                        if (players[play.first].ThisFineCount>0)
                        {
                            for (int j=0; j<20; j++)
                            {
                                if ( strlen(players[play.first].ThisFine[j]) == 0 )
                                {
                                    sprintf(players[play.first].ThisFine[j], "   ^1^KЎї ^7%s (^2ID = %d^7)   -   %s", GetFineName(play.first, packet->ReqI), fines[packet->ReqI].id, players[ packet->UCID ].PName);
                                    players[play.first].ThisFineCount++;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    if ( packet->ClickID <= 32 )
    {
        players[ packet->UCID ].BID2 =  packet->ClickID;
    }

    /** Штрафы **/
    if (packet->ClickID==38 and players[packet->UCID].cop)
    {
        ShowFinesPanel(packet->UCID, packet->ReqI);
    }

    /** Включаем погоню **/
    if (packet->ClickID==40)
    {
        if (strlen(players[packet->ReqI].UName) == 0)
        {
            return;
        }

        if (players[packet->UCID].DTPstatus <= 0)
        {
            if (players[packet->ReqI].Pogonya == 0)
            {
                players[packet->UCID].DTPstatus = -1;
                players[packet->ReqI].Pogonya = 1;
                int worktime = time(NULL);
                players[packet->ReqI].WorkTime = worktime+60*5;
                char Text[96];
                sprintf(Text, msg->_(packet->ReqI, "PogonyaOn" ), players[packet->ReqI].PName, players[packet->UCID].PName );
                SendMTC(255, Text);
                nrg->Lock(packet->ReqI);
            }
        }
        else
        {
            SendMTC(packet->UCID, "^2| ^C^7Вы не можете начинать погоню во время разбора ДТП");
        }

        return;
    }

    /** Выключаем погоню **/
    if (packet->ClickID==41)
    {
        if (strlen(players[packet->ReqI].UName) == 0)
        {
            return;
        }

        if (players[packet->ReqI].Pogonya != 0)
        {
            players[packet->UCID].DTPstatus = 0;
            for (int i = 0; i < 32; i++)
            {
                if (DTPvyzov[0][i] != 0 and DTPvyzov[2][i] == packet->UCID)
                {
                    players[packet->UCID].DTPstatus = 1;
                    break;
                }
            }

            players[packet->ReqI].Pogonya = 0;
            char Text[96];
            sprintf(Text, msg->_(packet->ReqI, "PogonyaOff" ), players[packet->ReqI].PName, players[packet->UCID].PName );
            SendMTC(255, Text);
            nrg->Unlock(packet->ReqI);

            //закрывашка штрафов
            for (byte c = 80; c < 165; c++)
            {
                SendBFN(packet->UCID, c);
            }

            //очистка кнопок
            for (byte i = 60; i < 92; i++)
            {
                SendBFN(255, i);
            }
        }
        return;
    }
}

void RCPolice::InsimBTT( struct IS_BTT* packet )
{
    SYSTEMTIME sm;
    GetLocalTime(&sm);

    char fine_c[255];
    sprintf(fine_c, "%slogs\\fines\\fine(%d.%d.%d).txt", RootDir, sm.wYear, sm.wMonth, sm.wDay);

    /** Арест на Х минут **/
    if ( packet->ClickID == 79)
    {
        if (strlen(packet->Text)==0)
        {
            return;
        }

        players[packet->ReqI].Pogonya = 0;
        nrg->Unlock(packet->ReqI);

        //закрывашка штрафов
        for (byte c = 80; c<165; c++)
        {
            SendBFN(packet->UCID, c);
        }

        //очистка кнопок
        for (byte i = 60; i < 92; i++)
        {
            SendBFN(255, i);
        }

        time_t now = time(NULL);
        char str[96];

        int MAXmin = 0;
        if (players[packet->UCID].Rank == 3)
        {
            MAXmin = 30;
        }

        if (players[packet->UCID].Rank == 4)
        {
            MAXmin = 60;
        }

        int Min = atoi(packet->Text);
        if (Min > MAXmin)
        {
            Min = MAXmin;
        }
        else if (Min < 5 and Min > 0)
        {
            Min = 5;
        }

        int i=0, finded = -1;

        for(auto& p: ArestPlayers)
        {
            i = p.first;
            if (strcmp(ArestPlayers[p.first].UName, players[packet->ReqI].UName) == 0)
            {
                finded = i;
                break;
            }
        }

        if (Min > 0)
        {
            /*for (i=0; i < 32; i++)
                if (ginfo->players[i].UCID == packet->ReqI)
                {
                    ginfo->players[i].Zone = 1;
                    break;
                }*/

            sprintf(str,"/spec %s",players[packet->ReqI].UName);
            SendMST(str);

            sprintf(str,msg->_(packet->ReqI, "ArestedMin"), players[packet->ReqI].PName, Min);
            SendMTC(255,str);
            strcpy(ArestPlayers[finded != -1 ? finded : ++i].UName, players[packet->ReqI].UName);
            ArestPlayers[i].ArestTime = now + Min*60;
        }
        else if (finded != -1)
        {
            ArestPlayers.erase(finded);

            sprintf(str,msg->_(packet->ReqI, "Reliaved"),players[packet->ReqI].PName);
            SendMTC(255,str);
            SendBFN(packet->ReqI, 210);
            SendBFN(packet->ReqI, 211);
        }
    }

    /** Пользователь выписывает штраф **/
    if ( packet->ClickID == 38 )
    {
        for ( auto& play: players )
        {
            if  ( players[ packet->UCID ].BID2 == players[ play.first ].BID)
            {
                if (atoi(packet->Text) > 0)
                {

                    for (int j = 0; j < MAX_FINES; j++)
                    {
                        if ( fines[j].id == atoi(packet->Text) )
                        {
                            char Msg[128];

                            sprintf(Msg, msg->_(play.first, "GiveFine"), players[packet->UCID].PName, GetFineName(play.first, atoi(packet->Text)));
                            SendMTC( play.first, Msg);

                            sprintf(Msg, msg->_(packet->UCID, "AddFine" ), players[play.first].PName, GetFineName(packet->UCID, atoi(packet->Text)));
                            SendMTC( packet->UCID , Msg);

                            for (int j=0; j<MAX_FINES; j++)
                            {
                                if (players[ play.first ].fines[j].fine_id == 0)
                                {
                                    players[ play.first ].fines[j].fine_id = atoi(packet->Text);
                                    players[ play.first ].fines[j].fine_date = int( time( NULL ) );
                                    players[ play.first ].fines[j].CopName = players[ packet->UCID ].UName;
                                    players[ play.first ].fines[j].CopPName = players[ packet->UCID ].PName;
                                    break;
                                }
                            }
                            ofstream readf (fine_c, ios::app);
                            readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[ packet->UCID ].UName << " get fine ID = " << packet->Text << " to "  << players[ play.first ].UName << endl;
                            readf.close();

                            if (players[play.first].ThisFineCount==0)
                            {
                                SendBFNAll(play.first);
                            }

                            for (int j=0; j<20; j++)
                            {
                                if ( strlen(players[play.first].ThisFine[j]) == 0 )
                                {
                                    sprintf(players[play.first].ThisFine[j], "^7%d. %s (^2ID = %d^7)   -   %s", j+1, GetFineName(play.first, atoi(packet->Text)), fines[atoi(packet->Text)].id, players[ packet->UCID ].PName);
                                    players[play.first].ThisFineCount++;
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    /** Пользователь отменяет штраф **/
    if (packet->ClickID==39)
    {
        for ( auto& play: players )
        {
            if  (players[ packet->UCID ].BID2 == players[ play.first ].BID)
            {
                if ( atoi( packet->Text ) > 0 )
                {
                    for (int j=0; j<MAX_FINES; j++)
                    {
                        if ( players[ play.first ].fines[j].fine_id == atoi( packet->Text ) )
                        {
                            char Msg[128];

                            sprintf(Msg, msg->_( play.first, "DeletedFine"), players[packet->UCID].PName, GetFineName(play.first, atoi(packet->Text)));
                            SendMTC( play.first , Msg);

                            sprintf(Msg, msg->_(packet->UCID, "DelFine"), players[ play.first ].PName, GetFineName(packet->UCID, atoi(packet->Text)));
                            SendMTC( packet->UCID , Msg);

                            players[ play.first ].fines[j].fine_id = 0;
                            players[ play.first ].fines[j].fine_date = 0;
                            ofstream readf (fine_c, ios::app);
                            readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[ packet->UCID ].UName << " cancle fine ID = " << packet->Text << " to "  << players[ play.first ].UName << endl;
                            readf.close();

                            if (players[play.first].ThisFineCount>0)
                            {
                                for (int j=0; j<20; j++)
                                {
                                    if ( strlen(players[play.first].ThisFine[j]) == 0 )
                                    {
                                        sprintf(players[play.first].ThisFine[j], "   ^1^KЎї ^7%s (^2ID = %d^7)   -   %s", GetFineName(play.first, atoi(packet->Text)), fines[atoi(packet->Text)].id, players[ packet->UCID ].PName);
                                        players[play.first].ThisFineCount++;
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
}

void RCPolice::InsimPEN( struct IS_PEN* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];

    if (packet->Reason == PENR_WRONG_WAY )
    {
        for (int j=0; j<MAX_FINES; j++)
        {
            if (players[ UCID ].fines[j].fine_id == 0)
            {
                players[ UCID ].fines[j].fine_id = 18;
                players[ UCID ].fines[j].fine_date = int(time(NULL));

                char Msg[96];
                sprintf(Msg, msg->_( UCID , "GiveFine2" ), GetFineName(UCID, 18));
                SendMTC( UCID , Msg);
                break;
            }
        }
    }
    else if (packet->Reason == PENR_SPEEDING )
    {

        if ( packet->NewPen == PENALTY_30 or
                packet->NewPen == PENALTY_45 or
                packet->NewPen == PENALTY_DT or
                packet->NewPen == PENALTY_SG )
        {
            for (int j=0; j<MAX_FINES; j++)
            {
                if (players[ UCID ].fines[j].fine_id == 0)
                {
                    players[ UCID ].fines[j].fine_id = 13;
                    players[ UCID ].fines[j].fine_date = int(time(NULL));

                    char Msg[64];
                    sprintf(Msg, msg->_( UCID , "GiveFine2" ), GetFineName(UCID, 13));
                    SendMTC( UCID , Msg);
                    break;
                }
            }
        }
    }
}

void RCPolice::InsimPLA( struct IS_PLA* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];

    if (packet->Fact == PITLANE_EXIT)
    {
        char Text[64];
        sprintf(Text, "/p_clear %s", players[ UCID ].UName);
        SendMST(Text);

        int count = 0;
        for (int j=0; j<MAX_FINES; j++)
        {
            if (players[ UCID ].fines[j].fine_id != 0)
            {
                count++;
            }
        }

        if (count > 10)
        {
            char Text[64];
            sprintf(Text, "/pitlane %s", players[ UCID ].UName);
            SendMTC( UCID , msg->_(  UCID , "3400" ));
            SendMST(Text);
        }
    }
}

void RCPolice::InsimMCI( struct IS_MCI* packet )
{
    for (int i = 0; i < packet->NumC; i++)
    {
        byte UCID = PLIDtoUCID[packet->Info[i].PLID];

        if (UCID == 0)
        {
            return;
        }

        int SirenaCount = 0;
        int SDtemp=0;

        int S = packet->Info[i].Speed*360/32768;

        /** окошко со штрафами **/
        if ( players[UCID].ThisFineCount != 0 )
        {
            byte ClickID=CLICKID::CLICK_ID_128, w=90, h=10+5*players[UCID].ThisFineCount, l=100, t=90;
            SendButton(255, UCID, ClickID++, l-w/2, t-h/2, w, h+8, 32, "");
            SendButton(255, UCID, ClickID++, l-w/2, t-h/2, w, h+8, 32, "");									//фон
            SendButton(254, UCID, ClickID++, l-7, t-h/2+h+1, 14, 6, ISB_LIGHT + ISB_CLICK, "^2OK"); 		//закрывашка
            SendButton(255, UCID, ClickID++, l-w/2, t-h/2, w, 10, 3+64, msg->_(UCID, "GiveFine3")); 			//заголовок

            for (int j=0; j < players[UCID].ThisFineCount; j++)
                SendButton(255, UCID, ClickID++, l-w/2+1, t-h/2+10+5*j, w-2, 5, ISB_LEFT + ISB_LIGHT, players[UCID].ThisFine[j]);
        }

        /** автоотключение радара **/
        if (S > 5 and players[UCID].Radar)
        {
            SendMTC(UCID, msg->_(UCID, "RadarOff"));
            players[UCID].Radar = false;
        }

        /** сирена копа **/
        if (players[UCID].cop and players[UCID].Sirena)
        {
            SendButton(255, UCID, 140, 90, 26, 20, 10, 32, siren.c_str());
        }
        else if (players[UCID].cop)
        {
            SendBFN(UCID, 140);
        }

        for (auto& play: players)
        {
            if (UCID == play.first)
            {
                continue;
            }

            int X1 = players[UCID].Info.X/65536,
                Y1 = players[UCID].Info.Y/65536,
                X2 = players[play.first].Info.X/65536,
                Y2 = players[play.first].Info.Y/65536,
                Dist = Distance(X1, Y1, X2, Y2);

            /** арест **/
            if (players[UCID].Pogonya == 1 and players[play.first].cop and players[play.first].Rank != 1)
            {
                if (S < 5 and Dist < 10)
                {
                    players[UCID].StopTime++;
                    if (players[UCID].StopTime >= 3)
                    {
                        players[UCID].StopTime = 0;
                        players[UCID].Pogonya = 2;
                        //nrg->Unlock(UCID);
                        char Text[128];
                        sprintf(Text, "^2| %s%s", players[UCID].PName, msg->_(UCID, "1702"));
                        SendMTC(255, Text);
                        SendMTC(play.first, msg->_(play.first, "1703"));

                        //запралата копам, которые были рядом во время ареста
                        for (auto& p: players)
                        {
                            if (players[p.first].cop)
                            {
                                int XX2 = players[p.first].Info.X/65536,
                                    YY2 = players[p.first].Info.Y/65536,
                                    Dist2 = Distance(X1, Y1, XX2, YY2);

                                if (Dist2<30)
                                {
                                    players[p.first].DoneCount++;
                                }
                            }
                        }
                    }
                }
                if (S > 5)
                {
                    players[UCID].StopTime = 0;
                }
            }

            /** сирена у игрока **/
            if (players[play.first].Sirena and players[play.first].cop and Dist < 180)
            {
                SirenaCount++;
                if (Dist < SDtemp or SDtemp ==0)
                {
                    SDtemp = Dist;
                }
            }
            players[UCID].SirenaDist = SDtemp;

            /** заявка на дтп **/
            if (players[UCID].DTP != 0 and play.first == players[UCID].DTP and Dist < 30 and players[play.first].DTPstatus == 1 and players[play.first].Info.Speed*360/32768 < 5)
            {
                players[play.first].DTPstatus = 2;
                players[UCID].DTP = 0;
            }

            /** радар **/
            if (players[play.first].Radar and !players[UCID].cop and players[UCID].Pogonya == 0 and Dist < 50)
            {
                if (Dist<25)
                {
                    int Speed = players[UCID].Info.Speed*360/32768;
                    struct streets StreetInfo;
                    street->CurentStreetInfo(&StreetInfo, UCID);

                    if (Speed > StreetInfo.SpeedLimit+10 and Speed > players[UCID].speed_over)
                        players[UCID].speed_over=Speed;
                }
                else if (players[UCID].speed_over>0)
                {
                    struct streets StreetInfo;
                    street->CurentStreetInfo(&StreetInfo, UCID);

                    char text[128];
                    int Speed2 = abs(players[UCID].speed_over - StreetInfo.SpeedLimit);
                    sprintf(text,msg->_(UCID, "Speeding"), players[UCID].PName, Speed2, street->GetStreetName(UCID, StreetInfo.StreetID), players[play.first].PName);
                    SendMTC(255,text);

                    if (players[UCID].Pogonya == 0)
                    {
                        players[UCID].Pogonya = 1;
                        int worktime = time(NULL);
                        players[UCID].WorkTime = worktime+60*5;
                        sprintf(text, msg->_(UCID, "PogonyaOn" ), players[UCID].PName, players[play.first].PName );
                        SendMTC(255, text);
                        nrg->Lock( UCID );
                    }
                    players[UCID].speed_over=0;
                }
            }
        }

        /** сирена у игрока **/
        if (SirenaCount > 0)
        {
            SendButton(255, UCID, 141, 0, 36, 200, (180-players[UCID].SirenaDist+4)/4, 0, siren.c_str());
        }
        else if (players[UCID].SirenaDist > 0 or SirenaCount == 0)
        {
            players[UCID].SirenaDist = 0;
            SendBFN(UCID, 141);
        }

        if (players[UCID].Pogonya == 0)
        {
            SendBFN(UCID, 204);
            SendBFN(UCID, 205);
            SendBFN(UCID, 210);
        }

        if (players[UCID].Pogonya != 0)
        {
            BtnPogonya(UCID);
        }

        memcpy(&players[UCID].Info, &packet->Info[i], sizeof(CompCar));
    }
}

void RCPolice::readconfig()
{
    cout << "RCTaxi::readconfig\n" ;
    char file[MAX_PATH];
    sprintf(file, "%sdata\\RCPolice\\FineAllow.txt", RootDir);

    HANDLE hwnd;
    WIN32_FIND_DATA fd;
    hwnd = FindFirstFile(file, &fd);
    if (hwnd == INVALID_HANDLE_VALUE)
    {
        printf ("RCTaxi: Can't find \n%s", file);
        return;
    }
    FindClose(hwnd);
    ifstream readf (file, ios::in);

    while (readf.good())
    {
        char line[128];
        readf.getline(line, 128);

		char str[128];
		sprintf(str, strtok (line, ";"));

        if (strlen(str) > 0)
        {
            int rnk = atoi(strtok(str, "="));

            if (rnk == 0 or rnk>4)
            {
				continue;
            }

            for (int i=0; i<MAX_FINES; i++)
			{
				int c = atoi(strtok(NULL, ","));

                if (c==0)
                {
					break;
                }

                FineAllow[rnk][i] = c;
			}
        }
    }
    readf.close();
}

void RCPolice::SetSirenLight( string sirenWord )
{
    siren = sirenWord;
}

bool RCPolice::IsCop( byte UCID )
{
    return players[UCID].cop;
}

int RCPolice::GetCopRank( byte UCID )
{
    return players[UCID].Rank;
}

int RCPolice::GetCopDTPstatus( byte UCID )
{
    return players[UCID].DTPstatus;
}

int RCPolice::InPursuite( byte UCID )
{
    return players[UCID].Pogonya;
}

int RCPolice::GetCopCount()
{
    int c = 0;
    for ( auto& play: players )
    {
        if (players[play.first].cop)
        {
            c++;
        }
    }
    return c;
}

int RCPolice::GetFineCount()
{
    int c=0;
    for (int i=0; i<MAX_FINES; i++)
    {
        if (fines[i].id != 0)
        {
            c++;
        }
    }
    return c;
}

void RCPolice::BtnPogonya(byte UCID)
{
    if (players[UCID].Pogonya == 1)
    {
        SendButton(255, UCID, 204, 0, 20, 200, 30, 1, msg->_(UCID, "RideButton" ));
        SendButton(255, UCID, 205, 0, 43, 200, 6, 0, msg->_(UCID, "RightAndStop" ));
    }
    else if (players[UCID].Pogonya == 2)
    {
        SendButton(255, UCID, 204, 0, 20, 200, 30, 1, msg->_(UCID, "ArestButton"));
        SendBFN(UCID, 205);
    }
}

void RCPolice::SetUserBID ( byte UCID, byte BID )
{
    players[ UCID ].BID = BID;
}

void RCPolice::SaveUserFines ( byte UCID )
{
    cout <<players[ UCID ].UName << " save fines_info" << endl;

    char file[255];
    sprintf(file, "%sdata\\RCPolice\\fines\\%s.txt", RootDir, players[ UCID ].UName);

    ofstream writef (file, ios::out);
    for (int i = 0; i < MAX_FINES; i++)
    {
        if (players[ UCID ].fines[i].fine_id > 0)
        {
            writef << players[ UCID ].fines[i].fine_id << ";" << players[ UCID ].fines[i].fine_date << ";" << players[ UCID ].fines[i].CopName << ";" << players[ UCID ].fines[i].CopPName <<  endl;
        }
    }

    writef.close();
}

void RCPolice::ReadUserFines( byte UCID )
{

    char file[255];
    sprintf(file, "%sdata\\RCPolice\\fines\\%s.txt", RootDir, players[ UCID ].UName);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file, &fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
        SaveUserFines( UCID );
    }
    else
    {
        ifstream readf (file, ios::in);
        int i=0;
        while (readf.good())
        {
            char str[128];
            readf.getline(str, 128);

            if (strlen(str) > 0)
            {
                char *id;
                char *date;
                char *CopName;
                char *CopPName;

                id = strtok(str, ";");
                date = strtok(NULL, ";");
                CopName = strtok(NULL, ";");
                CopPName = strtok(NULL, ";");

                players[ UCID ].fines[i].fine_id = atoi(id);
                if ( date )
                {
                    players[ UCID ].fines[i].fine_date = atoi(date);
                }

                if ( CopName )
                {
                    players[ UCID ].fines[i].CopName = CopName;
                }

                if ( CopPName )
                {
                    players[ UCID ].fines[i].CopPName = CopPName;
                }

                i++;
            }
        }
    }
}

void RCPolice::ReadFines()
{
    char file[255];
    strcpy(file, RootDir);
    sprintf(file, "%sdata\\RCPolice\\fines.txt" , RootDir);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file, &fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file, ios::in);

    int i = 0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);
        if (strlen(str) > 0)
        {
            if (strstr(str, "//"))
            {
                continue;
            }

            char * id;
            char * NameRu;
            char * NameEn;
            char * cash;
            id = strtok (str, ";");
            NameRu = strtok (NULL, ";");
            NameEn = strtok (NULL, ";");
            cash = strtok (NULL, ";");

            i = atoi(id);
            memset(&fines[i], 0, sizeof(struct fine));
            fines[i].id = i;
            strncpy(fines[i].NameRu, NameRu, 64);
            strncpy(fines[i].NameEn, NameEn, 64);
            fines[i].cash = atoi(cash);

        } // if strlen > 0
    } //while readf.good()

    readf.close();
}

void RCPolice::ButtonClock( byte UCID )
{
    char str[10];
    int TIME = players[UCID].WorkTime - time(NULL);
    sprintf(str, "^2%02d:%02d", TIME/60, TIME%60);

    SendButton(255, UCID, 210, 130, 1, 10, 8, 32, str);
}

char* RCPolice::GetFineName(byte UCID, int FineID)
{
    if (msg->GetLangID(UCID)==LANG_ID_RUS)
    {
        return fines[FineID].NameRu;
    }
    else
    {
        return fines[FineID].NameEn;
    }
}

void RCPolice::SendMTCToCop(const char* Msg, int Rank, ...)
{
    int *p=&Rank;
    while (*p)
    {
        for ( auto& play: players )
        {
            if (players[play.first].cop and players[play.first].Rank == *p++)
            {
                SendMTC(play.first, Msg);
            }
        }
    }

}

void RCPolice::Event()
{
    for( auto& p: ArestPlayers )
    {
        char PName[24];
        byte UCID = 0;
        sprintf(PName, ArestPlayers[p.first].UName);

        for (auto& pp: players)
        {
            if (strcmp(ArestPlayers[p.first].UName, players[pp.first].UName) == 0)
            {
                sprintf(PName, players[pp.first].PName);
                UCID = pp.first;
                break;
            }
        }

        if (time(NULL) > ArestPlayers[p.first].ArestTime)
        {
            ArestPlayers.erase(p.first);

            char str[128];
            sprintf(str, msg->_(UCID, "Reliaved"), PName);
            SendMTC(255,str);
            SendBFN(UCID, 210);
            SendBFN(UCID, 211);

            break;
        }
        else if (UCID != 0)
        {
            char str[10];
            int TIME = ArestPlayers[p.first].ArestTime - time(NULL);
            sprintf(str,"^1%02d:%02d",TIME/60,TIME%60);
            SendButton(255,UCID,210,130,1,10,8,32,str);
            SendButton(255,UCID,211,120,9,20,5,32, msg->_(UCID, "YouUndArest"));
        }
    }


    for ( auto& play: players )
    {
        byte UCID = play.first;
        auto playr = play.second;

        if ( playr.Pogonya == 1 )
        {
            ButtonClock(UCID);
            int nowtime = time( NULL );
            if (playr.WorkTime <= nowtime)
            {
                SendBFN( UCID , 210);

                char Text[64];
                sprintf( Text , "/msg ^2| %s %s" , playr.PName , msg->_(  UCID , "1706" ) );

                SendMST(Text);
                players[ UCID ].Pogonya = 0;
                nrg->Unlock( UCID );
                dl->AddSkill( UCID );

                //очистка кнопок
                for (byte i = 60; i<92; i++)
                {
                    SendBFN(255, i);
                }
            }
        }

        if (players[UCID].cop)
        {
            streets StreetInfo;

            //зарплата ДПС по сменам каждые 30 минут с момента заступления на смену
            CopPayRoll(UCID);

            char smn[20];
            int TM = 1800 - (time(NULL) - players[UCID].StartWork)%1800;
            sprintf(smn, "^CСмена: %02d:%02d ", (TM/60)%60, TM%60);
            SendButton(255, UCID, 70, 115, 1, 15, 4, 3+128, smn);

            //заявки на дтп
            for (int i = 0; i < 32; i++)
            {
                if (DTPvyzov[0][i] >= 0 and DTPvyzov[1][i] >= 0 and players[UCID].DTPstatus != -1 and players[UCID].Rank != 3)
                {
                    street->CurentStreetInfo(&StreetInfo, DTPvyzov[0][i]);	// улица, где дтп
                    byte id = 135;											// стартовый ИД кнопок

                    if (DTPvyzov[2][i] <= 0 and DTPvyzov[1][i] > 0)
                    {
                        int T = DTPvyzov[1][i] - time(NULL); 				// сколько осталось до конца заявки

                        char str[96];
                        sprintf(str, "^C^1Вызов на ДТП - ^8%s^7, %s, ^1%02d:%02d", players[DTPvyzov[0][i]].PName, StreetInfo.StreetRu, (T/60)%60, T%60);
                        SendButton(200, UCID, id+i, 159, 100 + 4*i, 40, 4, 32 + 8, str);

                        if (T<=0)
                        {
                            players[DTPvyzov[0][i]].DTP = 0;

                            if (DTPvyzov[2][i] != 0)
                            {
                                players[DTPvyzov[2][i]].DTPstatus = 0;
                            }

                            DTPvyzov[0][i] = 0;
                            DTPvyzov[1][i] = 0;
                            DTPvyzov[2][i] = 0;
                            SendBFN(255, id+i);

                            //штраф всем копам что профукали заявку
                            for (auto& play: players)
                            {
                                if (players[play.first].cop and players[play.first].DTPstatus == 0)
                                {
                                    SendMTC(play.first, "^2| ^1^CВы пропустили вызов на ДТП");
                                    if (dl->Islocked(play.first))
                                    {
                                        dl->Unlock( play.first );
                                        dl->RemSkill( play.first, 0.2);
                                        dl->Lock( play.first );
                                    }
                                    else
                                    {
                                        dl->RemSkill( play.first, 0.2);
                                    }
                                }
                            }

                            //нужен штраф за ложный вызов
                            //не снимать скилл и не показывать список заявок если ты в погоне
                        }
                    }
                    else if (DTPvyzov[2][i] == UCID)
                    {
                        if (players[UCID].DTPstatus == 1)
                        {
                            char str[96];
                            sprintf(str, "^CЗаявка принята - %s, %s", players[DTPvyzov[0][i]].PName, StreetInfo.StreetRu);
                            SendButton(200, UCID, id+i, 159, 100 + 4*i, 40, 4, 32+3, str);
                        }
                        else
                        {
                            char str[96];
                            sprintf(str, "^C^2Заявка ^8%s ^2^Cна рассмотрении - ^1закрыть", players[DTPvyzov[0][i]].PName, StreetInfo.StreetRu);
                            SendButton(200, UCID, id+i, 159, 100 + 4*i, 40, 4, 32+8, str);
                        }
                    }
                    else if (DTPvyzov[0][i] > 0 and DTPvyzov[2][i] > 0)
                    {
                        char str[96];
                        sprintf(str, "^CВызов на ДТП принят - %s^8, %s", players[DTPvyzov[0][i]].PName, StreetInfo.StreetRu);
                        SendButton(200, UCID, id+i, 159, 100 + 4*i, 40, 4, 32+7, str);
                    }
                }
            }

            // Погоны
            int id = 214,
                L = 58,
                H = 1;

            SendButton(255, UCID, id++, L, H + 3, 6, 2, 0, "^3^KЎЬ");
            SendButton(255, UCID, id++, L - 1, H - 1, 5, 10, 0, "^1^JЃs");
            SendButton(255, UCID, id++, L - 1, H - 1, 14, 10, 0, "^1^J             Ѓb");
            SendButton(255, UCID, id++, L - 0, H - 1, 12, 10, 0, "^1^JЃPЃPЃPЃP");
            SendButton(255, UCID, id++, L - 0, H - 1, 12, 10, 0, "^1^JЃQЃQЃQЃQ");

            if (players[UCID].Rank == 1)
            {
                //мл. лейт.
                SendButton(255, UCID, id++, L + 6, H + 2, 4, 4, 0, "^3^JЃљ");
            }
            else if (players[UCID].Rank == 2)
            {
                //лейт
                SendButton(255, UCID, id++, L + 6, H + 1, 5, 3, 0, "^3^JЃљ");
                SendButton(255, UCID, id++, L + 6, H + 4, 5, 3, 0, "^3^JЃљ");
            }
            else if (players[UCID].Rank == 3)
            {
                //ст. лейт
                SendButton(255, UCID, id++, L + 4, H + 2, 5, 4, 0, "^3^JЃљ");
                SendButton(255, UCID, id++, L + 6, H + 1, 5, 3, 0, "^3^JЃљ");
                SendButton(255, UCID, id++, L + 6, H + 4, 5, 3, 0, "^3^JЃљ");
            }
            else if (players[UCID].Rank == 4)
            {
                //капитан
                SendButton(255, UCID, id++, L + 2, H + 2, 6, 4, 0, "^3^JЃљ");
                SendButton(255, UCID, id++, L + 5, H + 2, 5, 4, 0, "^3^JЃљ");
                SendButton(255, UCID, id++, L + 7, H + 1, 5, 3, 0, "^3^JЃљ");
                SendButton(255, UCID, id++, L + 7, H + 4, 5, 3, 0, "^3^JЃљ");
            }


            /** ====== **/

            struct IS_BTN pack;
            memset(&pack, 0, sizeof(struct IS_BTN));
            pack.Size = sizeof(struct IS_BTN);
            pack.Type = ISP_BTN;
            pack.ReqI = 1;
            pack.UCID =  UCID ;
            pack.Inst = 0;
            pack.TypeIn = 0;
            pack.ClickID = CLICKID::CLICK_ID_60;
            pack.BStyle = 32;
            pack.L = 73;
            pack.T = 191;
            pack.W = 50;
            pack.H = 4;

            for ( auto& play2: players)
            {
                byte UCID2 = play2.first;
                auto playr2 = play2.second;
                if ( playr2.Pogonya != 0 )
                {
                    int time2 = playr2.WorkTime - time(NULL);
                    int min = (time2/60)%60;
                    int sec = time2%60;

                    float D = Distance( playr.Info.X, playr.Info.Y, playr2.Info.X, playr2.Info.Y)/65536;

                    street->CurentStreetInfo(&StreetInfo, UCID2);

                    if (playr2.Pogonya == 1)
                    {
                        sprintf(pack.Text, "%s^7 - %s, %0.0f ^Cм ^2(^1%02d:%02d^2)", playr2.PName, StreetInfo.StreetRu, D, min, sec );
                    }
                    else if (playr2.Pogonya == 2)
                    {
                        sprintf(pack.Text, "%s ^7 - %s, ^1^Cарестован", playr2.PName, StreetInfo.StreetRu);
                    }

                    insim->send_packet(&pack);
                    pack.T -=4;
                    pack.ClickID ++;
                }
            }
        }
    }
}
