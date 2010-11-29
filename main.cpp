#include "main.h"

#include "tools.h"

int ok = 1;
struct global_info ginfo;
RCPizza     pizza;
RCMessage   msg;
RCEnergy    nrg;
RCBank      bank;




void read_words()
{
    char file[255];
    strcpy(file,RootDir);
    strcat(file,"misc\\words.txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);
    int i=0;
    while (readf.good())
    {
        char str[32];
        readf.getline(str,32);

        if (strlen(str) > 0)
        {
            strncpy(ginfo.Words[i],str,strlen(str));
            i++;
        }
    }
    ginfo.WordsCount = i;

}

void save_user_cars (struct player *splayer)
{
    out <<splayer->UName << " save cars_info" << endl;

    char file[255];
    strcpy(file,RootDir);
    strcat(file,"data\\RCGarage\\");
    strcat(file,splayer->UName);
    strcat(file,".txt");

    ofstream writef (file,ios::out);
    for (int i = 0; i < MAX_CARS; i++)
    {
        if (strlen(splayer->cars[i].car)>0)
        {
            writef << splayer->cars[i].car << ";" << splayer->cars[i].tuning << ";" << splayer->cars[i].dist << endl;
        }
    }

    writef.close();
}

void save_user_fines (struct player *splayer)
{
    out <<splayer->UName << " save fines_info" << endl;

    char file[255];
    strcpy(file,RootDir);
    strcat(file,"data\\RCPolice\\fines\\");
    strcat(file,splayer->UName);
    strcat(file,".txt");

    ofstream writef (file,ios::out);
    for (int i = 0; i < MAX_FINES; i++)
    {
        if (splayer->fines[i].fine_id > 0)
        {
            writef << splayer->fines[i].fine_id << ";" << splayer->fines[i].fine_date <<  endl;
        }
    }

    writef.close();
}
/**
void save_user (struct player *splayer)
{
    out <<splayer->UName << " save user_info" << endl;

    char file[255];
    strcpy(file,RootDir);
    strcat(file,"data\\users\\");
    strcat(file,splayer->UName);
    strcat(file,".txt");

    ofstream writef (file,ios::out);
    writef << 1 << endl;
    writef << splayer->cash << ";"
    // << splayer->Energy << ";"
    << splayer->Cars << ";"
    << splayer->Lang << ";"
    << endl;
    writef.close();

    save_user_cars(splayer);
    save_user_fines(splayer);
}
**/
/**
void read_user (struct player *splayer)
{
    char file[255];
    strcpy(file,RootDir);
    strcat(file,"data\\users\\");
    strcat(file,splayer->UName);
    strcat(file,".txt");

    ifstream readf (file,ios::in);
    char key[2];
    readf.getline(key,2);
    if (atoi(key) > 0)
    {
        char data[128];
        readf.getline(data,128);
        int L2 = strlen(data);
        char *cash;
        char cashf[24];
        char *energy;
        char *car;
        char *lang;

        cash = strtok(data,";");
        energy = strtok(NULL,";");
        car = strtok(NULL,";");

        int L = strlen(cash) + strlen(energy) + strlen(car) + 3;
        //out << "L = " << L << "L2 = " << L2 << endl;
        if (L == L2)
            lang="rus";
        else
            lang = strtok(NULL,";");


        //memcpy(&cashf,&cash,strlen(cash));
        strcpy(cashf,cash);
        splayer->cash = atof(cashf);
        //splayer->Energy = atoi(energy);
        strcpy(splayer->Cars,car);
        strcpy(splayer->Lang,lang);
    }
    else
    {
        splayer->cash = 1000;
        //splayer->Energy = 10000;
        strcpy(splayer->Cars,"XFG ");
        strcpy(splayer->Lang,"rus");


        save_user(splayer);
        save_user_cars(splayer);
        save_user_fines(splayer);
    }
    if (strcmp(splayer->Lang,"eng") == 0)
    {
        splayer->lang_id = 1;
    }
    readf.close();
}
**/
void read_user_cars(struct player *splayer)
{
    char file[255];
    strcpy(file,RootDir);
    strcat(file,"data\\RCGarage\\");
    strcat(file,splayer->UName);
    strcat(file,".txt");


    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        strcpy(splayer->cars[0].car,"XFG");
        splayer->cars[0].tuning = 0;
        splayer->cars[0].dist = 0;
        save_user_cars(splayer);
    }
    else
    {
        ifstream readf (file,ios::in);
        int i=0;
        while (readf.good())
        {
            char str[128];
            readf.getline(str,128);

            if (strlen(str) > 0)
            {
                char *car;
                char *tun;
                char *dis;

                car = strtok(str,";");
                tun = strtok(NULL,";");
                dis = strtok(NULL,";");

                strcpy(splayer->cars[i].car,car);
                splayer->cars[i].tuning = atoi(tun);
                splayer->cars[i].dist = atoi(dis);

                i++;
            }
        }
    }
    FindClose(fff);





}

void read_user_fines(struct player *splayer)
{
    char file[255];
    strcpy(file,RootDir);
    strcat(file,"data\\RCPolice\\fines\\");
    strcat(file,splayer->UName);
    strcat(file,".txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        save_user_fines(splayer);
    }
    else
    {
        ifstream readf (file,ios::in);
        int i=0;
        while (readf.good())
        {
            char str[128];
            readf.getline(str,128);

            if (strlen(str) > 0)
            {
                char *id;
                char *date;


                id = strtok(str,";");
                date = strtok(NULL,";");


                splayer->fines[i].fine_id = atoi(id);
                splayer->fines[i].fine_date = atoi(date);

                i++;
            }
        }
    }




}


void save_car (struct player *splayer)
{

    out <<splayer->UName << " save " << splayer->CName << " info" << endl;

    for (int i=0; i<MAX_CARS; i++)
    {
        if (strcmp(splayer->CName,splayer->cars[i].car)==0)
        {
            splayer->cars[i].tuning = splayer->CTune;
            splayer->cars[i].dist = (int)splayer->Distance;
            break;
        }
    }

}



void help_cmds (struct player *splayer,int h_type)
{
    if (h_type == 1)
    {


        for ( int i = 3200 ; i <= 3214 ; i ++ )
            send_mtc(splayer->UCID,msg.message[splayer->lang_id][i]);


    }
    if (h_type == 2)
    {

        send_mtc(splayer->UCID,msg.message[splayer->lang_id][3100]);
        for ( int i=0; i<MAX_CARS; i++)
        {
            if (strlen(splayer->cars[i].car)>0)
            {
                //int     id;
                char    car[4];
                int     tune;
                char    dist[16];
                char    Text[64];

                strcpy(car , splayer->cars[i].car);
                tune = splayer->cars[i].tuning;
                itoa(splayer->cars[i].dist,dist,10);


                strcpy( Text, "^1| ^C^2");
                strcat( Text,car);
                strcat( Text," ^3");
                int dist_i = atoi(dist)/1000;
                char dist_c[12];
                itoa(dist_i,dist_c,10);
                strcat(Text,dist_c);
                strcat( Text,"^7 km (");

                int tune2 = 45;
                if (tune&1)
                {
                    strcat( Text,"^2E");
                    tune2 -= 15;
                }
                else
                {
                    strcat( Text,"^1E");
                }

                if (tune&2)
                {
                    strcat( Text," ^2T");
                    tune2 -= 20;
                }
                else
                {
                    strcat( Text," ^1T");
                }

                if (tune&8)
                    strcat( Text," ^2A");
                else
                {
                    strcat( Text," ^1A");
                }

                strcat( Text,"^7)(^3");

                char intake[16];
                itoa(tune2,intake,10);
                strcat( Text,intake);
                strcat( Text,"%^7)");
                send_mtc(splayer->UCID,Text);
            }
        }

    }
    if (h_type == 3)
    {

        int i=0;
        int j=0;
        for (i=0; i<MAX_FINES; i++)
        {
            if (splayer->fines[i].fine_id > 0)
            {


                char Text[64];
                // row[0] = username
                // row[1] = fine_id
                // row[2] = fine_date

                int fine_id = splayer->fines[i].fine_id;
                // int fine_date = atoi(row[2]);
                char num[3];
                itoa(fine_id,num,10);

                char pay[5];
                itoa(ginfo.fines[fine_id].cash,pay,10);

                strcpy(Text,"^2| ^7ID = ");
                strcat( Text,num);
                strcat( Text,". ");
                strncat( Text,ginfo.fines[fine_id].name,64-34);
                strcat( Text," ^3(^2");
                strcat( Text,pay);
                strcat( Text," RUR.^3)");
                send_mtc(splayer->UCID,Text);

                j++;
            }
        }

        if (j == 0)
        {
            send_mtc(splayer->UCID,msg.message[splayer->lang_id][3102]);
        }

    }

}



/**** buttons ****/
void btn_main (struct player *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;

    /* background */
    pack.BStyle = 16+64;
    pack.ClickID = 100;
    pack.L = 100-126/2;
    pack.T = 25;
    pack.W = 126;
    pack.H = 150;
    strcpy(pack.Text,"");
    insim.send_packet(&pack);
    Sleep(100);
    /* background */

    /* Title */
    pack.BStyle = 0;
    pack.ClickID = 101;
    pack.L = 101-126/2;
    pack.T = 26;
    pack.W = 126;
    pack.H = 20;
    strcpy(pack.Text,"^7RUSSIAN CRUISE");
    insim.send_packet(&pack);
    Sleep(100);
    /* Title */

    /* background #2 */
    pack.BStyle = 16+32;
    pack.ClickID = 102;
    pack.L = 101-126/2;
    pack.T = 55;
    pack.W = 124;
    pack.H = 118;
    strcpy(pack.Text,"");
    insim.send_packet(&pack);
    Sleep(100);

    /* background #2 */

}

void btn_info (struct player *splayer, int b_type)
{
    btn_main(splayer);

    char info_text[5][100];
    strncpy(info_text[0], "^7RUSSIAN CRUISE",99);
    strncpy(info_text[1], "^C^7 Добро пожаловать, ",99);
    strncpy(info_text[2], "^C^7 Деньги: ^2",99);
    strncpy(info_text[3], "^C^7   тут жизненный статус",99);
    strncpy(info_text[4], "^C^7Доступные автомобили:",99);




    char about_text[10][100];
    strncpy(about_text[0], "^7RUSSIAN CRUISE v 1.1.8",99);
    strncpy(about_text[1], "^C^7Developer: Kostin Denis",99);
    strncpy(about_text[2], "^C^7ICQ: 5518182",99);
    strncpy(about_text[3], "^C^7Skype: denisko_leva",99);
    strncpy(about_text[4], "^7",99);
    strncpy(about_text[5], "^C^7More information",99);
    strncpy(about_text[6], "^C^7http://russiancruise.ucoz.ru/",99);
    strncpy(about_text[7], "^7",99);
    strncpy(about_text[8], "^C^7        Thanks:",99);
    strncpy(about_text[9], "^C^3        repeat, NOSE, etc",99);

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;

    /* Tabs */
    pack.BStyle = 32+8;
    pack.ClickID = 103;
    pack.L = 101-126/2;
    pack.T = 46;
    pack.W = 16;
    pack.H = 8;
    strcpy(pack.Text,msg.message[splayer->lang_id][200]);
    insim.send_packet(&pack);
    Sleep(100);

    pack.BStyle = 32+8;
    pack.ClickID = 106;
    pack.L += 17;
    strcpy(pack.Text,msg.message[splayer->lang_id][201]);
    insim.send_packet(&pack);
    Sleep(100);


    pack.BStyle = 32+8;
    pack.ClickID = 105;
    pack.L += 17;
    pack.W = 20;
    strcpy(pack.Text,msg.message[splayer->lang_id][202]);
    insim.send_packet(&pack);
    Sleep(100);
    /* Tabs */



    if (b_type == 1)
    {
        for (int i=2; i<9; i++)
        {
            pack.L = (101-126/2)+1;
            pack.BStyle = 16;
            pack.T = 46+9*(i-1)+1;
            pack.W = 50;
            pack.H = 8;
            pack.ClickID = 110 + i;
            char cash[6];
            char sell[6];
            itoa(ginfo.car[i].cash,cash,10);
            itoa(ginfo.car[i].sell,sell,10);
            strcpy(pack.Text,"^2");
            strcat(pack.Text,ginfo.car[i].car);
            strcat(pack.Text," ^7(^2");
            strcat(pack.Text,cash);
            strcat(pack.Text,"^7/^3");
            strcat(pack.Text,sell);
            strcat(pack.Text,"^7)");
            insim.send_packet(&pack);
        }

        for (int i=11; i<19; i++)
        {
            pack.L = (101-126/2)+1+51;
            pack.BStyle = 16;
            pack.T = 46+9*(i-10)+1;
            pack.W = 50;
            pack.H = 8;
            pack.ClickID = 110 + i;
            char cash[6];
            char sell[6];
            itoa(ginfo.car[i].cash,cash,10);
            itoa(ginfo.car[i].sell,sell,10);

            strcpy(pack.Text,"^2");
            strcat(pack.Text,ginfo.car[i].car);
            strcat(pack.Text," ^7(^2");
            strcat(pack.Text,cash);
            strcat(pack.Text,"^7/^3");
            strcat(pack.Text,sell);
            strcat(pack.Text,"^7)");
            insim.send_packet(&pack);
        }

    } // if (type == 1)

    if (b_type == 2)
    {


    }

    if (b_type == 3)
    {
        pack.BStyle = 64;
        pack.L = 101-126/2;
        pack.W = 90;
        for (int g=0; g<10; g++)
        {
            pack.ClickID = 110 + g;
            pack.T += 9;
            strcpy(pack.Text,about_text[g]);
            insim.send_packet(&pack);
            Sleep(100);
        }
    }

    if (b_type == 4)
    {
        for (int i=1; i<=12; i++)
        {
            pack.L = (101-126/2)+1;
            pack.BStyle = 16 + 64;
            pack.T = 56+7*(i-1);
            pack.W = 122;
            pack.H = 6;
            pack.ClickID = 110 + i;

            // row[0] = username
            // row[1] = fine_id
            // row[2] = fine_date
            // int fine_date = atoi(row[2]);
            char num[3];
            itoa(ginfo.fines[i].id,num,10);

            char pay[5];
            itoa(ginfo.fines[i].cash,pay,10);

            strcpy(pack.Text,"^2| ^7ID = ");
            strcat( pack.Text,num);
            strcat( pack.Text,". ");
            strncat( pack.Text,ginfo.fines[i].name,96-34);
            strcat( pack.Text," ^3(^2");
            strcat( pack.Text,pay);
            strcat( pack.Text," RUR.^3)");
            insim.send_packet(&pack);
        }

    }

    pack.BStyle = 8;
    pack.ClickID = 149;
    pack.L = 99+126/2-8;
    pack.T = 26;
    pack.W = 8;
    pack.H = 8;
    strcpy(pack.Text,"^1[X]");
    insim.send_packet(&pack);
}



void btn_shop (struct player *splayer)
{
    //btn_main(splayer);
    int j;
    for (j =0 ; j< MAX_PLAYERS; j++)
    {
        if (bank.players[j].UCID == splayer->UCID)
        {
            break;
        }

    }

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;

    /* Tabs */
    pack.BStyle = 16;
    pack.ClickID = 150;
    pack.L = 39;
    pack.T = 56;
    pack.W = 33;
    pack.H = 8;
    strcpy(pack.Text,msg.message[splayer->lang_id][600]);
    insim.send_packet(&pack);
    pack.BStyle = 16+8;
    pack.ClickID = 151;
    pack.L = 39;
    pack.T = 56+9;
    pack.W = 16;
    strcpy(pack.Text,msg.message[splayer->lang_id][602]);
    insim.send_packet(&pack);
    pack.BStyle = 16+8;
    pack.ClickID = 152;
    pack.L += 17;
    pack.W = 16;
    strcpy(pack.Text,msg.message[splayer->lang_id][603]);
    insim.send_packet(&pack);
    pack.BStyle = 16;
    pack.ClickID = 153;
    pack.L = 39+34;
    pack.T = 56;
    pack.W = 33;
    pack.H = 8;
    strcpy(pack.Text,msg.message[splayer->lang_id][601]);
    insim.send_packet(&pack);
    pack.BStyle = 16+8;
    pack.ClickID = 154;
    pack.T = 56+9;
    pack.W = 16;
    strcpy(pack.Text,msg.message[splayer->lang_id][602]);
    insim.send_packet(&pack);
    pack.BStyle = 16+8;
    pack.ClickID = 155;
    pack.L += 17;
    pack.W = 16;
    strcpy(pack.Text,msg.message[splayer->lang_id][603]);
    insim.send_packet(&pack);
    /* Tabs */

    // Заплатка для магазина
    char Cars[255];
    ZeroMemory(&Cars,255);
    for (int i=1; i<MAX_CARS; i++)
    {
        for (int j=1; j<MAX_CARS; j++)
        {
            if ((strcmp(ginfo.car[i].car,splayer->cars[j].car) == 0) and (strlen(ginfo.car[i].car) > 0))
                strcat(Cars,splayer->cars[j].car);
        }
    }
    //cout << Cars << endl;


    if (splayer->Shop == 1)
    {


        pack.L = 39;
        pack.W = 33;
        pack.H = 8;

        for (int i=2; i<9; i++)
        {
            pack.ClickID = 50 + i;
            pack.T = 65+9*(i-1);

            if ((bank.players[j].Cash > ginfo.car[i].cash) and (strstr(Cars,ginfo.car[i].car)==0))
            {
                pack.BStyle = 4+16+8;
                strcpy(pack.Text,"^2");
            }
            else
            {
                pack.BStyle = 4+32;
                strcpy(pack.Text,"^1");
            }

            strcat(pack.Text,ginfo.car[i].car);
            insim.send_packet(&pack);
        }

        if (strcmp(ginfo.Product,"S2")==0)
        {
            pack.L = 39+34;
            pack.W = 33;
            pack.T = 65+9;


            for (int i=11; i<19; i++)
            {
                pack.ClickID = 50 + i;
                pack.T = 65+9*(i-10);
                if ((bank.players[j].Cash > ginfo.car[i].cash) and (strstr(Cars,ginfo.car[i].car)==0))
                {
                    pack.BStyle = 4+16+8;
                    strcpy(pack.Text,"^2");
                }
                else
                {
                    pack.BStyle = 4+32;
                    strcpy(pack.Text,"^1");
                }
                strcat(pack.Text,ginfo.car[i].car);
                insim.send_packet(&pack);
            }
        }

    } // if b_type == 1

    if (splayer->Shop == 2)
    {

        pack.L = 39;
        pack.W = 33;
        pack.H = 8;
        pack.BStyle = 4+16+8;

        int b = 1;
        for (int i=2; i<9; i++)
        {
            // out << ginfo.car[i].car << " " << splayer->Cars << endl;
            // out << (int)strstr(splayer->Cars,ginfo.car[i].car) << endl;
            if (strstr(Cars,ginfo.car[i].car))
            {
                pack.ClickID = 50 + i;
                pack.T = 65+9*b;
                strcpy(pack.Text,"^2");
                strcat(pack.Text,ginfo.car[i].car);
                insim.send_packet(&pack);
                b++;
            }
        }

        if (strcmp(ginfo.Product,"S2")==0)
        {
            pack.L = 39+34;
            pack.W = 33;
            pack.T = 65+9;
            b=1;
            for (int i=1; i<9; i++)
            {
                // out << ginfo.car[10+i].car << " " << splayer->Cars << endl;
                // out << (int)strstr(splayer->Cars,ginfo.car[10+i].car) << endl;
                if (strstr(Cars,ginfo.car[10+i].car))
                {
                    pack.ClickID = 60 + i;
                    pack.T = 65+9*b;
                    strcpy(pack.Text,"^2");
                    strcat(pack.Text,ginfo.car[10+i].car);
                    insim.send_packet(&pack);
                    b++;
                }
            }
        }


    }

    if (splayer->Shop == 3)
    {


        pack.L = 39;
        pack.W = 33;
        pack.H = 8;
        pack.BStyle = 4+16+8;

        int b = 1;
        if (!(splayer->CTune&1) and bank.players[j].Cash > 5000)
        {
            pack.ClickID = 71;
            pack.T = 65+9*b;
            strcpy(pack.Text,"^2ECU");
            insim.send_packet(&pack);
            b++;
        }
        if (!(splayer->CTune&2) and (splayer->CTune&1) and bank.players[j].Cash > 10000)
        {
            pack.ClickID = 72;
            pack.T = 65+9*b;
            strcpy(pack.Text,"^2Turbo");
            insim.send_packet(&pack);
            b++;
        }

        if (!(splayer->CTune&8) and bank.players[j].Cash > 20000)
        {
            pack.ClickID = 73;
            pack.T = 65+9*b;
            strcpy(pack.Text,"^2ABS");
            insim.send_packet(&pack);
            b++;
        }

        pack.BStyle = 16+8;
        pack.ClickID = 74;
        pack.L = 39+34;
        pack.W = 33;
        pack.H = 8;
        pack.T = 65+9;
        pack.TypeIn = 16;
        strcpy(pack.Text,"^C^2Окраска(^1без ^7CAR_^2)");
        //insim.send_packet(&pack);



    }

    if (splayer->Shop == 4)
    {


        pack.L = 39;
        pack.W = 33;
        pack.H = 8;
        pack.BStyle = 4+16+8;

        int b = 1;
        if (splayer->CTune&1)
        {
            pack.ClickID = 71;
            pack.T = 65+9*b;
            strcpy(pack.Text,"^2ECU");
            insim.send_packet(&pack);
            b++;
        }
        if (splayer->CTune&2)
        {
            pack.ClickID = 72;
            pack.T = 65+9*b;
            strcpy(pack.Text,"^2Turbo");
            insim.send_packet(&pack);
            b++;
        }

        if (splayer->CTune&8)
        {
            pack.ClickID = 73;
            pack.T = 65+9*b;
            strcpy(pack.Text,"^2ABS");
            insim.send_packet(&pack);
            b++;
        }

    }

    pack.BStyle = 8 + 16;
    pack.ClickID = 149;
    pack.TypeIn =0;
    pack.L = 39 +34 +34;
    pack.T = 56;
    pack.W = 16;
    pack.H = 8;
    strcpy(pack.Text,msg.message[splayer->lang_id][604]);
    insim.send_packet(&pack);
}


void btn_panel (struct player *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.BStyle = 32;
    pack.ClickID = 160;
    pack.L = 115;
    pack.T = 1;
    pack.W = 33;
    pack.H = 4;
    strcpy(pack.Text,splayer->street[splayer->StreetNum].Street);
    strcat(pack.Text," ^7(^1");
    char speed[3];
    itoa(splayer->street[splayer->StreetNum].SpeedLimit,speed,10);
    strcat(pack.Text,speed);
    strcat(pack.Text," ^C^3km/h^7)");
    insim.send_packet(&pack);

    pack.ClickID = 161;
    pack.L = 100;
    pack.T = 1;
    pack.W = 15;
    pack.H = 4;

    if (splayer->Zone== 1)
        strcpy(pack.Text,msg.message[splayer->lang_id][400]);
    else if (splayer->Zone== 2)
        strcpy(pack.Text,msg.message[splayer->lang_id][401]);
    else if (splayer->Zone== 3)
        strcpy(pack.Text,msg.message[splayer->lang_id][402]);
    else
        strcpy(pack.Text,msg.message[splayer->lang_id][403]);
    insim.send_packet(&pack);

}





void btn_sirena(struct player *splayer)
{
    //int Heith = 30;
    //int Left = 100 - Heith/2;

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 203;
    pack.BStyle = 1;
    //pack.L = 50;

    pack.T = 20;
    pack.W = 124 - (splayer->sirenaSize);
    pack.L = 100 - pack.W/2;
    pack.H = pack.W/3;

    if (pack.W <= 0)
        pack.W = 1;

    if (pack.L <= 0)
        pack.L = 1;

    if (pack.H <= 0)
        pack.H = 1;

    strcpy(pack.Text,siren);
    insim.send_packet(&pack);
}

void btn_pogonya(struct player *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 204;
    pack.BStyle = 1;
    pack.L = 50;
    pack.T = 51;
    pack.W = 100;
    pack.H = 30;
    strcpy(pack.Text,splayer->PogonyaReason);
    insim.send_packet(&pack);
}

void btn_work (struct player *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 210;
    pack.BStyle = 32;
    pack.L = 148;
    pack.T = 1;
    pack.W = 10;
    pack.H = 8;

    char min_c[3];
    char sec_c[3];

    int time2 = splayer->WorkTime - time(&stime);
    int min;
    int sec;

    min = time2/60;
    sec = time2%60;

    itoa(min,min_c,10);
    itoa(sec,sec_c,10);

    strcpy(pack.Text,"^2");
    strcat(pack.Text,min_c);
    strcat(pack.Text,":");

    if (sec < 10)
        strcat(pack.Text,"0");

    strcat(pack.Text,sec_c);
    insim.send_packet(&pack);

}

/****************************************/
void case_bfn ()
{
    struct IS_BFN *pack_bfn = (struct IS_BFN*)insim.get_packet();

    int i;
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].UCID == pack_bfn->UCID)
        {
            if (ginfo.players[i].bfn == 0)
            {
                ginfo.players[i].bfn = 1;
                btn_info(&ginfo.players[i],1);
            }
            else
            {
                ginfo.players[i].bfn = 0;
            }
            break;
        }
    }
}

void case_btc ()
{
    struct IS_BTC *pack_btc = (struct IS_BTC*)insim.get_packet();

    //out << (int)pack_btc->ClickID << endl;
    int i;
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].UCID == pack_btc->UCID)
        {

            char day[3];
            char month[3];
            char year[3];
            SYSTEMTIME sm;
            GetLocalTime(&sm);
            itoa(sm.wDay,day,10);
            itoa(sm.wMonth,month,10);
            itoa(sm.wYear,year,10);
            char log[255];
            strcpy(log,RootDir);
            strcat(log,"logs\\shop\\shop");
            strcat(log,"(");
            strcat(log,day);
            strcat(log,".");
            strcat(log,month);
            strcat(log,".");
            strcat(log,year);
            strcat(log,").txt");

            if (pack_btc->ClickID<=32)
            {
                ginfo.players[i].BID2 =  pack_btc->ClickID;

                if (ginfo.players[i].Action == 1) // send
                {
                    struct IS_BTN pack_btn;
                    memset(&pack_btn, 0, sizeof(struct IS_BTN));
                    pack_btn.Size = sizeof(struct IS_BTN);
                    pack_btn.Type = ISP_BTN;
                    pack_btn.ReqI = ginfo.players[i].UCID;              // Must be non-zero, I'll just use UCID
                    pack_btn.UCID = ginfo.players[i].UCID;              // UCID of the player that will receive the button
                    pack_btn.BStyle = 16 + ISB_CLICK;                 // Dark frame for window title
                    pack_btn.TypeIn = 16;
                    pack_btn.L = 116;
                    pack_btn.T = 13;
                    pack_btn.W = 34;
                    pack_btn.H = 5;
                    pack_btn.ClickID = 36;
                    strcpy(pack_btn.Text,msg.message[ginfo.players[i].lang_id][1000]);
                    insim.send_packet(&pack_btn);

                    pack_btn.TypeIn = 64-24-14;
                    pack_btn.L = 116;
                    pack_btn.T += 6;
                    pack_btn.W = 34;
                    pack_btn.H = 5;
                    pack_btn.ClickID = 37;
                    strcpy(pack_btn.Text, msg.message[ginfo.players[i].lang_id][1001]);
                    insim.send_packet(&pack_btn);

                    // cop buttons
                    if (ginfo.players[i].cop != 0)
                    {
                        pack_btn.BStyle = 32 + ISB_CLICK;
                        pack_btn.TypeIn = 2;
                        pack_btn.L = 116;
                        pack_btn.T += 6;
                        pack_btn.W = 34;
                        pack_btn.H = 5;
                        pack_btn.ClickID = 38;
                        strcpy(pack_btn.Text, msg.message[ginfo.players[i].lang_id][1002]);
                        insim.send_packet(&pack_btn);

                        pack_btn.BStyle = 32 + ISB_CLICK;
                        pack_btn.TypeIn = 2;
                        pack_btn.L = 116;
                        pack_btn.T += 6;
                        pack_btn.W = 34;
                        pack_btn.H = 5;
                        pack_btn.ClickID = 39;
                        strcpy(pack_btn.Text, msg.message[ginfo.players[i].lang_id][1003]);
                        insim.send_packet(&pack_btn);

                        pack_btn.BStyle = 32 + ISB_CLICK;
                        pack_btn.TypeIn = 0;
                        pack_btn.L = 116;
                        pack_btn.T += 6;
                        pack_btn.W = 34;
                        pack_btn.H = 5;
                        pack_btn.ClickID = 40;
                        strcpy(pack_btn.Text, msg.message[ginfo.players[i].lang_id][1004]);
                        insim.send_packet(&pack_btn);

                        pack_btn.BStyle = 32 + ISB_CLICK;
                        pack_btn.TypeIn = 0;
                        pack_btn.L = 116;
                        pack_btn.T += 6;
                        pack_btn.W = 34;
                        pack_btn.H = 5;
                        pack_btn.ClickID = 41;
                        strcpy(pack_btn.Text, msg.message[ginfo.players[i].lang_id][1005]);
                        insim.send_packet(&pack_btn);
                    }
                }
            }
            if (pack_btc->ClickID==34)
            {
                for (int j=0; j<50; j++)
                    send_bfn(ginfo.players[i].UCID,j);
                Sleep(100);
            }

            if (pack_btc->ClickID==40)
            {

                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo.players[i].BID2 == ginfo.players[g].BID)
                    {
                        if (ginfo.players[g].Pogonya == 0)
                        {
                            ginfo.players[g].Pogonya = 1;
                            int worktime = time(&stime);
                            ginfo.players[g].WorkTime = worktime+60*6;
                            strcpy(ginfo.players[g].PogonyaReason,msg.message[ginfo.players[g].lang_id][1006]);
                            char Text[96];
                            strcpy(Text,"/msg ^2| ");
                            //strcat(Text,ginfo.players[i].PName);
                            strcat(Text,msg.message[ginfo.players[g].lang_id][1007]);
                            strcat(Text,ginfo.players[g].PName);
                            send_mst(Text);
                        }
                        break;
                    }
                }
            }
            if (pack_btc->ClickID==41)
            {

                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo.players[i].BID2 == ginfo.players[g].BID)
                    {
                        if (ginfo.players[g].Pogonya != 0)
                        {
                            ginfo.players[g].Pogonya = 0;
                            send_bfn(ginfo.players[g].UCID,210);
                            char Text[96];
                            strcpy(Text,"/msg ^2| ");
                            //strcat(Text,ginfo.players[i].PName);
                            strcat(Text,msg.message[ginfo.players[g].lang_id][1008]);
                            strcat(Text,ginfo.players[g].PName);
                            send_mst(Text);
                        }
                        break;
                    }
                }
            }

            /** car buttons */
            for (int g=50; g< 70; g++)
            {
                if (g == pack_btc->ClickID)
                {
                    out << ginfo.players[i].UName << "come in to shop and \n";
                    if (ginfo.players[i].Shop == 1)
                    {
                        //strcat(ginfo.players[i].Cars,ginfo.car[g-50].car);
                        //strcat(ginfo.players[i].Cars," ");
                        bank.players[i].Cash -= ginfo.car[g-50].cash;
                        bank.BankFond += ginfo.car[g-50].cash;
                        out << "buy car - " << ginfo.car[g-50].car << endl;




                        ofstream readf (log,ios::app);
                        readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo.players[i].UName << " buy car " << ginfo.car[g-50].car << endl;
                        readf.close();

                        for ( int j=0; j<MAX_CARS; j++)
                        {
                            if (strlen(ginfo.players[i].cars[j].car) == 0)
                            {
                                strcpy(ginfo.players[i].cars[j].car,ginfo.car[g-50].car);
                                ginfo.players[i].cars[j].tuning=0;
                                ginfo.players[i].cars[j].dist=0;
                                break;
                            }

                        }



                        for (int j=40; j<200; j++)
                            send_bfn(pack_btc->UCID,j);

                        ginfo.players[i].Shop = 1;
                        btn_shop(&ginfo.players[i]);
                    }
                    if (ginfo.players[i].Shop == 2)
                    {
                        if (strcmp(ginfo.players[i].CName,ginfo.car[g-50].car)!=0)
                        {
                            out << "sell car - " << ginfo.car[g-50].car << endl;



                            ofstream readf (log,ios::app);

                            readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  ginfo.players[i].UName << " sell car " << ginfo.car[g-50].car << endl;


                            /*string Cars;
                            Cars += ginfo.players[i].Cars;
                            out << Cars << endl;
                            Cars.replace(Cars.find(ginfo.car[g-50].car),4,"");
                            out << Cars << endl;

                            strcpy(ginfo.players[i].Cars,Cars.c_str());*/
                           // readf << "DEBAG:" << bank.players[i].Cash << endl;
                            bank.players[i].Cash += ginfo.car[g-50].sell;
                            bank.BankFond -= ginfo.car[g-50].sell;

                            //readf << "DEBAG:" << bank.players[i].Cash << endl;
                            //out << ginfo.players[i].Cars << endl;
                            readf.close();
                            for ( int j=0; j<MAX_CARS; j++)
                            {
                                if (strcmp(ginfo.players[i].cars[j].car,ginfo.car[g-50].car) == 0)
                                {
                                    strcpy(ginfo.players[i].cars[j].car,"");
                                    ginfo.players[i].cars[j].tuning=0;
                                    ginfo.players[i].cars[j].dist=0;
                                    break;
                                }

                            }


                            for (int j=40; j<200; j++)
                                send_bfn(pack_btc->UCID,j);

                            ginfo.players[i].Shop = 2;
                            btn_shop(&ginfo.players[i]);
                        }
                        else
                        {
                            send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][1009]);
                        }
                    }
                    break;
                } // if g == clickId
            } // for g

            /** car buttons */

            /** tuning buttons */

            if (pack_btc->ClickID == 71)
            {
                if (ginfo.players[i].Shop == 3)
                {

                    ginfo.players[i].CTune += 1;
                    bank.players[i].Cash -= 5000;
                    bank.BankFond += 5000;


                    ofstream readf (log,ios::app);
                    readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo.players[i].UName << " buy ECU "<< endl;
                    readf.close();

                    for (int j=40; j<200; j++)
                        send_bfn(pack_btc->UCID,j);

                    ginfo.players[i].Shop = 3;
                    btn_shop(&ginfo.players[i]);
                }
                if (ginfo.players[i].Shop == 4)
                {

                    ginfo.players[i].CTune -= 1;
                    bank.players[i].Cash += 5000*8/10;
                    bank.BankFond -= 5000*8/10;

                    ofstream readf (log,ios::app);
                    readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo.players[i].UName << " sell ECU "<< endl;
                    readf.close();

                    for (int j=40; j<200; j++)
                        send_bfn(pack_btc->UCID,j);

                    ginfo.players[i].Shop = 4;
                    btn_shop(&ginfo.players[i]);
                }
            }
            if (pack_btc->ClickID == 72)
            {
                if (ginfo.players[i].Shop == 3)
                {

                    ginfo.players[i].CTune += 2;
                    bank.players[i].Cash -= 10000;
                    bank.BankFond += 10000;


                    ofstream readf (log,ios::app);
                    readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo.players[i].UName << " buy Turbo "<< endl;
                    readf.close();

                    for (int j=40; j<200; j++)
                        send_bfn(pack_btc->UCID,j);

                    ginfo.players[i].Shop = 3;
                    btn_shop(&ginfo.players[i]);
                }
                if (ginfo.players[i].Shop == 4)
                {

                    ginfo.players[i].CTune -= 2;
                    bank.players[i].Cash += 10000*8/10;
                    bank.BankFond -= 10000*8/10;


                    ofstream readf (log,ios::app);
                    readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo.players[i].UName << " sell Turbo "<< endl;
                    readf.close();

                    for (int j=40; j<200; j++)
                        send_bfn(pack_btc->UCID,j);

                    ginfo.players[i].Shop = 4;
                    btn_shop(&ginfo.players[i]);
                }
            }
            if (pack_btc->ClickID == 73)
            {
                if (ginfo.players[i].Shop == 3)
                {

                    ginfo.players[i].CTune += 8;
                    bank.players[i].Cash -= 20000;
                    bank.BankFond += 20000;


                    ofstream readf (log,ios::app);
                    readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo.players[i].UName << " buy ABS "<< endl;
                    readf.close();

                    for (int j=40; j<159; j++)
                        send_bfn(pack_btc->UCID,j);

                    ginfo.players[i].Shop = 3;
                    btn_shop(&ginfo.players[i]);
                }
                if (ginfo.players[i].Shop == 4)
                {

                    ginfo.players[i].CTune -= 8;
                    bank.players[i].Cash += 20000*8/10;
                    bank.BankFond -= 20000*8/10;


                    ofstream readf (log,ios::app);
                    readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo.players[i].UName << " sell ABS "<< endl;
                    readf.close();

                    for (int j=40; j<159; j++)
                        send_bfn(pack_btc->UCID,j);

                    ginfo.players[i].Shop = 4;
                    btn_shop(&ginfo.players[i]);
                }
            }

            /** tuning buttons */

            /** shop buttons */
            if (pack_btc->ClickID == 151)
            {
                for (int j=40; j<159; j++)
                    send_bfn(pack_btc->UCID,j);
                ginfo.players[i].Shop = 1;
                btn_shop(&ginfo.players[i]);
            }
            if (pack_btc->ClickID == 152)
            {
                for (int j=40; j<159; j++)
                    send_bfn(pack_btc->UCID,j);
                ginfo.players[i].Shop = 2;
                btn_shop(&ginfo.players[i]);
            }

            if (pack_btc->ClickID == 154)
            {
                for (int j=40; j<159; j++)
                    send_bfn(pack_btc->UCID,j);
                ginfo.players[i].Shop = 3;
                btn_shop(&ginfo.players[i]);
            }
            if (pack_btc->ClickID == 155)
            {
                for (int j=40; j<159; j++)
                    send_bfn(pack_btc->UCID,j);
                ginfo.players[i].Shop = 4;
                btn_shop(&ginfo.players[i]);
            }
            /** shop buttons */

            /** info buttons **/
            if (pack_btc->ClickID == 149)
            {
                for (int j=159; j>0; j--)
                {
                    send_bfn(pack_btc->UCID,j);
                    Sleep(10);
                }
            }

            if (pack_btc->ClickID == 103)
            {
                for (int j=0; j<50; j++)
                    send_bfn(pack_btc->UCID,100+j);
                btn_info(&ginfo.players[i],1);
            }

            if (pack_btc->ClickID == 104)
            {
                for (int j=0; j<50; j++)
                    send_bfn(pack_btc->UCID,100+j);

                btn_info(&ginfo.players[i],2);
            }

            if (pack_btc->ClickID == 105)
            {
                for (int j=0; j<50; j++)
                    send_bfn(pack_btc->UCID,100+j);

                btn_info(&ginfo.players[i],3);
            }
            if (pack_btc->ClickID == 106)
            {
                for (int j=0; j<50; j++)
                    send_bfn(pack_btc->UCID,100+j);

                btn_info(&ginfo.players[i],4);
            }
            /** info buttons **/

            if (pack_btc->ClickID == 200)
            {
                for (int j=0; j<5; j++)
                    send_bfn(pack_btc->UCID,200+j);
            }

            break;
        } // if UCID
    }// for
}

void case_btt ()
{
    struct IS_BTT *pack_btt = (struct IS_BTT*)insim.get_packet();

    //out << (int)pack_btt->ClickID << endl;
    char day[3];
    char month[3];
    char year[3];
    SYSTEMTIME sm;
    GetLocalTime(&sm);
    itoa(sm.wDay,day,10);
    itoa(sm.wMonth,month,10);
    itoa(sm.wYear,year,10);

    char send_c[255];
    strcpy(send_c,RootDir);
    strcat(send_c,"logs\\sends\\send.txt");

    char fine_c[255];
    strcpy(fine_c,RootDir);
    strcat(fine_c,"logs\\fines\\fine");
    strcat(fine_c,"(");
    strcat(fine_c,day);
    strcat(fine_c,".");
    strcat(fine_c,month);
    strcat(fine_c,".");
    strcat(fine_c,year);
    strcat(fine_c,").txt");


    int i;
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].UCID == pack_btt->UCID)
        {
            if (pack_btt->ClickID==36) // send cash
            {

                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo.players[i].BID2 == ginfo.players[g].BID)
                    {
                        if (atoi(pack_btt->Text) > 0)
                        {
                            if (bank.players[i].Cash > atoi(pack_btt->Text))
                            {
                                out << ginfo.players[i].UName << " send " << pack_btt->Text << " to "  << ginfo.players[g].UName << endl;
                                bank.players[i].Cash -= atoi(pack_btt->Text);
                                bank.players[g].Cash += atoi(pack_btt->Text);

                                char money[96];
                                itoa(atoi(pack_btt->Text),money,10);
                                char Msg[64];
                                strcpy(Msg,"^1| ");
                                strcat(Msg,ginfo.players[i].PName);
                                strcat(Msg,msg.message[ginfo.players[i].lang_id][1100]);
                                strcat(Msg,money);
                                strcat(Msg," RUR.");
                                send_mtc(ginfo.players[g].UCID,Msg);

                                ofstream readf (send_c,ios::app);
                                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  ginfo.players[i].UName << " send " << pack_btt->Text << " RUR. to "  << ginfo.players[g].UName << endl;
                                readf.close();
                            }
                            else
                            {
                                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][1101]);
                                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][1102]);
                            }
                        } // if atoi(pack_btt->Text) > 0
                        break;
                    }
                }//for

            }

            else if (pack_btt->ClickID == 37) // send privat message
            {

                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo.players[i].BID2 == ginfo.players[g].BID)
                    {
                        if (strlen(pack_btt->Text) > 0)
                        {
                            out << ginfo.players[i].UName << " send " << pack_btt->Text << " to "  << ginfo.players[g].UName << endl;
                            char Msg[64];
                            strcpy(Msg,msg.message[ginfo.players[g].lang_id][1103]);
                            strcat(Msg,ginfo.players[i].PName);
                            strcat(Msg,": ");
                            strcat(Msg,pack_btt->Text);
                            send_mtc(ginfo.players[g].UCID,Msg);
                        } // if atoi(pack_btt->Text) > 0
                        break;
                    }
                }//for

            }
            else if (pack_btt->ClickID==38)
            {

                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo.players[i].BID2 == ginfo.players[g].BID)
                    {
                        if (atoi(pack_btt->Text) > 0)
                        {

                            out << ginfo.players[i].UName << " send fine id = " << pack_btt->Text << " to "  << ginfo.players[g].UName << endl;

                            char Msg[64];
                            strcpy(Msg,msg.message[ginfo.players[g].lang_id][1104]);
                            send_mtc(ginfo.players[g].UCID,Msg);
                            strcpy(Msg,"^2| ^7");
                            strcat(Msg,ginfo.fines[atoi(pack_btt->Text)].name);
                            send_mtc(ginfo.players[g].UCID,Msg);

                            strcpy(Msg,msg.message[ginfo.players[i].lang_id][1105]);
                            send_mtc(ginfo.players[i].UCID,Msg);
                            send_mtc(ginfo.players[i].UCID,ginfo.fines[atoi(pack_btt->Text)].name);

                            strcpy(Msg,msg.message[ginfo.players[i].lang_id][1106]);
                            strcat(Msg,ginfo.players[g].PName);
                            send_mtc(ginfo.players[i].UCID,Msg);


                            for (int j=0; j<MAX_FINES; j++)
                            {
                                if (ginfo.players[g].fines[j].fine_id == 0)
                                {
                                    ginfo.players[g].fines[j].fine_id = atoi(pack_btt->Text);
                                    ginfo.players[g].fines[j].fine_date = int(time(&stime));
                                    break;
                                }
                            }



                            ofstream readf (fine_c,ios::app);
                            readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo.players[i].UName << " get fine ID = " << pack_btt->Text << " to "  << ginfo.players[g].UName << endl;
                            readf.close();


                        } // if atoi(pack_btt->Text) > 0
                        break;
                    }
                }//for

            }
            else if (pack_btt->ClickID==39)
            {

                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo.players[i].BID2 == ginfo.players[g].BID)
                    {
                        if (atoi(pack_btt->Text) > 0)
                        {

                            out << ginfo.players[i].UName << " cancle fine id = " << pack_btt->Text << " to "  << ginfo.players[g].UName << endl;

                            char Msg[64];
                            strcpy(Msg,msg.message[ginfo.players[g].lang_id][1107]);
                            send_mtc(ginfo.players[g].UCID,Msg);
                            strcpy(Msg,"^2| ");
                            strcat(Msg,ginfo.fines[atoi(pack_btt->Text)].name);
                            send_mtc(ginfo.players[g].UCID,Msg);

                            strcpy(Msg,msg.message[ginfo.players[i].lang_id][1108]);
                            send_mtc(ginfo.players[i].UCID,Msg);
                            send_mtc(ginfo.players[i].UCID,ginfo.fines[atoi(pack_btt->Text)].name);

                            strcpy(Msg,msg.message[ginfo.players[g].lang_id][1106]);
                            strcat(Msg,ginfo.players[g].PName);
                            send_mtc(ginfo.players[i].UCID,Msg);


                            for (int j=0; j<MAX_FINES; j++)
                            {
                                if (ginfo.players[g].fines[j].fine_id == atoi(pack_btt->Text))
                                {
                                    ginfo.players[g].fines[j].fine_id = 0;
                                    ginfo.players[g].fines[j].fine_date = 0;
                                    break;
                                }
                            }



                            ofstream readf (fine_c,ios::app);
                            readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo.players[i].UName << " cancle fine ID = " << pack_btt->Text << " to "  << ginfo.players[g].UName << endl;
                            readf.close();


                        } // if atoi(pack_btt->Text) > 0
                        break;
                    }
                }//for

            }

            /* else if (pack_btt->ClickID == 74)
             {
                 if (strlen(pack_btt->Text) > 0)
                 {
                     if (bank.players[i].Cash > 5000)
                     {
                         bank.players[i].Cash -= 5000;
                         strcpy(ginfo.players[i].SName,pack_btt->Text);

                         char Msg[64];
                         strcpy(Msg,"^1| ^C^7Вы купили окраску^7");
                         strcat(Msg,pack_btt->Text);
                         send_mtc(ginfo.players[i].UCID,Msg);
                         send_mtc(ginfo.players[i].UCID,"^1| ^C^7Проедте в гараж и поменяйте окраску.");


                     }
                     else
                     {
                         send_mtc(ginfo.players[i].UCID,"^1| ^C^7На вашем счету недостаточно средств.");
                     }
                 } // if atoi(pack_btt->Text) > 0
             } */

        }
    }
}


void case_cnl ()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim.get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].UCID == pack_cnl->UCID)
        {
            if (pack_cnl->Reason != LEAVR_DISCO)
            {
                //bank.players[i].Cash += 500;
            }

            //save_user(&ginfo.players[i]);
            save_user_cars(&ginfo.players[i]);
            save_user_fines(&ginfo.players[i]);

            out << ginfo.players[i].UName << " left server" << endl;
            memset(&ginfo.players[i],0,sizeof(struct player));
            break;
        }
    }
}


void case_cpr ()
{
    int i;

    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim.get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].UCID == pack_cpr->UCID)
        {
            out << ginfo.players[i].PName << " rename to " << pack_cpr->PName << endl;
            strcpy(ginfo.players[i].PName, pack_cpr->PName);
            // убираем ЛФС коды ^1 ^C ^L ^7 и т.д.
            char PlayerName[32];
            strcpy(PlayerName,pack_cpr->PName);
            char * pch;
            while (pch = strstr(PlayerName,"^"))
            {
                int point = strlen(pch);
                //out << pch << endl;
                strcpy(PlayerName+strlen(PlayerName)-point,pch+2);
            }
            out << PlayerName << endl;
            if (ginfo.players[i].PLID != 0)
            {
                if (((strncmp("[ДПС]",PlayerName,5)==0) or (strncmp("[дпс]",PlayerName,5)==0)) and (ginfo.players[i].cop !=1))
                {
                    if ( read_cop(&ginfo.players[i]) != 1)
                    {
                        send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][1300]);
                        char Text[64];
                        strcpy(Text, "/spec ");
                        strcat (Text, ginfo.players[i].UName);
                        //ginfo.players[i].PLID =0;
                        send_mst(Text);
                    }
                    else
                    {
                        ginfo.players[i].cop =1;
                        send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][1301]);
                    }
                }
                else
                {
                    if (ginfo.players[i].cop == 1)
                    {
                        ginfo.players[i].cop =0;
                        send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][1302]);
                    }
                }
            }

            break;
        }
    }
}




void case_flg ()
{
    int i;

    struct IS_FLG *pack_flg = (struct IS_FLG*)insim.get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].PLID == pack_flg->PLID)
        {
            break;
        }
    }
}
/**
void case_lap ()
{
    out << "IS_LAP\n";
    struct IS_LAP *pack_lap = (struct IS_LAP*)insim.get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].PLID == pack_lap->PLID)
        {


            int bonus = 100+(50*(pack_lap->LapsDone-1));
            bank.players[i].Cash += bonus;
            char bonus_c[64];
            strcpy(bonus_c,msg.message[ginfo.players[i].lang_id][1500]);
            char bonus_ic[5];
            itoa(bonus,bonus_ic,10);
            strcat(bonus_c,bonus_ic);
            strcat(bonus_c," ^7RUR.");
            send_mtc(ginfo.players[i].UCID,bonus_c);

            break;
        }

    }
}
**/
void case_mci ()
{
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim.udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++)

    {
        for (int j =0; j < MAX_PLAYERS; j++)
        {

            if (pack_mci->Info[i].PLID == ginfo.players[j].PLID and ginfo.players[j].PLID != 0 and ginfo.players[j].UCID != 0)

            {

                int Node = pack_mci->Info[i].Node;

                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;
                int Z = pack_mci->Info[i].Z/65536;

                int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

                int X1 = ginfo.players[j].Info.X2/65536;
                int Y1 = ginfo.players[j].Info.Y2/65536;
                int Z1 = ginfo.players[j].Info.Z2/65536;

                if (X1==0 and Y1==0 and Z1==0)
                {
                    X1=X;
                    Y1=Y;
                    Z1=Z;
                    ginfo.players[j].Info.X2 = pack_mci->Info[i].X;
                    ginfo.players[j].Info.Y2 = pack_mci->Info[i].Y;
                    ginfo.players[j].Info.Z2 = pack_mci->Info[i].Z;
                }

                float Dist = sqrt(pow((X-X1),2)+pow((Y-Y1),2)+pow((Z-Z1),2));
                if (ginfo.players[j].cop != 1)
                {
                    if ((abs((int)Dist) > 10) and (S>30) and (S<150))
                    {
                        ginfo.players[j].Distance += abs((int)Dist);
                        bank.players[j].Cash += abs((int)Dist)/10;
                        //bank.BankFond -= abs((int)Dist)/10;

                        ginfo.players[j].Info.X2 = pack_mci->Info[i].X;
                        ginfo.players[j].Info.Y2 = pack_mci->Info[i].Y;
                        ginfo.players[j].Info.Z2 = pack_mci->Info[i].Z;
                    }
                }

                /** Bonus **/

                if ( (Node <= ginfo.Node_Split1+5) and (Node >= ginfo.Node_Split1-5) and (ginfo.Node_Split1 != 0) )
                {
                    if (ginfo.players[j].Bonus_s1 == 0)
                    {
                        ginfo.players[j].Bonus_s1 = 1;
                    }
                }

                else if ( (Node <= ginfo.Node_Split2+5) and (Node >= ginfo.Node_Split2-5) and (ginfo.Node_Split2 != 0) )
                {
                    if (ginfo.players[j].Bonus_s2 == 0)
                    {
                        ginfo.players[j].Bonus_s2 = 1;
                    }

                }

                else if ( (Node <= ginfo.Node_Split3+5) and (Node >= ginfo.Node_Split3-5) and (ginfo.Node_Split3 != 0) )
                {
                    if (ginfo.players[j].Bonus_s3 == 0)
                    {
                        ginfo.players[j].Bonus_s3 = 1;
                    }
                }


                if ((Node <= ginfo.Node_Finish+5) and (Node >= ginfo.Node_Finish-5) and (ginfo.Node_Finish != 0))

                {
                    int sum = ginfo.players[j].Bonus_s1 + ginfo.players[j].Bonus_s2 + ginfo.players[j].Bonus_s3;
                    if(sum == ginfo.Splits_Count)
                    {

                        int bonus = 100+(50*(ginfo.players[j].Bonus_laps));
                        ginfo.players[j].Bonus_laps +=1;
                        //ginfo.players[j].Bonus_key = 0;
                        ginfo.players[j].Bonus_s1 = 0;
                        ginfo.players[j].Bonus_s2 = 0;
                        ginfo.players[j].Bonus_s3 = 0;

                        bank.players[j].Cash += bonus;
                        //bank.BankFond -= bonus;

                        char bonus_c[64];
                        strcpy(bonus_c,msg.message[ginfo.players[j].lang_id][1500]);
                        char bonus_ic[5];
                        itoa(bonus,bonus_ic,10);
                        strcat(bonus_c,bonus_ic);
                        strcat(bonus_c," ^7RUR.");
                        send_mtc(ginfo.players[j].UCID,bonus_c);
                    }
                    //
                }



                /** streets  **/


                for (int g=0; g<ginfo.players[j].str_count; g++)
                {
                    if (Node >= ginfo.players[j].street[g].NodeBeg and Node <= ginfo.players[j].street[g].NodeEnd )
                    {
                        ginfo.players[j].StreetNum = g;
                        //strcpy(ginfo.players[j].Street,"^C^7");
                        //strcat(ginfo.players[j].Street,ginfo.players[j].street[g].Street);
                    }
                }


                /**  steets **/

                /** Zones (PitSave, shop, etc) **/


                if (Check_Pos(ginfo.players[j].TrackInf.PitCount,ginfo.players[j].TrackInf.XPit,ginfo.players[j].TrackInf.YPit,X,Y))
                {
                    ginfo.players[j].Zone = 1;
                }
                else if (Check_Pos(ginfo.players[j].TrackInf.ShopCount,ginfo.players[j].TrackInf.XShop,ginfo.players[j].TrackInf.YShop,X,Y))
                {
                    ginfo.players[j].Zone = 2;
                }
                else
                {
                    ginfo.players[j].Zone = 0;
                }

                ginfo.players[j].Info.X = pack_mci->Info[i].X;
                ginfo.players[j].Info.Y = pack_mci->Info[i].Y;
                ginfo.players[j].Info.Node = pack_mci->Info[i].Node;
                ginfo.players[j].Info.Direction = pack_mci->Info[i].Direction;
                ginfo.players[j].Info.Heading = pack_mci->Info[i].Heading;
                ginfo.players[j].Info.Speed = pack_mci->Info[i].Speed;
                ginfo.players[j].Info.AngVel = pack_mci->Info[i].AngVel;
                //btn_svetofor3(&ginfo.players[i]);

            } // if pack_mci->Info[i].PLID == ginfo.players[j].PLID
        }
    }
}

void case_mci_cop ()
{
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim.udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++)

    {
        for (int j =0; j < MAX_PLAYERS; j++)
        {
            if (pack_mci->Info[i].PLID == ginfo.players[j].PLID and ginfo.players[j].PLID != 0 and ginfo.players[j].UCID != 0)

            {
                int S = ginfo.players[j].Info.Speed*360/32768;
                if (S < 5)
                    ginfo.players[j].StopTime ++;
                else
                {
                    ginfo.players[j].StopTime = 0;
                    if (ginfo.players[j].radar ==1 )
                    {
                        send_mtc(ginfo.players[j].UCID,msg.message[ginfo.players[j].lang_id][1700] );
                        ginfo.players[j].radar = 0;
                    }
                }



                if (ginfo.players[j].cop == 1)
                {
                    btn_svetofor3(&ginfo.players[j]);

                    for (int g =0; g < MAX_PLAYERS; g++)
                    {
                        if (ginfo.players[g].PLID !=0)
                        {
                            // int dN = abs(ginfo.players[g].Info.Node - ginfo.players[j].Info.Node);
                            int dX = abs((ginfo.players[g].Info.X/65536) - (ginfo.players[j].Info.X/65536));
                            int dY = abs((ginfo.players[g].Info.Y/65536) - (ginfo.players[j].Info.Y/65536));

                            int Rast = (int)sqrt((pow(dX,2))+(pow(dY,2)));

                            //printf("dX=%d ; dY=%d \n",dX,dY);
                            if (ginfo.players[g].Pogonya == 1)
                            {



                                //printf("COP - %s CRIM - %s dX=%d dY=%d; \n",ginfo.players[j].UName,ginfo.players[g].UName,dX,dY);

                                if ( (Rast < 10) and (ginfo.players[g].cop != 1))
                                {
                                    int S2 = ginfo.players[g].Info.Speed*360/32768;

                                    if ((S2 < 5) and (ginfo.players[g].StopTime > 6))
                                    {
                                        //send_mst("/rcm ^C^1ВЫ АРЕСТОВАНЫ");
                                        ginfo.players[g].Pogonya = 2;
                                        strcpy(ginfo.players[g].PogonyaReason,msg.message[ginfo.players[g].lang_id][1701]);
                                        char Text[96];
                                        strcpy(Text,"/msg ^2| ");
                                        strcat(Text,ginfo.players[g].PName);
                                        strcat(Text,msg.message[ginfo.players[j].lang_id][1702]);
                                        //strcat(Text,ginfo.players[g].PName);
                                        send_mst(Text);

                                        send_mtc(ginfo.players[j].UCID,msg.message[ginfo.players[j].lang_id][1703]);
                                    }

                                }
                            } // pogonya



                            if (ginfo.players[j].radar ==1 )
                            {
                                if ((Rast < 50 ) and (ginfo.players[g].cop != 1))
                                {
                                    //printf("COP - %s CRIM - %s dN=%d \n",ginfo.players[j].UName,ginfo.players[g].UName,dN);
                                    // player[g] in radar zone of player[j]
                                    int Speed = ginfo.players[g].Info.Speed*360/32768;
                                    if ((Speed > ginfo.players[j].street[ginfo.players[g].StreetNum].SpeedLimit+10) )
                                    {
                                        // speed > SpeedLimit + 10
                                        char text[64];
                                        strcpy(text,"^2| ");
                                        strcat(text,ginfo.players[g].PName);
                                        strcat(text,msg.message[ginfo.players[g].lang_id][1704]);
                                        char speed[3];
                                        int Speed2 = Speed - ginfo.players[j].street[ginfo.players[g].StreetNum].SpeedLimit;
                                        itoa(Speed2,speed,10);
                                        strcat(text,speed);
                                        strcat(text,msg.message[ginfo.players[j].lang_id][1705]);
                                        send_mtc(ginfo.players[j].UCID,text);
                                    }
                                }
                            }

                            if (ginfo.players[j].sirena ==1)
                            {
                                if ((dX < 120) and (dY < 120 ) and (ginfo.players[g].cop != 1))
                                {
                                    ginfo.players[g].sirenaOnOff = 1;
                                    ginfo.players[g].sirenaKey = 1;
                                    ginfo.players[g].sirenaSize = Rast;
                                }
                                else
                                {
                                    ginfo.players[g].sirenaOnOff = 0;
                                }
                            }
                            else
                            {
                                ginfo.players[g].sirenaOnOff = 0;
                            }

                        }
                    }
                }


                if ((ginfo.players[j].sirenaOnOff == 0) and (ginfo.players[j].sirenaKey == 1))
                {
                    ginfo.players[j].sirenaKey = 0;
                    send_bfn(ginfo.players[j].UCID,203);
                }

                if ( ginfo.players[j].sirenaOnOff == 1)
                    btn_sirena(&ginfo.players[j]);

                if ((ginfo.players[j].Pogonya == 0) and (strlen(ginfo.players[j].PogonyaReason) > 1))
                {
                    strcpy(ginfo.players[j].PogonyaReason,"");
                    send_bfn(ginfo.players[j].UCID,204);
                }

                if ( ginfo.players[j].Pogonya != 0)
                    btn_pogonya(&ginfo.players[j]);


            } // if CompCar->PLID == PLID
        }
    }
}



void case_mci_svetofor ()
{
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim.udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++)

    {
        for (int j =0; j < MAX_PLAYERS; j++)
        {

            if (pack_mci->Info[i].PLID == ginfo.players[j].PLID)

            {
                //out << "MCI packet for " << ginfo.players[j].UName << endl;

                int Node = pack_mci->Info[i].Node;
                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;
                int H = pack_mci->Info[i].Heading/182;
                //int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

                int SvetKey = 0;

                if (strcmp(ginfo.Track,"AS5") == 0)
                {
                    //out << ginfo.Track << endl;
                    // ZONE 1 PIT 1
                    // from 1 pit 2 svetofot
                    int Pit1X[5] = {-605,-624,-624,-605};
                    int Pit1Y[5] = {1,1,10,10};
                    int Pit2X[5] = {-605,-624,-624,-605};
                    int Pit2Y[5] = {-510,-510,-500,-500};
                    int CafeX[5] = {974,984,984,974};
                    int CafeY[5] = {-1027,-1027,-1039,-1039};
                    int Cafe2X[6] = {974,967,947,942,962};
                    int Cafe2Y[6] = {-1039,-1066,-1086,1075,-1040};

                    if ((Node > 12) and (Node < 32) and (X > -610) and (H > 140) and (H<185))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Node > 0) and (Node < 12) and (X > -610) and ((H > 350) or (H < 10)))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Check_Pos(4,Pit1X,Pit1Y,X,Y)) and (H > 260) and (H < 360))
                    {
                        SvetKey = 2;
                        ginfo.players[j].Svetofor = 2;
                    }
                    // !-- ZONE 1 PIT 1
                    // ZONE 2 PIT 2
                    if ((Node > 726) and (Node < 738) and (X > -610) and (H > 140) and (H<185))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Node > 713) and (Node < 727) and (X > -610) and ((H > 350) or (H < 10)))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Check_Pos(4,Pit2X,Pit2Y,X,Y)) and (H > 200) and (H < 360))
                    {
                        SvetKey = 2;
                        ginfo.players[j].Svetofor = 2;
                    }
                    // !-- ZONE 2 PIT 2
                    // ZONE 3 RCPizza CAFE
                    if ((Node > 377) and (Node < 390) and (Check_Pos(5,Cafe2X,Cafe2Y,X,Y)) and ((H > 300) or (H<10)))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Node > 360) and (Node < 377) and (X < 975) and (H > 140) and (H < 185))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Check_Pos(4,CafeX,CafeY,X,Y)) and (H > 0) and (H < 100))
                    {
                        SvetKey = 2;
                        ginfo.players[j].Svetofor = 2;
                    }
                    // !-- ZONE 3
                    // ZONE 4
                    if ((Node > 223) and (Node < 235) and (H > 170) and (H< 280))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Node > 238) and (Node < 262) and (H > 60) and (H< 100))
                    {
                        SvetKey = 2;
                        ginfo.players[j].Svetofor = 2;
                    }
                    // !-- ZONE 4

                    // ZONE 5
                    if ((Node > 534) and (Node < 549) and (H > 100) and (H< 170))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Node > 549) and (Node < 560) and ((H > 340) or (H< 10)))
                    {
                        SvetKey = 2;
                        ginfo.players[j].Svetofor = 2;
                    }
                    // !-- ZONE 5

                }// AS5

                else if (strcmp(ginfo.Track,"FE4") == 0)
                {
                    //out << ginfo.Track << endl;
                    // ZONE 1 PIT 1
                    // from 1 pit 2 svetofot
                    int Pit1X[5] = {179,189,189,179};
                    int Pit1Y[5] = {-15,-15,-23,-23};

                    int Pit2X[5] = {173,190,190,173};
                    int Pit2Y[5] = {210,210,205,205};

                    //int CafeX[5] = {974,984,984,974};
                    //int CafeY[5] = {-1027,-1027,-1039,-1039};

                    //int Cafe2X[6] = {974,967,947,942,962};
                    //int Cafe2Y[6] = {-1039,-1066,-1086,1075,-1040};

                    if ((Node > 220) and (Node < 231) and (X < 180) and (H > 130) and (H<230))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Node > 210) and (Node < 221) and (X < 180) and ((H > 310) or (H < 50)))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Check_Pos(4,Pit1X,Pit1Y,X,Y)) and (H > 75-50) and (H < 75+50))
                    {
                        SvetKey = 2;
                        ginfo.players[j].Svetofor = 2;
                    }
                    // !-- ZONE 1 PIT 1
                    // ZONE 2 PIT 2
                    if ((Node > 246) and (Node < 260) and (X < 180) and (H > 180-50) and (H<180+50))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Node > 237) and (Node < 247) and (X < 180) and ((H > 360-50) or (H < 50)))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Check_Pos(4,Pit2X,Pit2Y,X,Y)) and (H > 90-50) and (H < 90+50))
                    {
                        SvetKey = 2;
                        ginfo.players[j].Svetofor = 2;
                    }
                    // !-- ZONE 2 PIT 2

                    // ZONE 3 RCPizza CAFE
                    if ((Node > 540) and (Node < 550) and (X > -32) and ((H > 300) or (H<10)))
                    {
                        SvetKey = 1;
                        ginfo.players[j].Svetofor = 1;
                    }
                    if ((Node > 550) and (Node < 560) and (X > -32) and (H > 140) and (H < 185))
                    {
                        SvetKey = 2;
                        ginfo.players[j].Svetofor = 2;
                    }
                    // !-- ZONE 3
                    // ZONE 4

                    // !-- ZONE 4

                    // ZONE 5

                    // !-- ZONE 5


                }// FE4

                else if (strcmp(ginfo.Track,"SO4") == 0)
                {
                    /**********светофоры*********/

                    /***************** 1 zone ***************/

//1.1
                    if ((Node>=526) or (Node>=0 and Node<=3 )and X<-140)
                    {
                        if (H>180 and H<220)
                        {
                            SvetKey = 1;
                            ginfo.players[j].Svetofor = 1;
                        }

                    }
//2.2
                    if (Node >= 6 and Node <=13 )
                    {
                        if (H>0 and H<40)
                        {
                            SvetKey = 1;
                            ginfo.players[j].Svetofor = 1;
                        }

                    }
                    if (Node >= 3 and Node <6)
                    {
                        if (X>=-151 and X<-140)
                        {
                            if (H>60 and H<130)
                            {
                                SvetKey = 2;
                                ginfo.players[j].Svetofor = 2;
                            }
                        }

                    }
                    /******************************************/

                    /***************** 2 zone ***************/
//3.1
                    if (Node>=13 and Node<24 )
                    {
                        if (H>160 and H<200)
                        {
                            SvetKey = 1;
                            ginfo.players[j].Svetofor = 1;
                        }

                    }
//4.2
                    if ( Node>27 and Node<=40 )
                    {
                        if (H>250 and H<290)
                        {
                            SvetKey = 2;
                            ginfo.players[j].Svetofor = 2;
                        }
                    }




                    /********************************/

                    /***************** 3 zone ***************/
//5.1
                    if (Node>83 and Node<=93 )
                    {
                        if (H>70 and H<100)
                        {
                            SvetKey = 1;
                            ginfo.players[j].Svetofor = 1;
                        }

                    }
//6.2
                    if ( Node>=97 and Node<102 )
                    {
                        if (H<20 or H>300)
                        {
                            SvetKey = 2;
                            ginfo.players[j].Svetofor = 2;
                        }

                    }
                    if ( Node>=102 and Node<106 )
                    {
                        if (H>260)
                        {
                            SvetKey = 2;
                            ginfo.players[j].Svetofor = 2;
                        }

                    }


                    /********************************/

                    /***************** 4 zone ***************/
//7.1
                    if (Node>188 and Node<208 )
                    {
                        if (H>280 and H<320)
                        {
                            SvetKey = 1;
                            ginfo.players[j].Svetofor = 1;
                        }

                    }
//8.2
                    if ( Node>=214 and Node<224 )
                    {
                        if ( H<20 or H>350)
                        {
                            SvetKey = 2;
                            ginfo.players[j].Svetofor = 2;
                        }

                    }


                    /********************************/


                    /***************** 5 zone ***************/
                    if (Node>405 and Node<422 )
                    {
                        if (H<60 or H>351)
                        {
                            SvetKey = 1;
                            ginfo.players[j].Svetofor = 1;
                        }

                    }

                    /***/

//8.2
                    if ( Node>=433 and Node<440 and X>=250)
                    {
                        if ( H>160 and H<190)
                        {
                            SvetKey = 1;
                            ginfo.players[j].Svetofor = 1;
                        }
                    }

                    if ( Node>=428 and Node<433 and X>=244)
                    {
                        if ( H>160 and H<190)
                        {
                            SvetKey = 1;
                            ginfo.players[j].Svetofor = 1;
                        }

                    }

                    if ( Node>=425 and Node<428 and X>=240)
                    {
                        if ( H>160 and H<190)
                        {
                            SvetKey = 1;
                            ginfo.players[j].Svetofor = 1;
                        }

                    }

                    if ( Node>=425 and Node<=428 and X<240)
                    {
                        if ( H>160 and H<190)
                        {
                            SvetKey = 2;
                            ginfo.players[j].Svetofor = 2;
                        }
                    }

                }// SO4

                //SvetKey = 1;

                if (SvetKey == 1)
                {
                    btn_svetofor1(&ginfo.players[j]);
                }
                else if (SvetKey == 2)
                {
                    btn_svetofor2(&ginfo.players[j]);
                }
                else
                {
                    if (ginfo.players[j].Svetofor != 0)
                    {
                        for (int f=190; f < 203; f++)
                            send_bfn(ginfo.players[j].UCID,f);

                        ginfo.players[j].Svetofor = 0;
                        //out << "clear svetofor" << endl;
                    }
                }

            } // if pack_mci->Info[i].PLID == ginfo.players[j].PLID
        }
    }
}



void case_mso ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim.get_packet();

    // The chat message is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
    {
        // out << "(Chat message by host: " << pack_mso->Msg + ((unsigned char)pack_mso->TextStart) << ")" << endl;
        return;

    }

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].UCID == pack_mso->UCID)
        {
            //  out << "Msg: " << pack_mso->Msg << endl;
            break;
        }
    }

    //!help
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!info", 5) == 0)
    {
        out << ginfo.players[i].UName << " send !info" << endl;
        for (int j=159; j>0; j--)
        {
            send_bfn(ginfo.players[i].UCID,j);
            Sleep(10);
        }
        btn_info(&ginfo.players[i],1);
    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!help", 5) == 0)
    {
        out << ginfo.players[i].UName << " send !help" << endl;
        help_cmds(&ginfo.players[i],1);
    }
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!cars", 5) == 0)
    {
        out << ginfo.players[i].UName << " send !cars" << endl;
        help_cmds(&ginfo.players[i],2);
    }

    //!save
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!save", 5) == 0)
    {
        out << ginfo.players[i].UName << " send !save" << endl;
        save_car(&ginfo.players[i]);
        Sleep(500);
        save_user_cars(&ginfo.players[i]);
        save_user_fines(&ginfo.players[i]);

        bank.bank_save(i);
        nrg.energy_save(i);

        Sleep(500);
        send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][3000]);

    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!trans", 6) == 0 )
    {
        out << ginfo.players[i].UName << " send !trans" << endl;


        char message[96];
        strcpy(message,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));

        char file[255];
        strcpy(file,RootDir);
        strcat(file,"logs\\sends\\send.txt");

        HANDLE fff;
        WIN32_FIND_DATA fd;
        fff = FindFirstFile(file,&fd);
        if (fff == INVALID_HANDLE_VALUE)
        {
            out << "Can't find " << file << endl;
            return;
        }
        FindClose(fff);

        ifstream readf (file,ios::in);



        if (strlen(message) < 8)
        {
            // username not exist
            while (readf.good())
            {
                char str[128];
                readf.getline(str,128);
                if (strlen(str) > 0)
                {
                    if (strstr(str,ginfo.players[i].UName))
                    {
                        char Text[64];
                        strcpy(Text,"^1| ^C^7");
                        strncat(Text,str,55);
                        send_mtc(ginfo.players[i].UCID,Text);
                    }
                }
            }
        }
        else
        {
            // username exist
            char * comand;
            char * user;
            comand = strtok (message," ");
            user = strtok (NULL," ");

            while (readf.good())
            {
                char str[128];
                readf.getline(str,128);
                if (strlen(str) > 0)
                {
                    if ((strstr(str,ginfo.players[i].UName)) and (strstr(str,user)))
                    {
                        char Text[64];
                        strcpy(Text,"^1| ^C^7");
                        strncat(Text,str,55);
                        send_mtc(ginfo.players[i].UCID,Text);
                    }
                }
            }

        }

        readf.close();

    }

    //!shop
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!shop", 4) == 0)
    {
        out << ginfo.players[i].UName << " send !shop" << endl;
        if (ginfo.players[i].Zone == 2)
        {
            for (int j=0; j<200; j++)
                send_bfn(ginfo.players[i].UCID,j);

            ginfo.players[i].Shop=1;
            btn_shop(&ginfo.players[i]);
        }
        else
        {
            send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2000]);
        }
    }


    //!EXIT
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!exit", 5) == 0 and strcmp(ginfo.players[i].UName, "denis-takumi") == 0)
    {
        out << ginfo.players[i].UName << " send !exit" << endl;
        for (int j=0; j<MAX_PLAYERS; j++)
        {

            if (ginfo.players[j].UCID !=0 )
            {
                save_car(&ginfo.players[j]);

                save_user_cars(&ginfo.players[j]);
                save_user_fines(&ginfo.players[j]);
                bank.bank_save(j);
                nrg.energy_save(j);
            }
        }
        ok=0;
    }
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!reload", 7) == 0 and strcmp(ginfo.players[i].UName, "denis-takumi") == 0)
    {
        out << ginfo.players[i].UName << " send !reload" << endl;

        struct IS_TINY pack_requests;
        memset(&pack_requests, 0, sizeof(struct IS_TINY));
        pack_requests.Size = sizeof(struct IS_TINY);
        pack_requests.Type = ISP_TINY;
        pack_requests.ReqI = 1;

        pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
        insim.send_packet(&pack_requests);
    }
    //!evo
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!pit", 4) == 0)
    {

        char Msg[64];
        strcpy(Msg, "/pitlane ");
        strcat(Msg,ginfo.players[i].UName);
        send_mst(Msg);
        bank.players[i].Cash -=250;
        bank.BankFond += 250;
    }
    //!users
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!users",6) == 0)
    {
        ginfo.players[i].Action = 1;

        struct IS_BTN pack_btn;
        memset(&pack_btn, 0, sizeof(struct IS_BTN));
        pack_btn.Size = sizeof(struct IS_BTN);
        pack_btn.Type = ISP_BTN;
        pack_btn.ReqI = ginfo.players[i].UCID;              // Must be non-zero, I'll just use UCID
        pack_btn.UCID = ginfo.players[i].UCID;              // UCID of the player that will receive the button
        pack_btn.BStyle = 16 + ISB_CLICK;                 // Dark frame for window title
        pack_btn.L = 162;
        pack_btn.T = 1;
        pack_btn.W = 34;
        pack_btn.H = 5;
        for (int j=0; j<MAX_PLAYERS; j++)
        {
            if (ginfo.players[j].UCID!=0)
            {
                pack_btn.ClickID = ginfo.players[j].BID;
                strcpy(pack_btn.Text, ginfo.players[j].PName);
                insim.send_packet(&pack_btn);
                pack_btn.T += 6;
                Sleep(100);
            }

        }
        pack_btn.ClickID = 34;
        strcpy(pack_btn.Text,msg.message[ginfo.players[i].lang_id][2003]);
        insim.send_packet(&pack_btn);

    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!lang", 5) == 0 )
    {
        out << ginfo.players[i].UName << " send !lang" << endl;

        char message2[96];
        strcpy(message2,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));

        if (strlen(message2) < 8)
        {
            send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2104]);
            return;
        }

        char * comand;
        char * id;

        comand = strtok (message2," ");
        id = strtok (NULL," ");

        if ((!id) or (strlen(id) != 3))
        {
            send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2105]);
            return;
        }

        if (strstr("* eng rus",id) == 0)
        {
            return;
        }

        strcpy(ginfo.players[i].Lang,id);
        //read_lang(&ginfo.players[i]);
        if (strcmp(ginfo.players[i].Lang,"eng") == 0)
        {
            ginfo.players[i].lang_id = 1;
        }
        else
        {
            ginfo.players[i].lang_id = 0;
        }

        //read_track(&ginfo.players[i]);

    }

    /**  FLOOD ***/
    if (pack_mso->UserType != MSO_PREFIX) // if msg not with prefix '!'
    {

        time_t timef;
        int ftime = time(&timef); // get current time

        if (ginfo.players[i].FloodTime == 0)
        {
            ginfo.players[i].FloodTime = ftime;
        }

        int ts = ftime - ginfo.players[i].FloodTime;
        ginfo.players[i].FloodTime = ftime;


        if (ts < 5)
            ginfo.players[i].FloodCount++;
        else
        {
            ginfo.players[i].FloodCount = 1;
        }

        if (ginfo.players[i].FloodCount > 4)   //max lines to tolerate
        {
            send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2004]);
            ginfo.players[i].FloodCount = 0;
            bank.players[i].Cash -= 500;
            bank.BankFond += 500;
        }


        /*** МАТ И Т.П. ***/
        read_words();

        char Msg[96];
        strcpy(Msg,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));

        //strupr()

        for (int j=0; j< ginfo.WordsCount; j++)
        {
            if (strstr(Msg,ginfo.Words[j]))
            {
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2005]);
                bank.players[i].Cash -= 1000;
                bank.BankFond += 1000;
            }

        }
        strcpy(ginfo.players[i].Msg,Msg);
    } // if UserType != MSO_PREFIX
    /** Flood and bad words **/
}

void case_mso_cop ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim.get_packet();

    // The chat message is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
    {
        // out << "(Chat message by host: " << pack_mso->Msg + ((unsigned char)pack_mso->TextStart) << ")" << endl;
        return;

    }

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].UCID == pack_mso->UCID)
        {
            //  out << "Msg: " << pack_mso->Msg << endl;
            break;
        }
    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!sirena", 7) == 0 )
    {
        out << ginfo.players[i].UName << " send !sirena" << endl;

        if (ginfo.players[i].cop == 1)
        {
            if (ginfo.players[i].sirena ==0 )
            {
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2100]);
                ginfo.players[i].sirena = 1;
            }
            else
            {
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2101]);
                ginfo.players[i].sirena = 0;
            }
        }
    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!radar", 6) == 0 )
    {
        out << ginfo.players[i].UName << " send !radar" << endl;

        if (ginfo.players[i].cop == 1)
        {
            if (ginfo.players[i].radar ==0 )
            {
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2102]);
                ginfo.players[i].radar = 1;
            }
            else
            {
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2103]);
                ginfo.players[i].radar = 0;
            }
        }
    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!fines", 6) == 0)
    {
        out << ginfo.players[i].UName << " send !fines" << endl;
        help_cmds(&ginfo.players[i],3);
    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!pay", 4) == 0 )
    {
        out << ginfo.players[i].UName << " send !pay" << endl;

        char message2[96];
        strcpy(message2,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));

        if (strlen(message2) < 6)
        {
            send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2104]);
            return;
        }

        char * comand;
        char * id;

        comand = strtok (message2," ");
        id = strtok (NULL," ");


        int id_i = atoi(id);

        if ((!id) or (id_i < 1))
        {
            send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2105]);
            return;
        }
        int j=0;
        for (j=0; j<MAX_FINES; j++)
        {
            if (ginfo.players[i].fines[j].fine_id == id_i)
            {
                ginfo.players[i].fines[j].fine_id = 0;
                ginfo.players[i].fines[j].fine_date = 0;
                bank.players[i].Cash -= ginfo.fines[id_i].cash;
                bank.BankFond += ginfo.fines[id_i].cash;
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2106]);
                break;
            }
            if (j == MAX_FINES)
            {
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2107]);
            }
        }

    }

}




void case_ncn ()
{
    int i;

    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim.get_packet();

    //out << "ReqI = " << (int)pack_ncn->ReqI << endl;


    if (pack_ncn->UCID == 0)
    {
        // out << "(Host connected, not adding him to array...)" << endl << endl;
        return;
    }



    for (i=0; i<MAX_PLAYERS; i++)
        if (ginfo.players[i].UCID == 0)
            break;


    if (i == MAX_PLAYERS)
    {
        //  out << "NO MORE PEOPLE ALLOWED!!!" << endl << endl;
        return;
    }

    // Copy all the player data we need into the ginfo.players[] array
    strcpy(ginfo.players[i].UName, pack_ncn->UName);
    strcpy(ginfo.players[i].PName, pack_ncn->PName);
    ginfo.players[i].UCID = pack_ncn->UCID;
    ginfo.players[i].BID = i;
    ginfo.players[i].Zone = 1;
    //ginfo.players[i].Admin = pack_ncn->Admin;


    out <<  "\n******************************" << endl;
    out <<  ginfo.players[i].UName << " connected" << endl;
    //read_user(&ginfo.players[i]);
    read_user_cars(&ginfo.players[i]);
    read_user_fines(&ginfo.players[i]);
    //read_lang(&ginfo.players[i]);
    read_track(&ginfo.players[i]);
    out <<  "******************************" << endl;

    help_cmds(&ginfo.players[i],1);
    help_cmds(&ginfo.players[i],2);
}

void case_npl ()
{
    // out << "joining race or leaving pits" << endl;
    int i;

    struct IS_NPL *pack_npl = (struct IS_NPL*)insim.get_packet();

    // Find player using UCID and update his PLID
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].UCID == pack_npl->UCID)
        {

            // out << "Core.dll: find user " << ginfo.players[i].UName << endl;
            if (pack_npl->PType != 6)
            {
                char PlayerName[32];
                strcpy(PlayerName,ginfo.players[i].PName);
                char * pch;
                while (pch = strstr(PlayerName,"^"))
                {
                    int point = strlen(pch);
                    //out << pch << endl;
                    strcpy(PlayerName+strlen(PlayerName)-point,pch+2);
                }
                out << PlayerName << endl;

                if (((strncmp("[ДПС]",PlayerName,5)==0) or (strncmp("[дпс]",PlayerName,5)==0)) and (ginfo.players[i].cop !=1))
                {
                    if ( read_cop(&ginfo.players[i]) < 1)
                    {
                        char Text[64];
                        strcpy(Text, "/spec ");
                        strcat (Text, ginfo.players[i].UName);
                        //ginfo.players[i].PLID =0;
                        send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][1300]);
                        send_mst(Text);
                        return;
                    }
                    else
                    {
                        ginfo.players[i].cop =1;
                        send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][1301]);
                    }
                }

                if (ginfo.players[i].cop != 1)
                {

                    char Text[64];
                    strcpy(Text, "/spec ");
                    strcat (Text, ginfo.players[i].UName);

                    ginfo.players[i].PLID = pack_npl->PLID;


                    // out << "pack_npl->PType = " << (int)pack_npl->PType << endl;;

                    int j=0;
                    for (j=0; j<MAX_CARS; j++)
                    {
                        if (strcmp(ginfo.players[i].cars[j].car,pack_npl->CName)==0)
                            break;
                    }

                    if ( j != MAX_CARS)
                    {



                        strcpy(ginfo.players[i].CName ,pack_npl->CName);
                        ginfo.players[i].CTune = ginfo.players[i].cars[j].tuning;
                        ginfo.players[i].Distance = ginfo.players[i].cars[j].dist;
                        //strcpy(ginfo.players[i].SName , row[2]);


                        // out << "Core.dll: free result" << endl;



                        //itoa(ginfo.players[i].CTune,SQL,10);
                        //send_mtc(ginfo.players[i].UCID,SQL);


                        int tune = 45;
                        if (ginfo.players[i].CTune&1)
                            tune -= 15;
                        if (ginfo.players[i].CTune&2)
                            tune -= 20;
                        if (pack_npl->H_TRes < tune)
                        {
                            ginfo.players[i].PLID = 0;
                            ginfo.players[i].Zone = 1;
                            send_mst(Text);
                            char QSL [8];
                            itoa(tune,QSL,10);
                            char Texxt[32];
                            strcpy(Texxt,msg.message[ginfo.players[i].lang_id][2400] );
                            strcat(Texxt,QSL);
                            strcat(Texxt," %");
                            send_mtc(ginfo.players[i].UCID,Texxt);
                        }
                        else if ((pack_npl->SetF&4) and !(ginfo.players[i].CTune&8))
                        {
                            char Texxt[32];
                            strcpy(Texxt,msg.message[ginfo.players[i].lang_id][2401] );
                            send_mtc(ginfo.players[i].UCID,Texxt);
                            ginfo.players[i].Zone = 1;
                            ginfo.players[i].PLID = 0;
                            char Text[64];
                            strcpy(Text, "/spec ");
                            strcat (Text, ginfo.players[i].UName);
                            send_mst(Text);
                            return;
                        }



                    }
                    else
                    {
                        send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2404]);

                        ginfo.players[i].Zone = 1;
                        ginfo.players[i].PLID = 0;
                        send_mst(Text);
                        return;
                    }

                } // if cop !=1
                else
                {
                    ginfo.players[i].PLID = pack_npl->PLID;
                    //ginfo.players[i].EnergyTime = time(&stime);
                }



                break;
            } //if PTupe != 6
            else
            {
                out << "bot detecdet" << endl;
            }
        }//if UCID == UCID
    }//for



}


void case_pen ()
{
    int i;

    struct IS_PEN *pack_pen = (struct IS_PEN*)insim.get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].PLID == pack_pen->PLID)
        {
            if((pack_pen->NewPen != 0) and (pack_pen->Reason == PENR_SPEEDING))
            {
                ginfo.players[i].Penalty = 1;
                for (int j=0; j<MAX_FINES; j++)
                {
                    if (ginfo.players[i].fines[j].fine_id == 0)
                    {
                        ginfo.players[i].fines[j].fine_id = 13;
                        ginfo.players[i].fines[j].fine_date = int(time(&stime));

                        char Msg[64];
                        strcpy(Msg,msg.message[ginfo.players[i].lang_id][1104]);
                        send_mtc(ginfo.players[i].UCID,Msg);
                        strcpy(Msg,"^2| ^7");
                        strcat(Msg,ginfo.fines[13].name);
                        send_mtc(ginfo.players[i].UCID,Msg);
                        break;
                    }
                }
            }
            break;
        }
    }
}

void case_pla ()
{
    int i;

    struct IS_PLA *pack_pla = (struct IS_PLA*)insim.get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].PLID == pack_pla->PLID)
        {
            if (pack_pla->Fact == PITLANE_EXIT)
            {
                if (ginfo.players[i].Penalty != 0)
                {
                    char Text[64];
                    strcpy(Text, "/p_clear ");
                    strcat (Text, ginfo.players[i].UName);
                    send_mst(Text);
                }
                int count = 0;
                for (int j=0; j<MAX_FINES; j++)
                {
                    if (ginfo.players[i].fines[j].fine_id != 0)
                    {
                        count++;
                    }
                }

                if (count > 10)
                {
                    ginfo.players[i].PLID = 0;
                    char Text[64];
                    strcpy(Text, "/spec ");
                    strcat (Text, ginfo.players[i].UName);
                    send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][3400]);
                    send_mst(Text);
                }

            }
            break;
        }
    }
}

void case_pll ()
{
    // out << "player leaves race" << endl;
    int i;

    struct IS_PLL *pack_pll = (struct IS_PLL*)insim.get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].PLID == pack_pll->PLID)
        {
            ginfo.players[i].PLID = 0;
            ginfo.players[i].Info.X = 0;
            ginfo.players[i].Info.Y = 0;
            ginfo.players[i].Info.Z = 0;
            ginfo.players[i].Penalty =0;

            save_car(&ginfo.players[i]);

            if (ginfo.players[i].Pogonya == 1)
            {
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2600]);
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2601]);
                bank.players[i].Cash -= 5000;
                bank.BankFond += 5000;
            }
            else
            {
                if (ginfo.players[i].Zone != 1)
                {
                    send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2602]);
                    send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2603]);
                    bank.players[i].Cash -= 500;
                    bank.BankFond +=500;
                }
            }
            for (int g=0; g<200; g++)
                send_bfn(ginfo.players[i].UCID,g);

            break;
        }
    }
}

void case_plp ()
{
    // out << "player leaves race" << endl;
    int i;

    struct IS_PLP *pack_plp = (struct IS_PLP*)insim.get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].PLID == pack_plp->PLID)
        {
            ginfo.players[i].PLID = 0;
            ginfo.players[i].Info.X = 0;
            ginfo.players[i].Info.Y = 0;
            ginfo.players[i].Info.Z = 0;
            ginfo.players[i].Penalty =0;

            save_car(&ginfo.players[i]);

            if (ginfo.players[i].Pogonya == 1)
            {
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2700]);
                send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2701]);
                bank.players[i].Cash -= 5000;
                bank.BankFond += 5000;
            }
            else
            {
                if (ginfo.players[i].Zone != 1)
                {
                    send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2702]);
                    send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2703]);
                    bank.players[i].Cash -= 500;
                    bank.BankFond += 500;
                }
            }

            for (int g=0; g<200; g++)
                send_bfn(ginfo.players[i].UCID,g);

            break;
        }
    }
}

void case_rst ()
{
    struct IS_RST *pack_rst = (struct IS_RST*)insim.get_packet();
    out << "Race Start Packet" << endl;

    strcpy(ginfo.Track, pack_rst->Track);



    ginfo.Splits_Count = 0;

    if (pack_rst->Split1 <= pack_rst->NumNodes)
    {
        ginfo.Node_Split1 = pack_rst->Split1;
        ginfo.Splits_Count +=1;
    }
    if (pack_rst->Split2 <= pack_rst->NumNodes)
    {
        ginfo.Node_Split2 = pack_rst->Split2;
        ginfo.Splits_Count +=1;
    }
    if (pack_rst->Split3 <= pack_rst->NumNodes)
    {
        ginfo.Node_Split3 = pack_rst->Split3;
        ginfo.Splits_Count +=1;
    }

    ginfo.Node_Finish = pack_rst->Finish;

    /////////
    read_car();
    Sleep(100);
    read_fines();
    Sleep(100);
    pizza.readconfig(pack_rst->Track);
    nrg.readconfig(pack_rst->Track);
    bank.readconfig(pack_rst->Track);
    Sleep(100);
    /////////

    // out << pack_sta->Track << endl << ginfo.Track << endl;
};

void case_vtn ()
{
    struct IS_VTN *pack_vtn = (struct IS_VTN *)insim.get_packet();

    struct IS_TINY pack;
    memset(&pack, 0, sizeof(struct IS_TINY));
    pack.Size = sizeof(struct IS_TINY);
    pack.Type = ISP_TINY;
    pack.ReqI = 1;
    pack.SubT = TINY_VTC;
    insim.send_packet(&pack);

    for (int i=0; i<MAX_PLAYERS; i++)
    {
        if (ginfo.players[i].UCID == pack_vtn->UCID)
        {
            send_mst(msg.message[ginfo.players[i].lang_id][2900]);
            break;
        }
    }




}


int core_connect()
{
    struct IS_VER pack_ver;
    memset(&pack_ver,0,sizeof(struct IS_VER));

    if (insim.init (ginfo.IP, ginfo.TCPPORT, IS_PRODUCT_NAME,ginfo.ADMIN, &pack_ver, '!', ISF_MCI + ISF_MSO_COLS,500, ginfo.UDPPORT) < 0)
    {
        out << "\n * Error during initialization * " << endl;
        return -1;
    }
    else
    {

        strcpy(ginfo.Product,pack_ver.Product);

        struct IS_TINY pack_requests;
        memset(&pack_requests, 0, sizeof(struct IS_TINY));
        pack_requests.Size = sizeof(struct IS_TINY);
        pack_requests.Type = ISP_TINY;
        pack_requests.ReqI = 1;

        pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
        insim.send_packet(&pack_requests);

        pack_requests.SubT = TINY_NCN;      // Request all connections to store their user data
        insim.send_packet(&pack_requests);
        //out << "Connections request packet sent!" << endl;

        pack_requests.SubT = TINY_NPL;      // Request all players in-grid to know their PLID
        insim.send_packet(&pack_requests);
        //out << "Player info request packet sent!" << endl;


        pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
        insim.send_packet(&pack_requests);

        return 1;
    }


}

int core_reconnect()
{
    insim.isclose();

    out << "wait 1 minute and reconnect \n";
    Sleep (60000);

    struct IS_VER pack_ver;
    memset(&pack_ver,0,sizeof(struct IS_VER));
    if (insim.init (ginfo.IP, ginfo.TCPPORT, IS_PRODUCT_NAME,ginfo.ADMIN, &pack_ver, '!', ISF_MCI + ISF_MSO_COLS,500, ginfo.UDPPORT) < 0)
    {
        out << "\n * Error during initialization * " << endl;
        return -1;
    }
    else
    {

        strcpy(ginfo.Product,pack_ver.Product);

        struct IS_TINY pack_requests;
        memset(&pack_requests, 0, sizeof(struct IS_TINY));
        pack_requests.Size = sizeof(struct IS_TINY);
        pack_requests.Type = ISP_TINY;
        pack_requests.ReqI = 1;

        pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
        insim.send_packet(&pack_requests);

        pack_requests.SubT = TINY_NCN;      // Request all connections to store their user data
        insim.send_packet(&pack_requests);
        //out << "Connections request packet sent!" << endl;

        pack_requests.SubT = TINY_NPL;      // Request all players in-grid to know their PLID
        insim.send_packet(&pack_requests);
        //out << "Player info request packet sent!" << endl;

        pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
        insim.send_packet(&pack_requests);

        return 1;
    }
}

void read_track(struct player *splayer)
{
    char file[255];
    strcpy(file,RootDir);
    //strcat(file,"misc\\mysql.cfg");
    strcat(file,"data\\RCCore\\tracks\\");
    strcat(file,ginfo.Track);
    //strcat(file,".");
    //strcat(file,splayer->Lang);
    strcat(file,".txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);


    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);


        if (strstr(str,"//")) {}
        if (strlen(str) > 0)
        {

            if (strncmp(str,"/street",7)==0)
            {
                int i = 0;
                readf.getline(str,128);
                while (strncmp(str,"#street",7)!=0)
                {
                    if (!(strstr(str,"#street")))
                    {
                        char * street;
                        char * node1;
                        char * node2;
                        char * speed;
                        street = strtok (str,";");
                        node1 = strtok (NULL,";");
                        node2 = strtok (NULL,";");
                        speed = strtok (NULL,";");

                        memset(&splayer->street[i],0,sizeof(streets));
                        strcpy(splayer->street[i].Street,"^C^7");
                        strcat(splayer->street[i].Street,street);
                        splayer->street[i].NodeBeg = atoi(node1);
                        splayer->street[i].NodeEnd = atoi(node2);
                        splayer->street[i].SpeedLimit = atoi(speed);
                        readf.getline(str,128);
                        i++;
                    }
                } // while #street
                splayer->str_count = i;
            } // if /street

            if (strncmp(str,"/pitzone",8)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);
                memset(&splayer->TrackInf,0,sizeof(track_info));
                splayer->TrackInf.PitCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,";");
                    Y = strtok (NULL,";");
                    splayer->TrackInf.XPit[i] = atoi(X);
                    splayer->TrackInf.YPit[i] = atoi(Y);
                }

            }

            if (strncmp(str,"/shop",5)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);
                splayer->TrackInf.ShopCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,";");
                    Y = strtok (NULL,";");
                    splayer->TrackInf.XShop[i] = atoi(X);
                    splayer->TrackInf.YShop[i] = atoi(Y);
                }
            }

        } // if strlen > 0
    } //while readf.good()

    readf.close();
}



void read_car()
{

    char file[255];
    strcpy(file,RootDir);
    //strcat(file,"misc\\mysql.cfg");
    strcat(file,"misc\\cars.txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
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

            if (strstr(str,"//")) {}
            else
            {
                char * number;
                char * car;
                char * cash;
                number = strtok (str,";");
                car = strtok (NULL,";");
                cash = strtok (NULL,";");

                i = atoi(number);
                memset(&ginfo.car[i],0,sizeof(struct cars));
                ginfo.car[i].id = i;
                strcpy(ginfo.car[i].car, car);
                ginfo.car[i].cash =atoi(cash);
                ginfo.car[i].sell= ginfo.car[i].cash*8/10;
            }


        } // if strlen > 0
    } //while readf.good()

    readf.close();
}

void read_fines()
{

    char file[255];
    strcpy(file,RootDir);
    //strcat(file,"misc\\mysql.cfg");
    strcat(file,"misc\\fines.txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
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

            if (strstr(str,"//")) {}
            else
            {
                char * id;
                char * name;
                char * cash;
                id = strtok (str,";");
                name = strtok (NULL,";");
                cash = strtok (NULL,";");

                i = atoi(id);
                memset(&ginfo.fines[i],0,sizeof(struct cars));
                ginfo.fines[i].id = i;
                strcpy(ginfo.fines[i].name, name);
                ginfo.fines[i].cash = atoi(cash);

            }


        } // if strlen > 0
    } //while readf.good()

    readf.close();
}

int read_cop(struct player *splayer)
{
    char file[255];
    strcpy(file,RootDir);
    strcat(file,"misc\\cops.txt");
    //strcat(file,"tracks\\.txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return -1;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);

    int cop = 0;
    while (readf.good())
    {
        char str[32];
        readf.getline(str,32);
        if (strlen(str) > 0)
        {
            if (strncmp(splayer->UName,str,strlen(splayer->UName))==0)
            {
                cop ++;
            }
        }
    }


    readf.close();

    return cop;
}

void read_cfg()
{
    out << "Read Server Config" << endl;
    //out <<  << endl;
    char file[255];
    strcpy(file,RootDir);
    strcat(file,"misc\\");
    strcat(file,ServiceName);
    strcat(file,".cfg");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);

    char str[128];
    while(readf.good())
    {
        readf.getline(str,128);
        if (strlen(str) > 0)
        {
            // GET IP
            if (strncmp("Ip=",str,3)==0)
            {
                strncpy(ginfo.IP,str+3,strlen(str+3));
            }
            // GET TCPPORT
            if (strncmp("TcpPort=",str,8)==0)
            {
                ginfo.TCPPORT = atoi(str+8);
            }
            // GET UDPPORT
            if (strncmp("UdpPort=",str,8)==0)
            {
                ginfo.UDPPORT = atoi(str+8);
            }
            //GET ADMIN PASSWORD
            if (strncmp("Admin=",str,6)==0)
            {
                strncpy(ginfo.ADMIN,str+6,strlen(str+6));
            }
        }
    }
}
/**
void read_lang(struct player *splayer)
{
    char file[255];
    strcpy(file,RootDir);
    //strcat(file,"misc\\mysql.cfg");
    strcat(file,"lang\\");
    strcat(file,splayer->Lang);
    strcat(file,".txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);
    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);
        if (strlen(str) > 6)
        {
            char * id;
            char * mesage;

            id = strtok (str,"\"");
            mesage = strtok (NULL,"\"");

            // out << "id = " << id << "message = " << mesage << endl;

            //memset(&message[atoi(id)],0,sizeof(message));
            strcpy(msg.message[splayer->lang_id][atoi(id)], mesage);

        }
    }
}
**/

void chek_users1()
{
    /* char file[64] = "";
     strcpy(file,"users.log");
     ifstream readf (file,ios::in);
     int i=0;
     for ( int f=0;f<=ginfo.NUsers;f++)
     strcpy(ginfo.players2[f].UName,"");

     ginfo.NUsers =0;
     while (!readf.eof())
     {
     readf >> ginfo.players2[i].UName;
     ginfo.NUsers = i+1;
     i++;
     }

     for (int i = 0; i  <= MAX_PLAYERS; i++)

    {
        for (int j =0; j<= ginfo.NUsers; j++)
             {
                 if (ginfo.players[i].UCID !=0 and strcmp(ginfo.players2[j].UName , ginfo.players[i].UName) == 0)

                             {
                                out <<ginfo.players[i].UName <<endl;
                                 ginfo.players[i].Mode =1;
                                 break;
                             }
                             else{
                             ginfo.players[i].Mode =0;
                             }
             }
    }

    for (int g=0; g<MAX_PLAYERS;g++)
    {
        if(ginfo.players[g].UCID !=0 and  ginfo.players[g].Mode == 0)
        {
        spec(&ginfo.players[g]);
        struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = ginfo.players[g].UCID;
    strcpy(pack_mtc.Msg,"^C^7Вы не включили программу клиент");
    insim.send_packet(&pack_mtc);

        }
    }
    chek_users =0;*/
}

/*********************************************/

void *thread_mci (void *params)
{
    out << "\tthread \"Multi Car Info\" started" << endl;
    while (true)
    {
        if (insim.udp_next_packet() < 0)
        {
            //out << "\n * Error getting next UDP packet * " << endl;
            continue;
        }

        else
        {
            //out << "UDP packet MCI " << endl;
            case_mci ();
            case_mci_svetofor();
            case_mci_cop();

            pizza.pizza_mci();
            nrg.energy_mci();
            //bank.bank_mci();
        }

    }
    return 0;
};

void *thread_btn (void *params)
{
    out << "\tthread \"Buttons\" started" << endl;
    while (ok > 0)
    {
        for (int i=0; i<MAX_PLAYERS; i++)
        {
            if (ginfo.players[i].UCID != 0)
            {
                //nrg.btn_energy(i);
                bank.btn_cash(i);
                btn_panel(&ginfo.players[i]);
            }
        }
        Sleep(2000);
    }
    return 0;
};

void *thread_save (void *params)
{
    out << "\tthread \"Backup saving\" started" << endl;
    SYSTEMTIME sm; //time_t seconds;
    while (ok > 0)
    {
        GetLocalTime(&sm); //seconds = time (NULL);
        if ((sm.wMinute*60+sm.wSecond) % 1800 == 0) //every 30 minute
        {

            for (int j=0; j<MAX_PLAYERS; j++)
            {

                if (ginfo.players[j].UCID !=0 )
                {

                    save_car(&ginfo.players[j]);
                    save_user_cars(&ginfo.players[j]);
                    save_user_fines(&ginfo.players[j]);

                    nrg.energy_save(j);
                    bank.bank_save(j);

                    send_mtc(ginfo.players[j].UCID,msg.message[ginfo.players[j].lang_id][3000]);
                }
            }

        }
        Sleep(500);
        siren = "^4||||||||||^1||||||||||";
        Sleep(500);
        siren = "^1||||||||||^4||||||||||";
    }
    return 0;
};

void *thread_work (void *params)
{
    out << "\tthread \"Work\" started" << endl;
    Sleep(10000);
    //pizza.init(&pizza,&insim,&msg,&bank,&nrg);
    //pizza.readconfig(ginfo.Track);

    //int RCPizza_time = time(&stime);
    //out << RCPizza_time << endl;

    while (ok > 0)
    {
        // вывод кнопки с часиками и скрытие ее если таймер пришел в ноль
        for (int i=0; i<MAX_PLAYERS; i++)
        {
            if (ginfo.players[i].UCID != 0)
            {
                if (ginfo.players[i].Pogonya == 1)
                {
                    btn_work(&ginfo.players[i]);
                    int nowtime = time(&stime);
                    if (ginfo.players[i].WorkTime <= nowtime)
                    {
                        send_bfn(ginfo.players[i].UCID,210);
                        char Text[64];

                        strcpy(Text,"/msg ^2|");
                        strcat(Text,ginfo.players[i].PName);
                        strcat(Text,msg.message[ginfo.players[i].lang_id][1706]);
                        send_mst(Text);
                        ginfo.players[i].Pogonya = 0;

                    }
                }

            }
        } // конец цикла вывода кнопки с часиками и скрытие ее если таймер пришел в ноль
        /*
                 if ( ginfo_time <= time(&stime))
                 {
                srand(time(&stime));
                int r = rand()%3 + 1;
                RCPizza_time += 600/(ginfo.NumP+1);
                //out << r << endl;
                //out << ginfo_time - time(&stime) << endl;

                for (int i = 0; i<32; i++)
                {


                if ( (ginfo.players[i].UCID !=0) and (ginfo.players[i].WorkType == WK_RCPizza) and (ginfo.players[i].WorkAccept == 0))
                {
                out << ginfo.players[i].UName << " accepted\n";
                send_mtc(ginfo.players[i].UCID,"^C^3| ^7Поступил заказ на пиццу");
                send_mtc(ginfo.players[i].UCID,"^C^3| ^7Подъедте к пиццерии и отвезите заказ");
                ginfo.players[i].WorkAccept =1;
                ginfo.players[i].WorkPlayerAccept =0;
                ginfo.players[i].WorkZone =0;
                int worktime = time(&stime);
                ginfo.players[i].WorkTime = worktime+60*6;
                 break; // чтобы оповещал только одного игрока
                }

                }
                }
                */


        Sleep(1000);
    }
    return 0;
};

void *thread_svet1(void* params)
{
    out << "\tthread \"Svetofor 1\" started" << endl;
    for (;;)
    {
        int svtime = time(&stime)%40;
        //out << "svtime = " << svtime << endl;

        if (svtime < 10)
        {
            signal1="^C^0•";
            signal2="^C^0•";
            signal3="^C^2•";
        }
        else if (svtime == 10)
        {
            signal1="^C^0•";
            signal2="^C^0•";
            signal3="^C^0•";
        }
        else if (svtime == 11)
        {
            signal1="^C^0•";
            signal2="^C^0•";
            signal3="^C^2•";
        }
        else if (svtime == 12)
        {
            signal1="^C^0•";
            signal2="^C^0•";
            signal3="^C^0•";
        }
        else if (svtime == 13)
        {
            signal1="^C^0•";
            signal2="^C^0•";
            signal3="^C^2•";
        }
        else if (svtime == 14)
        {
            signal1="^C^0•";
            signal2="^C^0•";
            signal3="^C^0•";
        }
        else if (svtime == 15)
        {
            signal1="^C^0•";
            signal2="^C^0•";
            signal3="^C^2•";
        }
        else if (svtime == 16)
        {
            signal1="^C^0•";
            signal2="^C^0•";
            signal3="^C^0•";
        }
        else if ((svtime > 16) and (svtime < 20))
        {
            signal1="^C^0•";
            signal2="^C^3•";
            signal3="^C^0•";
        }
        else if ((svtime >= 20) and (svtime < 37))
        {
            signal1="^C^1•";
            signal2="^C^0•";
            signal3="^C^0•";
        }
        else if ((svtime >= 37))
        {
            signal1="^C^1•";
            signal2="^C^3•";
            signal3="^C^0•";
        }


        Sleep(1000);
    }
    return 0;
}

void *thread_svet2( void* params)
{
    out << "\tthread \"Svetofor 2\" started" << endl;
    for (;;)
    {
        int svtime = (time(&stime)+20)%40;
        //out << "svtime = " << svtime << endl;

        if (svtime < 10)
        {
            signal11="^C^0•";
            signal12="^C^0•";
            signal13="^C^2•";
        }
        else if (svtime == 10)
        {
            signal11="^C^0•";
            signal12="^C^0•";
            signal13="^C^0•";
        }
        else if (svtime == 11)
        {
            signal11="^C^0•";
            signal12="^C^0•";
            signal13="^C^2•";
        }
        else if (svtime == 12)
        {
            signal11="^C^0•";
            signal12="^C^0•";
            signal13="^C^0•";
        }
        else if (svtime == 13)
        {
            signal11="^C^0•";
            signal12="^C^0•";
            signal13="^C^2•";
        }
        else if (svtime == 14)
        {
            signal11="^C^0•";
            signal12="^C^0•";
            signal13="^C^0•";
        }
        else if (svtime == 15)
        {
            signal11="^C^0•";
            signal12="^C^0•";
            signal13="^C^2•";
        }
        else if (svtime == 16)
        {
            signal11="^C^0•";
            signal12="^C^0•";
            signal13="^C^0•";
        }
        else if ((svtime > 16) and (svtime < 20))
        {
            signal11="^C^0•";
            signal12="^C^3•";
            signal13="^C^0•";
        }
        else if ((svtime >= 20) and (svtime < 37))
        {
            signal11="^C^1•";
            signal12="^C^0•";
            signal13="^C^0•";
        }
        else if ((svtime >= 37))
        {
            signal11="^C^1•";
            signal12="^C^3•";
            signal13="^C^0•";
        }


        Sleep(1000);
    }
    return 0;
}


DWORD WINAPI ThreadMain(void *CmdLine)
{
    // TODO (#1#): Uncoment in Release
    //Sleep(2*60*1000);



    if (strlen(ServiceName) == 0)
    {
        out << "Не задан файл конфигурации\n"   ;
        service_status.dwCurrentState = SERVICE_STOPPED;
        // изменить состояние сервиса
        SetServiceStatus(hServiceStatus, &service_status);
        return 0;

    }

    // read_mysql();

    int error_ch;

    //cruise.opendb("cruise.db3");


    memset(&ginfo, 0, sizeof(struct global_info));

    read_cfg();

    if (core_connect()<0)
    {
        for (;;)
        {
            if (core_reconnect()>0)
                break;
        }
    }
    pthread_t mci_tid; // Thread ID
    pthread_t btn_tid; // Thread ID
    pthread_t work_tid; // Thread ID
    pthread_t save_tid; // Thread ID
    pthread_t svet1_tid; // Thread ID
    pthread_t svet2_tid; // Thread ID

    out << "Cruise started" << endl;
    out << "Start threads :" << endl;
    if (pthread_create(&btn_tid,NULL,thread_btn,NULL) < 0)
    {
        printf("Can't start `thread_btn` Thread\n");
        return 0;
    }
    Sleep(1000);
    if (pthread_create(&work_tid,NULL,thread_work,NULL) < 0)
    {
        printf("Can't start `thread_work` Thread\n");
        return 0;
    }
    Sleep(1000);
    if (pthread_create(&save_tid,NULL,thread_save,NULL) < 0)
    {
        printf("Can't start `thread_save` Thread\n");
        return 0;
    }
    Sleep(1000);
    if (pthread_create(&svet1_tid,NULL,thread_svet1,NULL) < 0)
    {
        printf("Can't start `thread_svet1` Thread\n");
        return 0;
    }
    Sleep(1000);
    if (pthread_create(&svet2_tid,NULL,thread_svet2,NULL) < 0)
    {
        printf("Can't start `thread_svet2` Thread\n");
        return 0;
    }
    Sleep(1000);

    pizza.init(RootDir,&pizza,&insim,&msg,&bank,&nrg);
    msg.init(RootDir);
    nrg.init(RootDir,&nrg,&insim,&msg,&bank);
    bank.init(RootDir,&insim,&msg);

    if (pthread_create(&mci_tid,NULL,thread_mci,NULL) < 0)
    {
        printf("Can't start `thread_mci` Thread\n");
        return 0;
    }
    Sleep(1000);
    out << "All threads started" << endl;





    while (ok > 0)
    {

        if (error_ch = insim.next_packet() < 0)
        {
            //out << "\n * Error getting next packet * " << endl;
            //out<< error_ch;
            for (;;)
            {
                if (core_reconnect()>0)
                    break;
            }

        }


        switch (insim.peek_packet())
        {
        case ISP_MSO:
            //out << " IS_MSO \n";
            case_mso ();
            //case_mso_work();
            case_mso_cop();
            pizza.pizza_mso();
            nrg.energy_mso();
            // bank.bank_mso();
            break;


        case ISP_NPL:
            //out << " IS_NPL \n";
            //chek_users1(&ginfo);
            case_npl ();
            pizza.pizza_npl();
            nrg.energy_npl();
            bank.bank_npl();
            break;

        case ISP_NCN:
            //out << " IS_NCN \n";
            case_ncn ();
            pizza.pizza_ncn();
            nrg.energy_ncn();
            bank.bank_ncn();
            break;

        case ISP_CNL:
            //out << " IS_CNL \n";
            case_cnl ();
            pizza.pizza_cnl();
            nrg.energy_cnl();
            bank.bank_cnl();
            break;

        case ISP_PLL:
            //out << " IS_PLL \n";
            case_pll ();
            pizza.pizza_pll();
            nrg.energy_pll();
            bank.bank_pll();
            break;

        case ISP_PLP:
            //out << " IS_PLP \n";
            case_plp ();
            pizza.pizza_plp();
            nrg.energy_plp();
            bank.bank_plp();
            break;


        case ISP_BTC:
            //out << " IS_BTC \n";
            case_btc ();
            break;
        case ISP_BTT:
            //out << " IS_BTT \n";
            case_btt ();
            break;
        case ISP_BFN:
            //out << " ISP_BFN \n";
            case_bfn();
            break;


        case ISP_SPX:
            //out << " IS_SPX \n";
            //case_spx (&ginfo);
            break;

        case ISP_CPR:
            //out << " IS_CPR \n";
            case_cpr ();
            pizza.pizza_crp();
            nrg.energy_crp();
            bank.bank_crp();
            break;

        case ISP_RST:
            out << " IS_RST \n";
            case_rst ();
            break;

        case ISP_REO:
            //out << " IS_REO \n";
            // case_reo (&ginfo);
            break;

        case ISP_NLP:
            //out << " IS_NLP \n";
            // case_reo (&ginfo);
            break;

        case ISP_FLG:
            //out << " IS_FLG \n";
            case_flg ();
            break;

        case ISP_PEN:
            //out << " IS_PEN \n";
            case_pen ();
            break;
        case  ISP_PLA:
            cout << " IS_PLA \n";
            case_pla();
            break;

        case ISP_VTN:
            //out << " IS_VTN \n";
            case_vtn();
            break;
        }


    }


    if (insim.isclose() < 0)
    {
        //out << "\n * Error closing connection * " << endl;
        return 0;
    }



    service_status.dwCurrentState = SERVICE_STOPPED;
    // изменить состояние сервиса
    SetServiceStatus(hServiceStatus, &service_status);
    return 0;
}


int  nCount;     // счетчик


int core_install_service(char* param[])
{
    hServiceControlManager = OpenSCManager(
                                 NULL,      // локальная машина
                                 NULL,      // активная база данных сервисов
                                 SC_MANAGER_CREATE_SERVICE  // возможно создание сервиса
                             );
    if (hServiceControlManager == NULL)
    {
        printf("Open service control manager failed.\n");
        //<< "The last error code: " << GetLastError() << endl
        //<< "Press any key to exit." << endl;
        //cin.get();

        return 0;
    }
    printf("Service control manager is opened.\n");
    char dir[255];

    strcpy(dir,param[0]);
    strcat(dir," ");
    strcat(dir,param[1]);
    //out << dir << endl;
    //out << "Press any key to continium." << endl;
    //cin.get();

    // устанавливаем новый сервис
    hService = CreateService(
                   hServiceControlManager,      // дескриптор менеджера сервисов
                   ServiceName,                 // внутреннее имя сервиса, используемое SCM
                   ServiceName,                 // внешнее имя сервиса в панели управления
                   SERVICE_ALL_ACCESS,          // полный контроль над сервисом
                   SERVICE_WIN32_OWN_PROCESS,   // сервис является процессом
                   SERVICE_AUTO_START,        // запускается сервис по требованию
                   SERVICE_ERROR_NORMAL,        // обработка ошибок нормальная
                   dir,                         // путь к сервису
                   NULL,                        // сервис не принадлежит к группе
                   NULL,                        // тэг группы не изменяется
                   NULL,                        // сервис не зависит от других сервисов
                   NULL,                        // имя совпадает с текущим именем учетной записи
                   NULL                         // пароля нет
               );
    if (hService == NULL)
    {
        printf("Create service failed.");

        // закрываем дескриптор менеджера сервисов
        CloseServiceHandle(hServiceControlManager);

        return 0;
    }

    printf("Service is installed.");
    // закрываем дескрипторы
    CloseServiceHandle(hService);
    CloseServiceHandle(hServiceControlManager);
    return 2;
}

int core_uninstall_service(char* param[])
{
    hServiceControlManager = OpenSCManager(
                                 NULL,      // локальная машина
                                 NULL,      // активная база данных сервисов
                                 SC_MANAGER_CONNECT   // соединение с менеджером сервисов
                             );
    if (hServiceControlManager == NULL)
    {
        printf("Open service control manager failed.");

        return 0;
    }

    printf("Service control manager is opened.");

    // открываем сервис
    hService = OpenService(
                   hServiceControlManager,  // дескриптор менеджера сервисов
                   ServiceName,            // имя сервиса
                   SERVICE_ALL_ACCESS | DELETE  // любой доступ к сервису
                   // и удаление из базы данных
               );
    if (hService == NULL)
    {
        printf("Open service failed.");
        // закрываем дескриптор менеджера сервисов
        CloseServiceHandle(hServiceControlManager);

        return 0;
    }

    printf("Service is opened.");


    // получаем состояние сервиса
    if (!QueryServiceStatus(hService, &service_status))
    {
        printf("Query service status failed.");

        // закрываем дескрипторы
        CloseServiceHandle(hServiceControlManager);
        CloseServiceHandle(hService);

        return 0;
    }

    // если сервис работает, то останавливаем его
    if (service_status.dwCurrentState != SERVICE_STOPPED)
    {
        printf("Service is working. It will be stoped");
        if (!ControlService(hService, SERVICE_CONTROL_STOP, &service_status))
        {
            printf("Control service failed.");

            // закрываем дескрипторы
            CloseServiceHandle(hServiceControlManager);
            CloseServiceHandle(hService);

            return 0;
        }

        // ждем, пока сервис остановится
        Sleep(5000);
    }

    // удаляем сервис
    if (!DeleteService(hService))
    {
        printf("Delete service failed.");

        // закрываем дескрипторы
        CloseServiceHandle(hServiceControlManager);
        CloseServiceHandle(hService);

        return 0;
    }

    printf("The service is deleted.");

    // закрываем дескрипторы
    CloseServiceHandle(hServiceControlManager);
    CloseServiceHandle(hService);

    return 2;
}
// главная функция приложения
int main(int argc, char* argv[])
{


    int need = 92;
    int d;

    for (d = strlen(argv[0]); d > 0; d--)
    {
        if ( int(argv[0][d]) == need )
            break;
    }
    strncpy(RootDir,argv[0],d+1);



    strcpy(ServiceName,argv[1]);


    if (strcmp(argv[argc-1],"install") == 0 )
    {
        core_install_service(argv);
        return 0;
    }
    else if (strcmp(argv[argc-1],"uninstall") == 0 )
    {
        core_uninstall_service(argv);
        return 0;
    }
    else if (strcmp(argv[argc-1],"console") == 0 )
    {
        char day[3];
        char month[3];
        char year[3];

        SYSTEMTIME sm;
        GetLocalTime(&sm);

        itoa(sm.wDay,day,10);
        itoa(sm.wMonth,month,10);
        itoa(sm.wYear,year,10);


        char log[MAX_PATH];
        strcpy(log,RootDir);
        strcat(log,"logs\\");
        strcat(log,ServiceName);
        strcat(log,"(");
        strcat(log,day);
        strcat(log,".");
        strcat(log,month);
        strcat(log,".");
        strcat(log,year);
        strcat(log,").log");

        out.open(log);

        out << RootDir << endl;
        out << "Main Thead started. Wait 2 minuts while all services are started.\n"   ;
        CreateThread(NULL,0,ThreadMain,0,0,NULL);

        // рабочий цикл сервиса
        while (ok)
        {
            Sleep(1000);
        }
        out.close();

        Sleep(10000);

        return 0;
    }

    //  инициализируем структуру сервисов
    SERVICE_TABLE_ENTRY  service_table[] =
    {
        {ServiceName, ServiceMain},   // имя сервиса и функция сервиса
        { NULL, NULL }                 // больше сервисов нет
    };

    // запускаем диспетчер сервиса
    if (!StartServiceCtrlDispatcher(service_table))
    {

        return 0;
    }

    return 0;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    char day[3];
    char month[3];
    char year[3];

    SYSTEMTIME sm;
    GetLocalTime(&sm);

    itoa(sm.wDay,day,10);
    itoa(sm.wMonth,month,10);
    itoa(sm.wYear,year,10);


    char log[MAX_PATH];
    strcpy(log,RootDir);
    strcat(log,"logs\\");
    strcat(log,ServiceName);
    strcat(log,"(");
    strcat(log,day);
    strcat(log,".");
    strcat(log,month);
    strcat(log,".");
    strcat(log,year);
    strcat(log,").log");

    out.open(log);

    //out << "регистрируем обработчик управляющих команд для сервиса\n";
    hServiceStatus = RegisterServiceCtrlHandler(
                         ServiceName,        // имя сервиса
                         ServiceCtrlHandler   // обработчик управляющих команд
                     );
    if (!hServiceStatus)
    {

        out << "Register service control handler failed.";


        return;
    }

    // инициализируем структуру состояния сервиса
    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState = SERVICE_START_PENDING;
    service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP |
                                        SERVICE_ACCEPT_SHUTDOWN;
    service_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
    service_status.dwServiceSpecificExitCode = 0;
    service_status.dwCheckPoint = 0;
    service_status.dwWaitHint = 5000;

    // устанавливаем состояние сервиса
    if (!SetServiceStatus(hServiceStatus, &service_status))
    {

        out << "Set service status 'SERVICE_START_PENDING' failed.";


        return;
    }

    // определяем сервис как работающий
    service_status.dwCurrentState = SERVICE_RUNNING;
    // нет ошибок
    service_status.dwWin32ExitCode = NO_ERROR;
    // устанавливаем новое состояние сервиса
    if (!SetServiceStatus(hServiceStatus, &service_status))
    {

        out << "Set service status 'START_PENDING' failed.";

        return;
    }

    // открываем файл протокола работы сервиса

    out << "The service is started." << endl << flush;
    out << "Main Thead started. Wait 2 minuts while all services are started.\n"   ;
    CreateThread(NULL,0,ThreadMain,0,0,NULL);

    // рабочий цикл сервиса
    while (service_status.dwCurrentState == SERVICE_RUNNING)
    {
        ++nCount;
        Sleep(3000);
    }

    out << "" << endl;
    out << "\t-----------------------------" << endl;
    out << "" << endl;
    out.close();
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControl)
{
    switch (dwControl)
    {
    case SERVICE_CONTROL_STOP:     // остановить сервис
        // save all users
        for (int j=0; j<MAX_PLAYERS; j++)
        {

            if (ginfo.players[j].UCID !=0 )
            {
                save_car(&ginfo.players[j]);
                save_user_cars(&ginfo.players[j]);
                save_user_fines(&ginfo.players[j]);
                nrg.energy_save(j);
                bank.bank_save(j);
                Sleep(500);
            }
        }
        ok=0;
        // записываем конечное значение счетчика
        //out << "Count = " << nCount << endl;
        out << "The service is finished." << endl << flush;
        // закрываем файл


        // устанавливаем состояние остановки
        service_status.dwCurrentState = SERVICE_STOPPED;
        // изменить состояние сервиса
        SetServiceStatus(hServiceStatus, &service_status);
        break;

    case SERVICE_CONTROL_SHUTDOWN:     // завершить сервис
        service_status.dwCurrentState = SERVICE_STOPPED;
        // изменить состояние сервиса
        SetServiceStatus(hServiceStatus, &service_status);
        break;

    default:
        // увеличиваем значение контрольной точки
        ++service_status.dwCheckPoint;
        // оставляем состояние сервиса без изменения
        SetServiceStatus(hServiceStatus, &service_status);
        break;
    }

    return;
}

