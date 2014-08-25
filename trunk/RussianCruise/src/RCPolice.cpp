#include "RCPolice.h"
RCPolice::RCPolice(const char* Dir)
{
    ClassName = "RCPolice";
    strcpy(RootDir,Dir);
    memset( &fines, 0, sizeof( fine ) * MAX_FINES );
}

RCPolice::~RCPolice()
{

}

int RCPolice::init(MYSQL *conn, CInsim *InSim, void *Message, void *Bank, void *RCdl, void * STreet, void *Energy, void *Light)
{
    dbconn = conn;
    if (!dbconn)
    {
        CCText("^3RCPolice:\t\t^1Can't sctruct MySQL Connector");
        return -1;
    }
    insim = InSim;
    if (!insim)
    {
        CCText("^3RCPolice:\t\t^1Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)Message;
    if (!msg)
    {
        CCText("^3RCPolice:\t^1Can't struct RCMessage class");
        return -1;
    }

    bank = (RCBank *)Bank;
    if (!bank)
    {
        CCText("^3RCPolice:\t^1Can't struct RCBank class");
        return -1;
    }

    dl = (RCDL *)RCdl;
    if (!dl)
    {
        CCText("^3RCPolice:\t^1Can't struct RCDL class");
        return -1;
    }

    street = (RCStreet *)STreet;
    if (!street)
    {
        CCText("^3RCPolice:\t^1Can't struct RCStreet class");
        return -1;
    }

    nrg = (RCEnergy *)Energy;
    if (!nrg)
    {
        CCText("^3RCPolice:\t^1Can't struct RCEnergy class");
        return -1;
    }

    lgh = (RCLight *)Light;
    if (!lgh)
    {
        CCText("^3RCPolice:\t^1Can't struct RCLight class");
        return -1;
    }

    CCText("^3R"+ClassName+":\t^2inited");
    return 0;
}

void RCPolice::InsimNCN( struct IS_NCN* packet )
{
    if ( packet->UCID == 0 )
    {
        return;
    }

    players[packet->UCID].UName = packet->UName;
    players[packet->UCID].PName = packet->PName;
    players[packet->UCID].Admin = packet->Admin;

    //читаем штрафы
    ReadUserFines(packet->UCID);
}

void RCPolice::InsimNPL( struct IS_NPL* packet )
{
	byte UCID = packet->UCID;
    PLIDtoUCID[packet->PLID] = packet->UCID;
    players[packet->UCID].CName = packet->CName;

	if ( ArestPlayers.end() != ArestPlayers.find( players[ packet->UCID ].UName ) )
	{
		time_t now = time(NULL);

		if ( now <= ArestPlayers[  players[packet->UCID].UName ] )
		{
			insim->SendMST("/spec " +  players[packet->UCID].UName );

			char str[96];
			sprintf(str, "^2| ^7^CВы арестованы. До конца ареста осталось ^1%d ^7минут", (int)((ArestPlayers[ players[packet->UCID].UName ] - now) / 60 + 1));
			insim->SendMTC(packet->UCID, str);
		}

	}


    if ((players[packet->UCID].PName.find("^4[^C^7ДПС^4]") != string::npos || players[packet->UCID].PName.find("^4[^C^7ГАИ^4]") != string::npos) and !players[packet->UCID].cop)
    {

        players[packet->UCID].Rank = 0;

        if ( LoadCopStat(packet->UCID) && players[packet->UCID].Rank > 0)
        {
            players[packet->UCID].cop = true;
            insim->SendMTC(packet->UCID, "^2| ^7^CВы заступили на смену");

            if (!((strncmp("DPS", packet->SName, 3) == 0)
                    || (strncmp("dps", packet->SName, 3) == 0)
                    || (strncmp("POLICE", packet->SName, 6) == 0)
                    || (strncmp("police", packet->SName, 6) == 0)))
                insim->SendMTC(packet->UCID, "^2| ^C^7В названии скина не найдено указателей на принадлежность к ДПС");


            players[packet->UCID].StartWork = time(NULL) - 5;


			players[UCID].PStat.DateActive = time(NULL);


            tm *timeStruct = tools::GetLocalTime();

            if (players[UCID].PStat.CurrentDay != timeStruct->tm_mday)
            {
                players[UCID].PStat.CurrentDay = timeStruct->tm_mday;
                players[UCID].PStat.ArrestWithFineByDay = 0;
                players[UCID].PStat.ArrestWithOutFineByDay = 0;
                players[UCID].PStat.SolvedIncedentsByDay = 0;
                players[UCID].PStat.FinedByDay = 0;
                players[UCID].PStat.CanceledFinesByDay = 0;
                players[UCID].PStat.HelpArrestByDay = 0;
            }
        }
        else
        {
            insim->SendMTC(packet->UCID, msg->_(packet->UCID , "CopFalse"));
            insim->SendMST("/spec " + players[packet->UCID].UName);
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

    if (players[UCID].Pogonya != 0)
        players[UCID].Pogonya = 2;

    players[UCID].Sirena = false;
    if (players[UCID].Radar.On)
        RadarOff(UCID);

    lgh->SetLight3(UCID, false);
    dl->Unlock(UCID);
    nrg->Unlock(UCID);

    if (players[UCID].cop)
        SaveCopStat(UCID);

    //PLIDtoUCID.erase(packet->PLID);
}

void RCPolice::InsimPLL( struct IS_PLL* packet )
{
    byte UCID = PLIDtoUCID[packet->PLID];

    if (players[UCID].Pogonya != 0)
        players[UCID].Pogonya = 2;

    players[UCID].Sirena = false;
    if (players[UCID].Radar.On)
        RadarOff(UCID);

    lgh->SetLight3(UCID, false);
    dl->Unlock(UCID);
    nrg->Unlock(UCID);

    if (players[UCID].cop)
        SaveCopStat(UCID);

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
                DTPvyzov[1][i] = time(NULL) + 2 * 60;
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
                insim->SendBFN(255, 91 + i);
                break;
            }
        }
    }

    CopPayRoll(packet->UCID, false);

    Save(packet->UCID);
    players.erase(packet->UCID);
}

void RCPolice::InsimCPR( struct IS_CPR* packet )
{
    players[packet->UCID].PName = packet->PName;

    if (players[packet->UCID].cop && ( players[packet->UCID].PName.find("^4[^C^7ДПС^4]") == string::npos && players[packet->UCID].PName.find("^4[^C^7ГАИ^4]") == string::npos ) )
    {
        SaveCopStat(packet->UCID);
        CopPayRoll(packet->UCID, false);
        insim->SendBFN(packet->UCID, 70);
        players[packet->UCID].cop = false;
        players[packet->UCID].StartWork = 0;
        players[packet->UCID].Rank = 0;
        lgh->SetLight3(packet->UCID, false);
		dl->Unlock(packet->UCID);
		nrg->Unlock(packet->UCID);
        insim->SendMTC(packet->UCID, "^2| ^7^CВы больше не инспектор ДПС");
        insim->SendBFNAll(packet->UCID);

        for (int i = 0; i < 32; i++)
        {
            if (DTPvyzov[0][i] > 0)
            {
                if (DTPvyzov[2][i] == packet->UCID)
                {
                    DTPvyzov[1][i] = time(NULL) + 2 * 60;
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
                    insim->SendBFN(255, 91 + i);
                    break;
                }
            }
        }

        for (int i=214; i <= 222; i++)
            insim->SendBFN(packet->UCID, i);
    }
}

bool RCPolice::IsPursuit(byte UCID)
{
    return players[UCID].Pogonya > 0;
}

void RCPolice::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
        return;

    if ( packet->UserType != MSO_PREFIX )
        return;

    byte UCID = packet->UCID;

    xString Message = packet->Msg + packet->TextStart;
	vector<string> args = Message.split(' ',1);

    if (Message == "!911" or Message == "!dtp" or Message == "!^Cдтп")
    {
        if (players[UCID].cop)
            return;

        if (GetCopCount() <= 0)
        {
            insim->SendMTC(UCID, msg->_(UCID, "2102"));
            return;
        }

        if (players[UCID].Pogonya > 0)
        {
            insim->SendMTC(UCID, msg->_(UCID, "WherePog"));
            return;
        }

        if (players[UCID].DTP != 0)
        {
            insim->SendMTC(UCID, msg->_(UCID, "WhereDtp"));
            return;
        }

        if (players[UCID].Info.Speed * 360 / 32768 > 0)
        {
            insim->SendMTC(UCID, msg->_(UCID, "NoDtpWhenSp"));
            return;
        }

        if (time(NULL)-players[UCID].LastDtpTime < 60*5)
        {
            insim->SendMTC(UCID, msg->_(UCID, "2106"));
            return;
        }

		players[UCID].blame = false;
        players[UCID].LastDtpTime = time(NULL);

        players[UCID].DTP = 255;

        for (int i = 0; i < 32; i++)
        {
            if (DTPvyzov[0][i] <= 0)
            {
                DTPvyzov[0][i] = (int)UCID; 		    //кто вызвал
                DTPvyzov[1][i] = time(NULL) + 5 * 60;	//когда вызвал

                insim->SendMTC(UCID, msg->_(UCID, "2107"));
                insim->SendMTC(UCID, msg->_(UCID, "2108"));

                char str[96];
                sprintf(str, msg->_(UCID, "2109"), players[UCID].PName.c_str());
                SendMTCToCop(str, 1, 2, 4);
                return;
            }
        }

        insim->SendMTC(UCID, msg->_(UCID, "2110"));
    }


    if (Message == "!fines" or Message == "!^Cштрафы")
    {
        int j=0;
        for (int i=0; i < MAX_FINES; i++)
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
                insim->SendMTC( packet->UCID , Text);
                j++;
            }
        }

        if (j == 0)
        {
            insim->SendMTC(packet->UCID , msg->_(  packet->UCID , "NoFines" ));
        }
    }

    if (Message.find("!pay") == 0 or Message.find("!^Cоплатить") == 0)
    {



        if ( args.size() < 2)
        {
            insim->SendMTC( packet->UCID , msg->_(  packet->UCID , "2105" ));
            return;
        }

        int id_i = atoi(args[1].c_str());

        if ( id_i < 1 || id_i > MAX_FINES || fines[id_i].cash <= 0)
        {
            insim->SendMTC( packet->UCID , msg->_(  packet->UCID , "2105" ));
            return;
        }

        if (bank->InBank( packet->UCID ))
        {
            if (fines[id_i].cash < bank->GetCash( packet->UCID ))
            {
                int j=0;
                for (j=0; j < MAX_FINES; j++)
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

                                bank->AddCash( play.first , (fines[id_i].cash) * 0.05, true);
                                cop++;
                            }
                        }

                        insim->SendMTC( packet->UCID , msg->_(  packet->UCID , "PayDone" ));
                        bank->RemCash( packet->UCID , fines[id_i].cash);
                        bank->AddToBank((fines[id_i].cash) - ((fines[id_i].cash) * 0.05) * cop);

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
                        insim->SendMTC( packet->UCID, msg->_(  packet->UCID , "NoFine" ));
                    }
                }
            }
            else
            {
                insim->SendMTC(packet->UCID, msg->_(packet->UCID, "NoManyPay"));
            }
        }
        else
        {
            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "NoOnBank"));
        }
    }

    if (Message == "!sirena" or Message == "!^Cсирена")
    {
        if (players[packet->UCID].cop)
        {
            if (players[packet->UCID].Sirena)
            {
                insim->SendMTC(packet->UCID, msg->_(packet->UCID, "2100"));
                players[packet->UCID].Sirena = false;
            }
            else
            {
                insim->SendMTC(packet->UCID, msg->_(packet->UCID, "2101"));
                players[packet->UCID].Sirena = true;
            }
        }
    }

    if (Message == "!radar" or Message == "!^Cрадар")
    {
        if (players[packet->UCID].cop)
        {
            if (players[packet->UCID].Rank == 1 || players[packet->UCID].Rank == 3)
            {
                insim->SendMTC(UCID, msg->_(UCID, "2111"));
                return;
            }

            if (players[UCID].DTPstatus <= 0)
            {
                if (players[UCID].Radar.On)
                {
                    RadarOff(packet->UCID);
                    insim->SendMTC(UCID, msg->_(UCID, "RadarOff"));
                }
                else
                {
                    struct streets StreetInfo;
                    street->CurentStreetInfo(&StreetInfo, packet->UCID);

                    int count = 0;
                    for (auto& play: players)
                    {
                        if (players[play.first].Radar.On && ++count > 2)
                        {
                            insim->SendMTC(packet->UCID, msg->_(packet->UCID, "EndOfRadar"));
                       		return;
                        }
                    }

                    if (StreetInfo.SpeedLimit == 0)
                    {
                        insim->SendMTC(UCID, "^2| ^7Вы не можете поставить радар в этом месте");
                        return;
                    }

                    int X, Y, H, Z;
                    H = players[UCID].Info.Heading / 182;
                    X = players[UCID].Info.X / 4096 - cos((H - 60) * M_PI / 180) * 40;
                    Y = players[UCID].Info.Y / 4096 - sin((H - 60) * M_PI / 180) * 40;
                    Z = players[UCID].Info.Z / 16452;

                    cout << " X: " << X;
                    cout << " Y: " << Y;
                    cout << " Z: " << Z;
                    cout << endl;

                    ObjectInfo *obj = new ObjectInfo;
                    obj->Index = 22;
                    obj->Heading = H/360*256;
                    obj->X = X;
                    obj->Y = Y;
                    obj->Zchar = Z;
                    obj->Flags = 0;
                    AddObject(obj);

                    obj->Index = 138;
                    AddObject(obj);
                    delete obj;

                    players[UCID].Radar.X = X;
                    players[UCID].Radar.Y = Y;
                    players[UCID].Radar.Z = Z;
                    players[UCID].Radar.On = true;

                    //CCText(ToString(players[UCID].Radar.X) + " " + ToString(players[UCID].Radar.Y) + " " + ToString(players[UCID].Radar.Z));

                    insim->SendMTC(packet->UCID, msg->_(packet->UCID, "RadarOn"));
                }
            }
            else insim->SendMTC(UCID, msg->_(UCID, "2112"));
        }
    }

    if (Message.find("!kick") == 0 )
    {
        if (players[packet->UCID].Rank < 3)
            return;

		if(args.size() < 2)
			return;

        string user = args[1];

		if (players[ packet->UCID ].cop )
		{
			char str[96];

			insim->SendMST("/kick " + user);

            CCText(string(players[ packet->UCID ].UName) + " kicked " + string(user));

			tm *timeStruct = tools::GetLocalTime();

			sprintf(str, "%02d:%02d:%02d %s kicked %s", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec, players[ packet->UCID ].UName.c_str(), user.c_str());

			char log[MAX_PATH];
			sprintf(log, "%slogs/cop/kick(%d.%d.%d).txt", RootDir, timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);
			ofstream readf (log, ios::app);
			readf << str << endl;
			readf.close();
		}

    }

    if ( players[ packet->UCID ].Admin)
    {
		if(args.size() < 2)
			return;

		const char* param = args[1].c_str();

        if (Message.find("!cop_add", 8) == 0 )
        {
            if (strlen(param) > 0)
            {
                string query = string("SELECT * FROM police WHERE username = '") + param + string("';");

                DB_ROWS res = dbSelect(query);

                if ( res.size() > 0)
                {
                    DB_ROW arFields;
                    arFields["active"] = "Y";

                    dbUpdate("police", arFields, {"username",param});
                }
                else
                {
                    char query[MAX_PATH];
                    sprintf(query,"INSERT INTO police (username) VALUES ('%s')",param);
                    dbExec(query);

                }
            }
        }

        if (Message.find("!cop_del") == 0 )
        {
            if (strlen(param) > 0)
            {
                DB_ROW arFields;
                arFields["active"] = "N";

                dbUpdate("police", arFields, {"username",param});
            }
        }

        if (Message.find("!cop_up") == 0 )
        {

            for (auto& p: players)
			{
                if (string(players[p.first].UName) == string(param))
                {
                    if (players[p.first].Rank < 4)
                    {
                        players[p.first].Rank++;
                        insim->SendMTC(255, "^2| ^8" + string(players[p.first].PName) + " ^2^Cповышен до ^7" + ToString(players[p.first].Rank) + " ^2ранга");
                        insim->SendBFNAll(p.first);
                    }
                    else
                        insim->SendMTC(UCID, "^2| ^8" + string(players[p.first].PName) + " ^7^Cдостиг максимального ранга");

                    return;
                }
			}
            insim->SendMTC(UCID, "^2| ^7^CИгрок не найден");
        }

        if (Message.find("!cop_down") == 0 )
        {
            for (auto& p: players)
			{
                if (string(players[p.first].UName) == string(param))
                {
                    if (players[p.first].Rank > 1)
                    {
                        players[p.first].Rank--;
                        insim->SendMTC(255, "^2| ^8" + string(players[p.first].PName) + " ^1^Cпонижен до ^7" + ToString(players[p.first].Rank) + " ^1ранга");
                        insim->SendBFNAll(p.first);
                    }
                    else
                        insim->SendMTC(UCID, "^2| ^8" + string(players[p.first].PName) + " ^7^Cдостиг минимального ранга");

                    return;
                }
            }
            insim->SendMTC(UCID, "^2| ^7^CИгрок не найден");
        }
    }
}

void RCPolice::RadarOff(byte UCID)
{
    ObjectInfo *obj = new ObjectInfo;
    obj->X = players[UCID].Radar.X;
    obj->Y = players[UCID].Radar.Y;
    obj->Zchar = players[UCID].Radar.Z;
    obj->Flags = 0;
    obj->Heading = 0;
    obj->Index = 22;
    DelObject(obj);
    obj->Index = 138;
    DelObject(obj);
    delete obj;
    players[UCID].Radar.On = false;
}

void RCPolice::CopPayRoll(byte UCID, bool FullWork = true)
{
    if (!players[UCID].cop)
        return;

    int TM = (time(NULL) - players[UCID].StartWork)%1800;

    if (FullWork and TM != 0)
        return;

    if (TM < 30 and !FullWork)
        return;

    if (TM == 0 and players[UCID].DoneCount == 0)
    {
        players[UCID].DoneCount = -1;
        insim->SendMTC(UCID, msg->_(UCID,"2113"));
        return;
    }

    if (players[UCID].DoneCount > 10)
        players[UCID].DoneCount = 10;

    if (players[UCID].DoneCount < 0)
        players[UCID].DoneCount = 0;

    if (players[UCID].DoneCount == 0)
        return;

    int Cash = players[UCID].DoneCount * (500 + 250 * players[UCID].Rank);
    char str[96];

    if (FullWork)
    {
        sprintf(str, "^2| ^7^CВаш заработок за прошедшую смену: %d ^3RUR", Cash);
    }
    else
    {
        Cash = Cash / 2;
        players[UCID].DoneCount = players[UCID].DoneCount / 2;

        sprintf(str, "^2| ^7^CВы не дождались конца смены, заработок: %d ^3RUR", Cash);
    }

    insim->SendMTC(UCID, str);

    bank->RemFrBank(Cash);
    bank->AddCash(UCID, Cash, false);

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

    players[UCID].DoneCount = -1;
}

void RCPolice::ShowFinesPanel( byte UCID, byte UCID2 )
{
    insim->SendBFN(UCID, 79);
    insim->SendBFN(UCID, 80);

    char Text[128];
    byte c = GetFineCount();

    byte
    id=180, 				//стартовый ид кнопок
    id2=205,
    l=100, 				//не менять
    t=90,				//не менять
    hButton=5, 			//высота строки
    w=100, 				//ширина поля
    h=10 + c * hButton; 	//высота поля

    sprintf(Text, "^C^7Панель выписки штрафов (^8%s^7)", players[UCID2].PName.c_str());

    insim->SendButton(255, UCID, 176, l - w / 2, t - h / 2, w, h + 8, 32, ""); 				    //фон
    insim->SendButton(255, UCID, 177, l - w / 2, t - h / 2, w, h + 8, 32, "");				    //фон
    insim->SendButton(255, UCID, 178, l - w / 2, t - h / 2, w, 10, 64, Text); 				    //заголовок
    insim->SendButton(254, UCID, 179, l - 7, t - h / 2 + h + 1, 14, 6, 16 + ISB_CLICK, "^2OK"); //закрывашка

    if ( players[UCID].Admin or (players[UCID].Rank > 2 and players[UCID2].Pogonya == 2))
        insim->SendButton(UCID2, UCID, 80, l + w / 2 - 15, t - h / 2 + h + 1, 14, 6, 16 + 8 + 5, "^CАрестовать", 2);

	if (players[UCID].Rank > 1 and players[UCID2].Pogonya == 1 and players[UCID2].Info.Speed * 360 / 32768 == 0)
		insim->SendButton(UCID2, UCID, 79, l - w / 2 + 1, t - h / 2 + h + 1, 20, 6, 16 + 8 + 5, "^CСошел с трассы?", 1);

    for (int i = 0; i < c; i++)
    {
        byte fid = fines[i + 1].id;
        if (fid == 0)
            fid = 255;

        int st = 7;

        if (players[UCID2].Pogonya == 2 or players[UCID].DTPstatus == 2 or players[UCID].Rank == 4)
        {
            for (int i=0; i < MAX_FINES; i++)
                if (FineAllow[players[UCID].Rank][i] == fid)
                {
                    st = 8 + 3;
                    break;
                }
        }
        else
        {
            if (fid == 14 or fid == 17 or fid == 20 or fid == 21)
                st = 8 + 3;
            if (players[UCID].Rank == 3 and (fid == 16 or fid == 1 or fid == 2))
                st = 8 + 3;
        }

        sprintf(Text, "^2%02d. ^8%s", fid, GetFineName(UCID, i + 1));

        insim->SendButton(UCID2, UCID, id+fid, l - w / 2 + 1, t - h / 2 + 10 + hButton * i, w - 12, hButton, 16 + 64 + st, Text);
        insim->SendButton(UCID2, UCID, id2+fid, l + w / 2 - 11, t - h / 2 + 10 + hButton * i, 10, hButton, 16 + st, "^CОтмена");
    }
}

void RCPolice::InsimBTC( struct IS_BTC* packet )
{
    if ( packet->ClickID == 178 and packet->ReqI == 254 )
    {
        players[packet->UCID].ThisFineCount = 0;

        for (int i=176; i < 239; i++)
            insim->SendBFN(packet->UCID, i);

        for (int j=0; j < 20; j++)
            strcpy(players[packet->UCID].ThisFine[j], "");

		CopUname = "";
    }

    if ( packet->ClickID == 179 and packet->ReqI == 254 ) //погоня офф
        for(int i=80; i < 239; i++)
            insim->SendBFN(packet->UCID, i);

    if ( packet->ClickID > 180 and packet->ClickID <= 205 and packet->ReqI != 255) //выписка штрафа
    {
        tm *timeStruct = tools::GetLocalTime();

        char fine_c[255];
        sprintf(fine_c, "%slogs/fines/fine(%d.%d.%d).txt", RootDir, timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);

        int FID = packet->ClickID-180;

        if (players[packet->UCID].DTPstatus == 2)
        {
            players[packet->ReqI].blame = true;
            if (FID != 14 and FID != 16 and FID != 17 and FID != 20 and FID != 21)
                players[packet->UCID].FineC += fines[FID].cash;
        }

        players[packet->UCID].PStat.FinedByDay++;
        players[packet->UCID].PStat.Fined++;

        char Msg[128];
        sprintf(Msg, msg->_(packet->ReqI, "GiveFine"), players[packet->UCID].PName.c_str(), GetFineName(packet->ReqI, FID));
        insim->SendMTC( packet->ReqI, Msg);

        sprintf(Msg, msg->_(packet->UCID , "AddFine" ), players[packet->ReqI].PName.c_str(), GetFineName(packet->UCID, FID));
        insim->SendMTC( packet->UCID , Msg);

        for (int j=0; j < MAX_FINES; j++)
        {
            if (players[ packet->ReqI ].fines[j].fine_id == 0)
            {
                players[ packet->ReqI ].fines[j].fine_id = FID;
                players[ packet->ReqI ].fines[j].fine_date = int( time( NULL ) );
                players[ packet->ReqI ].fines[j].CopName = players[ packet->UCID ].UName;
                players[ packet->ReqI ].fines[j].CopPName = players[ packet->UCID ].PName;
                break;
            }
        }

        if (players[packet->ReqI].Pogonya == 2 or players[packet->UCID].DTPstatus == 2)
            if (FID != 14 and FID != 16 and FID != 17 and FID != 20 and FID != 21)
                players[packet->UCID].DTPfines++;

        // sprintf(Msg, "%02d:%02d:%02d %s add fine ID = %d for %s", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec, players[ packet->UCID ].UName.c_str(), FID, players[ packet->ReqI ].UName.c_str());

       /* ofstream readf (fine_c, ios::app);
        readf << Msg << endl;
        readf.close();*/

        if (players[packet->ReqI].ThisFineCount == 0)
            insim->SendBFNAll(packet->ReqI);

        for (int j=0; j < 20; j++)
        {
            if ( strlen(players[packet->ReqI].ThisFine[j]) == 0 )
            {
                sprintf(players[packet->ReqI].ThisFine[j], "^7%d. %s (^2ID = %d^7)   -   %s", j + 1, GetFineName(packet->ReqI, FID), FID, players[ packet->UCID ].PName.c_str());
                players[packet->ReqI].ThisFineCount++;
                break;
            }
        }
    }

    if ( packet->ClickID > 205 and packet->ClickID <= 239 and packet->ReqI != 255) //отмена штрафа
    {
        tm *timeStruct = tools::GetLocalTime();

        char fine_c[255];
        sprintf(fine_c, "%slogs/fines/fine(%d.%d.%d).txt", RootDir, timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);

        int FID = packet->ClickID-205;
        if (players[packet->UCID].DTPstatus == 2)
            if (FID != 14 and FID != 16 and FID != 17 and FID != 20 and FID != 21)
                players[packet->UCID].FineC -= fines[FID].cash;

        for (int j=0; j < MAX_FINES; j++)
        {
            if ( players[ packet->ReqI ].fines[j].fine_id == FID )
            {
                players[packet->UCID].PStat.CanceledFinesByDay++;
                players[packet->UCID].PStat.CanceledFines++;

                if (players[packet->ReqI].Pogonya == 2 or players[packet->UCID].DTPstatus == 2)
                    players[packet->UCID].DTPfines--;

                char Msg[128];
                sprintf(Msg, msg->_( packet->ReqI, "DeletedFine"), players[packet->UCID].PName.c_str(), GetFineName(packet->ReqI, FID));
                insim->SendMTC( packet->ReqI , Msg);

                sprintf(Msg, msg->_(packet->UCID, "DelFine"), players[ packet->ReqI ].PName.c_str(), GetFineName(packet->UCID, FID));
                insim->SendMTC( packet->UCID , Msg);

                players[ packet->ReqI ].fines[j].fine_id = 0;
                players[ packet->ReqI].fines[j].fine_date = 0;

                sprintf(Msg, "%02d:%02d:%02d %s cancel fine ID = %d for %s", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec, players[ packet->UCID ].UName.c_str(), FID, players[ packet->ReqI ].UName.c_str());

                ofstream readf (fine_c, ios::app);
                readf << Msg << endl;
                readf.close();

                if (players[packet->ReqI].ThisFineCount > 0)
                {
                    for (int j=0; j < 20; j++)
                    {
                        if ( strlen(players[packet->ReqI].ThisFine[j]) == 0 )
                        {
                            sprintf(players[packet->ReqI].ThisFine[j], "   ^1^KЎї ^7%s (^2ID = %d^7)   -   %s", GetFineName(packet->ReqI, FID), fines[FID].id, players[ packet->UCID ].PName.c_str());
                            players[packet->ReqI].ThisFineCount++;
                            break;
                        }
                    }
                }
                break;
            }
        }
    }

    /** Штрафы **/
    if (packet->ClickID == 73 and players[packet->UCID].cop)
        ShowFinesPanel(packet->UCID, packet->ReqI);

    /** Включаем погоню **/
    if (packet->ClickID == 74)
    {
        if ( players[packet->ReqI].UName.length() == 0 )
            return;

        if (players[packet->UCID].DTPstatus <= 0)
        {
            if (players[packet->ReqI].Pogonya == 0)
            {
            	players[packet->UCID].FineC = 0;
                players[packet->UCID].DTPfines = 0;
                //players[packet->UCID].DTPstatus = -1;
                players[packet->ReqI].Pogonya = 1;
                int worktime = time(NULL);
                players[packet->ReqI].WorkTime = worktime + 60 * 5;
                char Text[96];
                sprintf(Text, msg->_(packet->ReqI, "PogonyaOn" ), players[packet->ReqI].PName.c_str(), players[packet->UCID].PName.c_str() );
                insim->SendMTC(255, Text);
                nrg->Lock(packet->ReqI);
            }
        }
        else
        {
            insim->SendMTC(packet->UCID, msg->_(packet->UCID,"2119"));
        }

        return;
    }

    /** Выключаем погоню **/
    if (packet->ClickID == 75)
    {
        if ( players[packet->ReqI].UName.length() == 0)
            return;

        if (players[packet->ReqI].Pogonya != 0)
        {
        	players[packet->UCID].FineC = 0;

            //запралата копам, которые были рядом во время ареста
            if (players[packet->UCID].DTPfines > 0)
            {
            	players[packet->UCID].PStat.ArrestWithFineByDay++;
            	players[packet->UCID].PStat.ArrestWithFine++;
                players[packet->UCID].DoneCount++;
                players[packet->UCID].DTPfines = 0;
                for (auto& p: players)
                {
                    if (players[p.first].cop and p.first != packet->UCID and players[p.first].Rank > 1)
                    {
                        int X1 = players[packet->ReqI].Info.X / 65536,
                            Y1 = players[packet->ReqI].Info.Y / 65536,
                            X2 = players[p.first].Info.X / 65536,
                            Y2 = players[p.first].Info.Y / 65536,
                            Dist2 = Distance(X1, Y1, X2, Y2);

                        if (Dist2 < 30)
                        {
                            insim->SendMTC(p.first, "^2| ^7^CВы получили прибавку к зарплате за содействие при аресте");

                            players[p.first].PStat.HelpArrest++;
                            players[p.first].PStat.HelpArrestByDay ++;

                            players[p.first].DoneCount++;
                            players[p.first].DTPfines = 0;
                        }
                    }
                }
            }
            else
            {
				players[packet->UCID].PStat.ArrestWithOutFineByDay++;
            	players[packet->UCID].PStat.ArrestWithOutFine++;
            }

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
            sprintf(Text, msg->_(packet->ReqI, "PogonyaOff" ), players[packet->ReqI].PName.c_str(), players[packet->UCID].PName.c_str() );
            insim->SendMTC(255, Text);
            nrg->Unlock(packet->ReqI);

            //закрывашка штрафов
            for (byte c = 80; c < 165; c++)
                insim->SendBFN(packet->UCID, c);

            //очистка кнопок
            for (byte i = 101; i < 110; i++)
                insim->SendBFN(255, i);
        }
        return;
    }
}

void RCPolice::InsimBTT( struct IS_BTT* packet )
{
    tm *timeStruct = tools::GetLocalTime();

    char fine_c[255];
    sprintf(fine_c, "%slogs/fines/fine(%d.%d.%d).txt", RootDir, timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);

    /** нажатие на заявку */
    if (packet->ReqI == 200)
    {
        int i = packet->ClickID - 91;
        char str[96];

        if (players[packet->UCID].DTPstatus == 2 and DTPvyzov[2][i] == packet->UCID)
        {
            sprintf(str, msg->_(DTPvyzov[0][i],"2114"), players[packet->UCID].PName.c_str());
            insim->SendMTC(DTPvyzov[0][i], str);

            sprintf(str, msg->_(packet->UCID,"2115"), players[DTPvyzov[0][i]].PName.c_str());
            insim->SendMTC(packet->UCID, str);

            players[DTPvyzov[0][i]].DTP = 0;
            if (DTPvyzov[2][i] != 0)
                players[DTPvyzov[2][i]].DTPstatus = 0;

            if (players[packet->UCID].DTPfines > 0)
			{
                players[packet->UCID].DoneCount++;
                players[packet->UCID].PStat.SolvedIncedentsByDay++;
                players[packet->UCID].PStat.SolvedIncedents++;
			}
			players[packet->UCID].DTPfines = 0;

			//выплата компенсации в виде % от штрафов
			if (!players[DTPvyzov[0][i]].blame and players[packet->UCID].FineC>0)
			{
				sprintf(str, msg->_(DTPvyzov[0][i],"Compens"), (double)((double)players[packet->UCID].FineC*0.05));
				insim->SendMTC(DTPvyzov[0][i], str);
				sprintf(str, msg->_(packet->UCID,"CompensPl"), players[DTPvyzov[0][i]].PName.c_str(), (double)((double)players[packet->UCID].FineC*0.05));
				insim->SendMTC(packet->UCID, str);
				bank->RemFrBank(players[packet->UCID].FineC*0.05);
				bank->AddCash(DTPvyzov[0][i], (players[packet->UCID].FineC*0.05), true);

				tm *timeStruct = tools::GetLocalTime();

                sprintf(str, "%02d:%02d:%02d %s get compensation %0.0f from %s", timeStruct->tm_hour, timeStruct->tm_min, timeStruct->tm_sec, players[DTPvyzov[0][i]].UName.c_str(), (double)((double)players[packet->UCID].FineC*0.05), players[packet->UCID].UName.c_str());

                char log[MAX_PATH];
                sprintf(log, "%slogs/cop/compens(%d.%d.%d).txt", RootDir, timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);
                ofstream readf (log, ios::app);
                readf << str << endl;
                readf.close();
			}

			players[packet->UCID].FineC = 0;
			players[DTPvyzov[0][i]].blame = false;

            DTPvyzov[0][i] = 0;
            DTPvyzov[1][i] = 0;
            DTPvyzov[2][i] = 0;
            insim->SendBFN(255, packet->ClickID);

            //закрывашка штрафов
            for (byte c = 176; c < 239; c++)
                insim->SendBFN(packet->UCID, c);

            //очистка кнопок
            for (byte i = 91; i < 110; i++)
                insim->SendBFN(255, i);
        }
        else if (DTPvyzov[2][i] <= 0)
        {
            if (players[packet->UCID].DTPstatus > 0)
            {
                insim->SendMTC(packet->UCID, msg->_(packet->UCID,"2116"));
                return;
            }

            DTPvyzov[2][i] = packet->UCID;
            DTPvyzov[1][i] = 0;

            sprintf(str, msg->_(packet->UCID,"2117"), players[packet->UCID].PName.c_str());
            insim->SendMTC(DTPvyzov[0][i], str);

            sprintf(str, msg->_(packet->UCID,"2118"), players[packet->UCID].PName.c_str(), players[DTPvyzov[0][i]].PName.c_str());
            SendMTCToCop(str, 1, 2, 4);

            players[DTPvyzov[2][i]].DTPstatus = 1;
            players[DTPvyzov[2][i]].DTPfines = 0;
            players[DTPvyzov[2][i]].FineC = 0;
            players[DTPvyzov[0][i]].DTP = packet->UCID;
        }
    }

    /** Игрок в погоне сошел с трассы **/
    if ( packet->ClickID == 79)
    {
        players[packet->ReqI].Pogonya = 2;
        ShowFinesPanel(packet->UCID, packet->ReqI);
        insim->SendBFN(packet->UCID, 79);

        char Text[128];
        sprintf(Text, "^2| ^C^7Сход с трассы во время погони");
        insim->SendMTC(packet->ReqI, Text);
        sprintf(Text, "^2| %s%s", players[packet->ReqI].PName.c_str(), msg->_(packet->ReqI, "1702"));
        insim->SendMTC(255, Text);
        insim->SendMTC(packet->UCID, msg->_(packet->UCID, "1703"));
    }

    /** Арест на Х минут **/
    if ( packet->ClickID == 80)
    {
        if (strlen(packet->Text) == 0)
            return;

        players[packet->ReqI].Pogonya = 0;
        nrg->Unlock(packet->ReqI);

        //закрывашка штрафов
        for (byte c = 176; c < 239; c++)
            insim->SendBFN(packet->UCID, c);

        //очистка кнопок
        for (byte i = 81; i < 175; i++)
            insim->SendBFN(255, i);

        time_t now = time(NULL);
        char str[96];

        int MAXmin = 0;
        if (players[packet->UCID].Rank == 3)
            MAXmin = 30;

        if (players[packet->UCID].Rank == 4)
            MAXmin = 60;

        int Min = atoi(packet->Text);
        if (Min > MAXmin)
            Min = MAXmin;
        else if (Min < 5 and Min > 0)
            Min = 5;

        if (Min > 0)
        {
            insim->SendMST("/spec " + players[packet->ReqI].UName);

            sprintf(str, msg->_(packet->ReqI, "ArestedMin"), players[packet->ReqI].PName.c_str(), Min);

            insim->SendMTC(255, str);

            ArestPlayers[ players[packet->ReqI].UName ] = now + Min * 60;
        }
        else if ( ArestPlayers.find(  players[packet->ReqI].UName ) != ArestPlayers.end() )
        {
            ArestPlayers.erase(  players[packet->ReqI].UName );

            sprintf(str, msg->_(packet->ReqI, "Reliaved"), players[packet->ReqI].PName.c_str());
            insim->SendMTC(255, str);
            insim->SendBFN(packet->ReqI, 85);
            insim->SendBFN(packet->ReqI, 86);
        }
    }
}

void RCPolice::InsimPEN( struct IS_PEN* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];

    if (packet->Reason == PENR_WRONG_WAY )
    {
        for (int j=0; j < MAX_FINES; j++)
        {
            if (players[ UCID ].fines[j].fine_id == 0)
            {
                players[ UCID ].fines[j].fine_id = 18;
                players[ UCID ].fines[j].fine_date = int(time(NULL));

                char Msg[96];
                sprintf(Msg, msg->_( UCID , "GiveFine2" ), GetFineName(UCID, 18));
                insim->SendMTC( UCID , Msg);
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
            for (int j=0; j < MAX_FINES; j++)
            {
                if (players[ UCID ].fines[j].fine_id == 0)
                {
                    players[ UCID ].fines[j].fine_id = 13;
                    players[ UCID ].fines[j].fine_date = int(time(NULL));

                    char Msg[64];
                    sprintf(Msg, msg->_( UCID , "GiveFine2" ), GetFineName(UCID, 13));
                    insim->SendMTC( UCID , Msg);
                    break;
                }
            }
        }
    }
}

void RCPolice::InsimOBH( struct IS_OBH* packet )
{
    if (packet->Index == 22 || packet->Index == 138)
    {
        for (auto& play: players)
        {
            byte UCID = play.first;

            if (players[UCID].Radar.On && players[UCID].Radar.X == packet->X && players[UCID].Radar.Y == packet->Y)
            {
                //CCText("^2 !!!! " + ToString(packet->X) + " - " + ToString(packet->Y));

                insim->SendMTC(UCID, "^2| ^C^7Радар ^1сломан");
                RadarOff(UCID);
            }
        }
    }
}

void RCPolice::InsimPLA( struct IS_PLA* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];

    if (packet->Fact == PITLANE_EXIT)
    {

        insim->SendMST("/p_clear " + players[ UCID ].UName);

        int count = 0;
        for (int j=0; j < MAX_FINES; j++)
        {
            if (players[ UCID ].fines[j].fine_id != 0)
            {
                count++;
            }
        }

        if (count > 10)
        {
            insim->SendMTC( UCID , msg->_(  UCID , "3400" ));
            insim->SendMST("/pitlane " + players[ UCID ].UName);
        }
    }
}

void RCPolice::InsimMCI( struct IS_MCI* packet )
{
    for (int i = 0; i < packet->NumC; i++)
    {
        byte UCID = PLIDtoUCID[packet->Info[i].PLID];

        if (UCID == 0)
            return;

        int SirenaCount = 0;
        int SDtemp=0;

        int S = packet->Info[i].Speed * 360 / 32768;

        /** окошко со штрафами **/
        if ( players[UCID].ThisFineCount != 0 )
        {
            byte ClickID=180, w=90, h=10 + 5 * players[UCID].ThisFineCount, l=100, t=90;
            insim->SendButton(255, UCID, 176, l - w / 2, t - h / 2, w, h + 8, 32, "");
            insim->SendButton(255, UCID, 177, l - w / 2, t - h / 2, w, h + 8, 32, "");									//фон
            insim->SendButton(254, UCID, 178, l - 7, t - h / 2 + h + 1, 14, 6, 16 + 8, "^2OK"); 		    //закрывашка
            insim->SendButton(255, UCID, 179, l - w / 2, t - h / 2, w, 10, 3 + 64, msg->_(UCID, "GiveFine3")); 			//заголовок

            for (int j=0; j < players[UCID].ThisFineCount; j++)
                insim->SendButton(255, UCID, ClickID++, l - w / 2 + 1, t - h / 2 + 10 + 5 * j, w - 2, 5, ISB_LEFT + ISB_LIGHT, players[UCID].ThisFine[j]);
        }

        /** автоотключение радара **/
        if (players[UCID].Radar.On && Distance(players[UCID].Radar.X, players[UCID].Radar.Y, players[UCID].Info.X/4096, players[UCID].Info.Y/4096) > 150)
        {
            RadarOff(UCID);
            insim->SendMTC(UCID, msg->_(UCID, "RadarOff"));
        }

        /** сирена копа **/
        if (players[UCID].cop and players[UCID].Sirena)
            insim->SendButton(255, UCID, 81, 90, 26, 20, 10, 32, siren.c_str());
        else if (players[UCID].cop)
            insim->SendBFN(UCID, 81);

        for (auto& play: players)
        {
            if (UCID == play.first)
            {
                continue;
            }

            int X1 = players[UCID].Info.X / 65536,
                Y1 = players[UCID].Info.Y / 65536,
                X2 = players[play.first].Info.X / 65536,
                Y2 = players[play.first].Info.Y / 65536,
                Dist = Distance(X1, Y1, X2, Y2);

            /** арест **/
            if (players[UCID].Pogonya == 1 and players[play.first].cop and players[play.first].Rank != 1)
            {
                if (S < 5 and Dist < 10)
                {
                    players[UCID].StopTime++;
                    if (players[UCID].StopTime >= 4)
                    {
                        players[UCID].StopTime = 0;
                        players[UCID].Pogonya = 2;
                        char Text[128];
                        sprintf(Text, "^2| %s%s", players[UCID].PName.c_str(), msg->_(UCID, "1702"));
                        insim->SendMTC(255, Text);
                        insim->SendMTC(play.first, msg->_(play.first, "1703"));
                    }
                }
                if (S > 5)
                    players[UCID].StopTime = 0;
            }

            /** сирена у игрока **/
            if (players[play.first].Sirena and players[play.first].cop and Dist < 180)
            {
                SirenaCount++;
                if (Dist < SDtemp or SDtemp ==0)
                    SDtemp = Dist;
            }
            players[UCID].SirenaDist = SDtemp;

            /** заявка на дтп **/
            if (players[UCID].DTP != 0 and play.first == players[UCID].DTP and Dist < 30 and players[play.first].DTPstatus == 1 and players[play.first].Info.Speed * 360 / 32768 < 5)
            {
                players[play.first].DTPstatus = 2;
                players[UCID].DTP = 0;
            }

            /** радар **/
            if (!players[UCID].cop and players[UCID].Pogonya == 0 and players[play.first].cop and players[play.first].Radar.On)
            {
                Dist = Distance(X1, Y1, players[play.first].Radar.X/16, players[play.first].Radar.Y/16);
                if (Dist < 50)
                {
                    struct streets StreetInfo;
                    street->CurentStreetInfo(&StreetInfo, UCID);

                    if (Dist < 25)
                    {
                        int Speed = players[UCID].Info.Speed * 360 / 32768;


                        if (Speed > StreetInfo.SpeedLimit + 10 and Speed > players[UCID].speed_over)
                            players[UCID].speed_over = Speed - StreetInfo.SpeedLimit;

                    }
                    else if (players[UCID].speed_over > 0)
                    {
                        char text[128];

                        sprintf(text, msg->_(UCID, "Speeding"), players[UCID].PName.c_str(), players[UCID].speed_over, street->GetStreetName(UCID, StreetInfo.StreetID), players[play.first].PName.c_str());
                        insim->SendMTC(255, text);

                        if (players[UCID].Pogonya == 0)
                        {
                            players[UCID].Pogonya = 1;
                            int worktime = time(NULL);
                            players[UCID].WorkTime = worktime + 60 * 5;
                            sprintf(text, msg->_(UCID, "PogonyaOn" ), players[UCID].PName.c_str(), players[play.first].PName.c_str() );
                            insim->SendMTC(255, text);
                            nrg->Lock( UCID );
                        }
                        players[UCID].speed_over=0;
                    }
                }
                else if (players[UCID].speed_over>0)
                    players[UCID].speed_over=0;
            }
        }

        /** сирена у игрока **/
        if (SirenaCount > 0)
        {
            if (players[UCID].cop)
                insim->SendButton(255, UCID, 82, 0, 36, 200, (byte)((180 - players[UCID].SirenaDist + 4) / 10)+1, 0, siren.c_str());
            else
                insim->SendButton(255, UCID, 82, 0, 36, 200, (180 - players[UCID].SirenaDist + 4) / 4, 0, siren.c_str());
        }
        else if (players[UCID].SirenaDist > 0 or SirenaCount == 0)
        {
            players[UCID].SirenaDist = 0;
            insim->SendBFN(UCID, 82);
        }

        if (players[UCID].Pogonya == 0)
        {
            insim->SendBFN(UCID, 83);
            insim->SendBFN(UCID, 84);
            insim->SendBFN(UCID, 85);
        }

        if (players[UCID].Pogonya != 0)
        {
            BtnPogonya(UCID);
        }

        players[UCID].Info = packet->Info[i];
    }
}

void RCPolice::ReadConfig(const char* Track)
{
    ReadFines();

    char file[MAX_PATH];
    sprintf(file, "%s/data/RCPolice/FineAllow.txt", RootDir);

    ifstream readf (file, ios::in);

    if (!readf.is_open())
    {
    	CCText("  ^7RCPolice   ^1ERROR: ^8file " + string(file) + " not found");
        return;
    }

    while (readf.good())
    {
        char line[128];
        readf.getline(line, 128);

        if(strlen(line) <= 1)
            continue;

        char str[128];
        sprintf(str, strtok (line, ";"));

        if (strlen(str) > 1)
        {
            int rnk = atoi(strtok(str, "="));

            if (rnk == 0 or rnk > 4)
                continue;

            for (int i=0; i < MAX_FINES; i++)
            {
                char* cc = strtok(NULL, ",");
                int c = 0;
                if( cc != nullptr )
                    c = atoi(cc);

                if (c == 0)
                    break;

                FineAllow[rnk][i] = c;
            }
        }
    }
    readf.close();

    CCText("  ^7RCPolice\t^2OK");
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
        if (players[play.first].cop and players[play.first].Rank !=3)
            c++;
    return c;
}

int RCPolice::GetFineCount()
{
    int c=0;
    for (int i=0; i < MAX_FINES; i++)
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
        insim->SendButton(255, UCID, 83, 0, 20, 200, 30, 1, msg->_(UCID, "RideButton" ));
        insim->SendButton(255, UCID, 84, 0, 43, 200, 6, 0, msg->_(UCID, "RightAndStop" ));
    }
    else if (players[UCID].Pogonya == 2)
    {
        insim->SendButton(255, UCID, 83, 0, 20, 200, 30, 1, msg->_(UCID, "ArestButton"));
        insim->SendBFN(UCID, 84);
    }
}

void RCPolice::Save ( byte UCID )
{
    if (players[UCID].cop)
        SaveCopStat(UCID);

    //cout <<players[ UCID ].UName << " save fines_info" << endl;

    char file[255];
    sprintf(file, "%s/data/RCPolice/fines/%s.txt", RootDir, players[ UCID ].UName.c_str());

    ofstream writef (file, ios::out);
    for (int i = 0; i < MAX_FINES; i++)
        if (players[ UCID ].fines[i].fine_id > 0)
            writef << players[ UCID ].fines[i].fine_id << ";" << players[ UCID ].fines[i].fine_date << ";" << players[ UCID ].fines[i].CopName << ";" << players[ UCID ].fines[i].CopPName <<  endl;

    writef.close();
}

void RCPolice::ReadUserFines( byte UCID )
{

    char file[255];
    sprintf(file, "%s/data/RCPolice/fines/%s.txt", RootDir, players[ UCID ].UName.c_str());

    ifstream readf (file, ios::in);

    if (!readf.is_open())
    {
        cout << "Can't find " << file << endl;
        Save( UCID );
    }
    else
    {
        int i=0;
        while (readf.good())
        {
            char str[128];
            readf.getline(str, 128);

            if (strlen(str) > 1)
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
                    players[ UCID ].fines[i].fine_date = atoi(date);

                if ( CopName )
                    players[ UCID ].fines[i].CopName = CopName;

                if ( CopPName )
                    players[ UCID ].fines[i].CopPName = CopPName;

                i++;
            }
        }
    }
}

void RCPolice::ReadFines()
{
    char file[255];
    strcpy(file, RootDir);
    sprintf(file, "%s/data/RCPolice/fines.txt" , RootDir);

    ifstream readf (file, ios::in);

    if (!readf.is_open())
    {
        cout << "Can't find " << file << endl;
        return;
    }

    int i = 0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);
        if (strlen(str) > 1)
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
    sprintf(str, "^2%02d:%02d", TIME / 60, TIME%60);

    insim->SendButton(255, UCID, 85, 130, 1, 10, 8, 32, str);
}

char* RCPolice::GetFineName(byte UCID, int FineID)
{
    if (msg->GetLang(UCID) == "eng")
        return fines[FineID].NameEn;
    else
        return fines[FineID].NameRu;
}

void RCPolice::SendMTCToCop(const char* Msg, int Rank, ...)
{
    int *p=&Rank;
    while (*p)
        for ( auto& play: players )
            if (players[play.first].cop and players[play.first].Rank == *p++)
                insim->SendMTC(play.first, Msg);
}

bool
RCPolice::LoadCopStat(byte UCID)
{

    string query = "SELECT * FROM police WHERE username = '";
    query += players[UCID].UName;
    query += "';";

    DB_ROWS res = dbSelect(query);

    if ( res.size() > 0)
    {
        DB_ROW row = res.front();

        if (row["active"] == "N")
        {
        	return false;
        }

		players[UCID].Rank = atoi( row["rank"].c_str() );

        players[UCID].PStat.DateActive = atoi( row["date_active"].c_str() );
        players[UCID].PStat.CurrentDay = atoi( row["current_day"].c_str() );
        players[UCID].PStat.ArrestWithFineByDay = atoi( row["arrests_with_fine_by_day"].c_str() );
        players[UCID].PStat.ArrestWithOutFineByDay = atoi( row["arrests_without_fine_by_day"].c_str() );
        players[UCID].PStat.HelpArrestByDay = atoi( row["helparrest_by_day"].c_str() );
        players[UCID].PStat.SolvedIncedentsByDay = atoi( row["solved_accidents_by_day"].c_str() );
        players[UCID].PStat.FinedByDay = atoi( row["fined_by_day"].c_str() );
        players[UCID].PStat.CanceledFinesByDay = atoi( row["fines_canceled_by_day"].c_str() );
        players[UCID].PStat.ArrestWithFine = atoi( row["arrests_with_fine"].c_str() );
        players[UCID].PStat.ArrestWithOutFine = atoi( row["arrests_without_fine"].c_str() );
        players[UCID].PStat.HelpArrest = atoi( row["helparrest"].c_str() );
        players[UCID].PStat.SolvedIncedents = atoi( row["solved_accidents"].c_str() );
        players[UCID].PStat.Fined = atoi( row["fined"].c_str() );
        players[UCID].PStat.CanceledFines = atoi( row["fines_canceled"].c_str() );

    }
    else
		return false;

    return true;
}

void RCPolice::SaveCopStat(byte UCID)
{

    DB_ROW arFields;
    arFields["rank"] = ToString(players[UCID].Rank);
    arFields["date_active"] = ToString(players[UCID].PStat.DateActive);
    arFields["current_day"] = ToString(players[UCID].PStat.CurrentDay);
    arFields["arrests_with_fine_by_day"] = ToString(players[UCID].PStat.ArrestWithFineByDay);
    arFields["arrests_without_fine_by_day"] = ToString(players[UCID].PStat.ArrestWithOutFineByDay);
    arFields["helparrest_by_day"] = ToString(players[UCID].PStat.HelpArrestByDay);
    arFields["solved_accidents_by_day"] = ToString(players[UCID].PStat.SolvedIncedentsByDay);
    arFields["fined_by_day"] = ToString(players[UCID].PStat.FinedByDay);
    arFields["fines_canceled_by_day"] = ToString(players[UCID].PStat.CanceledFinesByDay);
    arFields["arrests_with_fine"] = ToString(players[UCID].PStat.ArrestWithFine);
    arFields["arrests_without_fine"] = ToString(players[UCID].PStat.ArrestWithOutFine);
    arFields["helparrest"] = ToString(players[UCID].PStat.HelpArrest);
    arFields["solved_accidents"] = ToString(players[UCID].PStat.SolvedIncedents);
    arFields["fined"] = ToString(players[UCID].PStat.Fined);
    arFields["fines_canceled"] = ToString(players[UCID].PStat.CanceledFines);

    bool up = dbUpdate("police", arFields, {"username",players[UCID].UName});

    if ( !up )
    {
        CCText("^1RCPolice: " + (string)players[UCID].UName + " SaveStat error");
        insim->SendMTC(255, "^1RCPolice: " + (string)players[UCID].UName + " SaveStat error");
        return;
    }
}

void RCPolice::Event()
{
    for ( auto& play: players )
    {
        byte UCID = play.first;
        auto playr = play.second;

        // Обработка арестованных игроков
        if( ArestPlayers.find( playr.UName ) != ArestPlayers.end() )
		{
			if ( time(NULL) > ArestPlayers[ playr.UName ] )
			{
				ArestPlayers.erase( playr.UName );

				char str[128];
				sprintf(str, msg->_(UCID, "Reliaved"), playr.UName.c_str() );
				insim->SendMTC(255, str);
				insim->SendBFN(UCID, 85);
				insim->SendBFN(UCID, 86);
			}
			else
			{
				char str[10];
				int TIME = ArestPlayers[ playr.UName ] - time(NULL);
				sprintf(str, "^1%02d:%02d", TIME / 60, TIME%60);
				insim->SendButton(255, UCID, 85, 130, 1, 10, 8, 32, str);
				insim->SendButton(255, UCID, 86, 120, 9, 20, 5, 32, msg->_(UCID, "YouUndArest"));
			}
		}

        if ( playr.Pogonya == 1 )
        {
            ButtonClock(UCID);
            int nowtime = time( NULL );
            if (playr.WorkTime <= nowtime)
            {
                insim->SendBFN(UCID, 85);

                char Text[64];
                sprintf( Text , "/msg ^2| %s %s" , playr.PName.c_str(), msg->_(  UCID , "1706" ) );

                insim->SendMST(Text);
                players[ UCID ].Pogonya = 0;
                nrg->Unlock( UCID );
                dl->AddSkill( UCID );

                //очистка кнопок
                for (byte i = 101; i < 110; i++)
                    insim->SendBFN(255, i);
            }
        }

        if (players[UCID].cop)
        {
            streets StreetInfo;

            CopPayRoll(UCID);

            char smn[32];
            int TM = 1800 - (time(NULL) - players[UCID].StartWork)%1800;
            sprintf(smn, "^CСмена: %02d:%02d", (TM / 60)%60, TM%60);
            insim->SendButton(255, UCID, 87, 115, 1, 15, 4, 3 + 128, smn);

            //заявки на дтп
            for (int i = 0; i < 32; i++)
            {
                if (DTPvyzov[0][i] >= 0 and DTPvyzov[1][i] >= 0 and players[UCID].DTPstatus != -1 and players[UCID].Rank != 3)
                {
                    street->CurentStreetInfo(&StreetInfo, DTPvyzov[0][i]);	// улица, где дтп
                    byte id = 91;											// стартовый ИД кнопок

                    if (DTPvyzov[2][i] <= 0 and DTPvyzov[1][i] > 0)
                    {
                        int T = DTPvyzov[1][i] - time(NULL); 				// сколько осталось до конца заявки

                        char str[96];
                        sprintf(str, "^C^1Вызов на ДТП - ^8%s^7, %s, ^1%02d:%02d", players[DTPvyzov[0][i]].PName.c_str(), StreetInfo.StreetRu, (T / 60)%60, T%60);
                        insim->SendButton(200, UCID, id + i, 159, 100 + 4 * i, 40, 4, 32 + 8, "\0^CПодтверди нажатие\0" + string(str), 1);

                        if (T <= 0)
                        {
                            players[DTPvyzov[0][i]].DTP = 0;

                            if (DTPvyzov[2][i] != 0)
                                players[DTPvyzov[2][i]].DTPstatus = 0;

                            DTPvyzov[0][i] = 0;
                            DTPvyzov[1][i] = 0;
                            DTPvyzov[2][i] = 0;
                            insim->SendBFN(255, id + i);

                            //штраф всем копам что профукали заявку
                            for (auto& play: players)
                            {
                                if (players[play.first].cop and players[play.first].Rank != 3 and players[play.first].DTPstatus == 0)
                                {
                                    insim->SendMTC(play.first, msg->_(play.first,"2120"));
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
                        }
                    }
                    else if (DTPvyzov[2][i] == UCID)
                    {
                        if (players[UCID].DTPstatus == 1)
                        {
                            char str[96];
                            sprintf(str, msg->_(UCID,"2121"), players[DTPvyzov[0][i]].PName.c_str(), StreetInfo.StreetRu);
                            insim->SendButton(200, UCID, id + i, 159, 100 + 4 * i, 40, 4, 32 + 3, str);
                        }
                        else
                        {
                            char str[96];
                            sprintf(str, msg->_(UCID,"2122"), players[DTPvyzov[0][i]].PName.c_str(), StreetInfo.StreetRu);
                            insim->SendButton(200, UCID, id + i, 159, 100 + 4 * i, 40, 4, 32 + 8, "\0^CПодтверди нажатие\0" + string(str), 1);
                        }
                    }
                    else if (DTPvyzov[0][i] > 0 and DTPvyzov[2][i] > 0)
                    {
                        char str[96];
                        sprintf(str, msg->_(UCID,"2123"), players[DTPvyzov[0][i]].PName.c_str(), StreetInfo.StreetRu);
                        insim->SendButton(200, UCID, id + i, 159, 100 + 4 * i, 40, 4, 32 + 7, str);
                    }
                }
            }

            /** Погоны */
            int id = 111,
                L = 58,
                H = 1;

            insim->SendButton(255, UCID, id++, L, H + 3, 6, 2, 0, "^3^KЎЬ");
            insim->SendButton(255, UCID, id++, L - 1, H - 1, 5, 10, 0, "^1^JЃs");
            insim->SendButton(255, UCID, id++, L - 1, H - 1, 14, 10, 0, "^1^J             Ѓb");
            insim->SendButton(255, UCID, id++, L - 0, H - 1, 12, 10, 0, "^1^JЃPЃPЃPЃP");
            insim->SendButton(255, UCID, id++, L - 0, H - 1, 12, 10, 0, "^1^JЃQЃQЃQЃQ");

            if (players[UCID].Rank == 1)
            {
                //мл. лейт.
                insim->SendButton(255, UCID, id++, L + 6, H + 2, 4, 4, 0, "^3^JЃљ");
            }
            else if (players[UCID].Rank == 2)
            {
                //лейт
                insim->SendButton(255, UCID, id++, L + 6, H + 1, 5, 3, 0, "^3^JЃљ");
                insim->SendButton(255, UCID, id++, L + 6, H + 4, 5, 3, 0, "^3^JЃљ");
            }
            else if (players[UCID].Rank == 3)
            {
                //ст. лейт
                insim->SendButton(255, UCID, id++, L + 4, H + 2, 5, 4, 0, "^3^JЃљ");
                insim->SendButton(255, UCID, id++, L + 6, H + 1, 5, 3, 0, "^3^JЃљ");
                insim->SendButton(255, UCID, id++, L + 6, H + 4, 5, 3, 0, "^3^JЃљ");
            }
            else if (players[UCID].Rank == 4)
            {
                //капитан
                insim->SendButton(255, UCID, id++, L + 2, H + 2, 6, 4, 0, "^3^JЃљ");
                insim->SendButton(255, UCID, id++, L + 5, H + 2, 5, 4, 0, "^3^JЃљ");
                insim->SendButton(255, UCID, id++, L + 7, H + 1, 5, 3, 0, "^3^JЃљ");
                insim->SendButton(255, UCID, id++, L + 7, H + 4, 5, 3, 0, "^3^JЃљ");
            }


            /** список погонь **/

            id = 101;
            byte T = 194;
            for ( auto& play2: players)
            {
                byte UCID2 = play2.first;
                auto playr2 = play2.second;
                if ( playr2.Pogonya != 0 )
                {
                    int time2 = playr2.WorkTime - time(NULL);
                    int min = (time2 / 60)%60;
                    int sec = time2 % 60;

                    float D = Distance( playr.Info.X, playr.Info.Y, playr2.Info.X, playr2.Info.Y) / 65536;

                    street->CurentStreetInfo(&StreetInfo, UCID2);

                    string str = StringFormat("%s^7 - %s, %0.0f ^Cм ^2(^1%02d:%02d^2)", playr2.PName.c_str(), StreetInfo.StreetRu, D, min, sec);

                    if (playr2.Pogonya == 2)
                        str = StringFormat(msg->_(UCID,"2124"), playr2.PName.c_str(), StreetInfo.StreetRu);

                    insim->SendButton(255, UCID, id++, 75, T, 50, 4, 32, str);
                    T -=4;
                }
            }
        }
    }
}
