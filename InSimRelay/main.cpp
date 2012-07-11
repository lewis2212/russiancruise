using namespace std;

//#include <iostream>
//#include <fstream>
#include "CInsim.h"
#include "IsRelay.h"
#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <time.h>
#include <mysql/mysql.h>


#define IS_PRODUCT_NAME "LFSPRelay"

#define MY_PORT   47474


SYSTEMTIME sm; //time_t seconds;
int clients;

MYSQL db;
MYSQL_RES *res;
MYSQL_ROW row;

DWORD WINAPI ListenTread(void *csocket);
DWORD WINAPI InsimTread(void* ID);
DWORD WINAPI InsimGo(void *csocket);

struct SERVER
{
    SOCKET  sock;
    int     ID;
    char	HName[32];	// Name of the host
    char	Track[6];	// Short track name
    char    Admin[25];
    char    Spec[25];
    byte	Flags;		// Info flags about the host - see NOTE 1) below
    byte	NumConns;	// Number of people on the host
    byte    NumP;
    byte    ok;
};

struct CLIENT
{
    int UCID;
    int ID;
    SOCKET sock;
    byte Admin;
};

struct GINFO
{
    SERVER server[100];
    int NServ; //num of servers
    CLIENT client[100];

};

struct SERVERS
{
    char id[5];
    char IP[16];
    word Port;
    char AdminPass[25];
    char SpecPass[25];
};

GINFO ginfo;

unsigned char packet[PACKET_MAX_SIZE];           // A buffer where the current packet is stored


/** client functions **/


void send_mst(SOCKET sock,char *msg)
{
    IS_MST pack_mst;
    memset(&pack_mst,0,sizeof(IS_MST));
    pack_mst.Size = sizeof(IS_MST);
    pack_mst.Type = ISP_MST;		// ISP_MST

    strcpy((char *)pack_mst.Msg,msg);
    CInsim insim;
    insim.sock = sock;
    insim.send_packet(&pack_mst);
}


int irp_sel(IsRelay *client, CInsim *insim)
{
    struct IR_SEL *pack_sel = (struct IR_SEL*)client->get_packet();

    //cout << pack_sel->HName<< endl;
    //cout << pack_sel->Admin<< endl;
    //cout << pack_sel->Spec<< endl;

    for (int i=0; i<=100; i++)
    {
        if (ginfo.server[i].ID != 0)
        {
            if (strcmp(ginfo.server[i].HName,pack_sel->HName) == 0)
            {
                // <!--check specpass-->
                if (ginfo.server[i].Flags&HOS_SPECPASS)
                {
                    // if Spectator pass required, but none given
                    if (strcmp(pack_sel->Spec,"") == 0)
                    {
                        struct IR_ERR error;
                        memset(&error,0,sizeof(struct IR_ERR));
                        error.Size=sizeof(struct IR_ERR);		// 4
                        error.Type= IRP_ERR;		// IRP_ERR
                        error.ReqI = client->get_reqi();		// As given in RL_SEL, otherwise 0
                        error.ErrNo = IR_ERR_NOSPEC;		// Error number - see NOTE 2) below
                        client->send_packet(&error);
                        return 0;
                    }
                    // if Wrong spec pass given by client
                    else if (strcmp(ginfo.server[i].Spec,pack_sel->Spec) != 0)
                    {
                        struct IR_ERR error;
                        memset(&error,0,sizeof(struct IR_ERR));
                        error.Size=sizeof(struct IR_ERR);		// 4
                        error.Type= IRP_ERR;		// IRP_ERR
                        error.ReqI = client->get_reqi();		// As given in RL_SEL, otherwise 0
                        error.ErrNo = IR_ERR_SPEC;		// Error number - see NOTE 2) below
                        client->send_packet(&error);
                        return 0;
                    }
                }

                if (strlen(pack_sel->Admin) > 0)
                {
                    // if admin > 0 and it's true pass
                    if (strcmp(pack_sel->Admin,ginfo.server[i].Admin) != 0)
                    {
                        struct IR_ERR error;
                        memset(&error,0,sizeof(struct IR_ERR));
                        error.Size=sizeof(struct IR_ERR);		// 4
                        error.Type= IRP_ERR;		// IRP_ERR
                        error.ReqI = client->get_reqi();		// As given in RL_SEL, otherwise 0
                        error.ErrNo = IR_ERR_ADMIN;		// Error number - see NOTE 2) below
                        client->send_packet(&error);
                        return 0;
                    }
                    else if (strcmp(pack_sel->Admin,ginfo.server[i].Admin) == 0)
                    {
                        client->Admin = 1;
                    }

                }
                else
                {
                    client->Admin = 0;
                }

                // <--check specpass-->

                insim->sock = ginfo.server[i].sock;
                int j=0;
                for (j=0; j<100; j++)
                {
                    if (ginfo.client[j].UCID == (int)client->sock)
                    {
                        ginfo.client[j].sock = client->sock;
                        ginfo.client[j].ID = ginfo.server[i].ID;
                        break;
                    }
                }
                if (j == 100)
                {
                    for (int j=0; j<100; j++)
                    {
                        if (ginfo.client[j].UCID == 0)
                        {
                            ginfo.client[j].UCID = (int)client->sock;
                            ginfo.client[j].sock = client->sock;
                            ginfo.client[j].ID = ginfo.server[i].ID;
                            break;
                        }
                    }
                }

                IS_TINY pack_tiny;
                memset(&pack_tiny,0,sizeof(IS_TINY));
                pack_tiny.Size = sizeof(IS_TINY);
                pack_tiny.Type = ISP_TINY;
                pack_tiny.ReqI = client->get_reqi();
                pack_tiny.SubT = TINY_VER;
                insim->send_packet(&pack_tiny);

                GetLocalTime(&sm); //seconds = time (NULL);
                printf("%d:%d Client select %s (Id = %d)\n",sm.wHour,sm.wMinute,pack_sel->HName,(int)client->sock);

                break;
            } // HName == HName
        }
    }

    return 0;
}


void irp_hlr(IsRelay *client, CInsim *insim)
{
    struct IR_HLR *pack_nlr = (struct IR_HLR*)client->get_packet();


    IR_HOS pack_hos;
    memset(&pack_hos,0,sizeof(IR_HOS));
    pack_hos.Size = sizeof(IR_HOS);
    pack_hos.Type = IRP_HOS;
    pack_hos.ReqI = pack_nlr->ReqI;


    int g=0;
    //cout << ginfo.NServ << endl;

    for (int i=0; i<ginfo.NServ; i++)
    {
        if (ginfo.server[i].ID != 0)
        {
            strcpy(pack_hos.Info[g%6].HName,ginfo.server[i].HName);
            strcpy(pack_hos.Info[g%6].Track,ginfo.server[i].Track);
            pack_hos.Info[g%6].Flags = ginfo.server[i].Flags;
            if (i == 0)
                pack_hos.Info[g].Flags += HOS_FIRST;

            if (g == ginfo.NServ-1)
                 pack_hos.Info[g].Flags += HOS_LAST;

            pack_hos.Info[g%6].NumConns = ginfo.server[i].NumConns;
            g++;
        }
        if (g>0 and g%6 == 0)
        {
            pack_hos.NumHosts = g;
            printf("NumHosts = %d\n",g);
            client->send_packet(&pack_hos);

            for(int j=0; j<6; j++)
            ZeroMemory(&pack_hos.Info[j],sizeof(HInfo));
            g = 0;
        }

    }

    if (g != 0)
    {
        pack_hos.NumHosts = g;
        printf("NumHosts = %d\n",g);
        client->send_packet(&pack_hos);
    }


}


void irp_arq(IsRelay *client, CInsim *insim)
{
    IS_TINY pack_ok;
    memset(&pack_ok,0,sizeof(IS_TINY));
    pack_ok.Size = sizeof(IS_TINY);
    pack_ok.Type = IRP_ARP;
    pack_ok.ReqI = 1;
    pack_ok.SubT = client->Admin;
    client->send_packet(&pack_ok);
}

void irp_close(IsRelay *client, CInsim *insim)
{
    //cout << "IR_CLOSE" << endl;
    struct IR_CLOSE *pack_cls = (struct IR_CLOSE*)client->get_packet();

    //cout << "IR_CLOSE[type] = " << (int)pack_cls->Type << endl;
    //cout << "IR_CLOSE[ServID] = " << (int)pack_cls->ServID << endl;
    for (int i=0; i<100; i++)
    {
        if (ginfo.server[i].ID == (int)pack_cls->ServID )
        {
            //cout << "server fined - " << ginfo.server[i].ID << endl;
            ginfo.server[i].ok = 0;
            send_mst(ginfo.server[i].sock,"/msg ^1LFSPRelay disconnect");

            break;
        }
    }
}



/** end of client functions **/





int main(int argc, char* argv[])
{
    if (!mysql_init(&db))
    {
        printf("Can't creat MySQL descriptor\n");
        return 0;
    }

    while (true)
    {
        if (!mysql_real_connect(&db,"localhost","root","04051989","isrelay",3310,NULL,0))
        {
            printf("Can't connect to MySQL server\n");
            Sleep(60*1000);
        }
        else
        {
            break;
        }
    }



    memset (&ginfo,0,sizeof(GINFO));
    memset(packet, 0, PACKET_MAX_SIZE);

    mysql_query(&db,"update servers set status='0';");
    mysql_query(&db,"update servers set name='';");


    CreateThread(NULL,0,InsimGo,NULL,0,NULL);



    char buff[1024];
    //int bytes_recv;
    if (WSAStartup(0x0202,(WSADATA *) &buff[0]))
    {
        // Ошибка!
        printf("Error WSAStartup %d\n",WSAGetLastError());
        return -1;
    }

    // Шаг 2 - создание сокета
    SOCKET mysocket;
    // AF_INET     - сокет Интернета
    // SOCK_STREAM  - потоковый сокет (с
    //      установкой соединения)
    // 0      - по умолчанию выбирается TCP протокол
    if ((mysocket=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        // Ошибка!
        printf("Error socket %d\n",WSAGetLastError());
        WSACleanup();
        // Деиницилизация библиотеки Winsock
        return -1;
    }

    // Шаг 3 связывание сокета с локальным адресом
    sockaddr_in local_addr;
    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htons(MY_PORT);
    // не забываем о сетевом порядке!!!
    local_addr.sin_addr.s_addr=0;
    // сервер принимает подключения
    // на все IP-адреса

    // вызываем bind для связывания
    if (bind(mysocket,(sockaddr *)&local_addr,sizeof(local_addr)))
    {
        // Ошибка
        printf("Error bind %d\n",WSAGetLastError());
        closesocket(mysocket);  // закрываем сокет!
        WSACleanup();
        return -1;
    }

    // Шаг 4 ожидание подключений
    // размер очереди – 0x100
    if (listen(mysocket, 0x100))
    {
        // Ошибка
        printf("Error listen %d\n",WSAGetLastError());
        closesocket(mysocket);
        WSACleanup();
        return -1;
    }

    // printf("Wait for connect\n");

    // Шаг 5 извлекаем сообщение из очереди
    SOCKET client_socket;    // сокет для клиента
    sockaddr_in client_addr;    // адрес клиента
    // (заполняется системой)

    // функции accept необходимо передать размер
    // структуры
    int client_addr_size=sizeof(client_addr);

    // цикл извлечения запросов на подключение из
    // очереди
    while ((client_socket=accept(mysocket, (sockaddr*)&client_addr, &client_addr_size)))
    {
        clients++;
        GetLocalTime(&sm); //seconds = time (NULL);
        printf("%d:%d Client connected (Id = %d)\n",sm.wHour,sm.wMinute,(int)client_socket);
        //printf("new client!\n");
        DWORD ID;
        CreateThread(NULL,0,ListenTread,(void*)&client_socket,0,&ID);

    }

    mysql_close(&db);
    closesocket(mysocket);
    WSACleanup();
    system("PAUSE");
    return 0;
}


DWORD WINAPI ListenTread(void *csocket)
{
    SOCKET th_sock;
    th_sock = ((SOCKET*)csocket)[0];


    CInsim insim;
    IsRelay client;



    memcpy(&client.sock,&th_sock,sizeof(SOCKET));

    int ok = 1;

    while ( ok > 0)
    {

        //char size[1];
        //char buff[PACKET_BUFFER_SIZE];

        if (client.next_packet() < 0)
        {
            break;
        }

        /** POLICY **/

        char get[128];

        memcpy(get,client.get_packet(),client.packet_size);

        //printf("Pack: %s\n",get);

        if ( strncmp(get,"<policy",7)== 0 )
        {

            printf("Request Policy\n");

            char request[512];
            memset(&request,0,512);
            strcpy(request,"<?xml version=\"1.0\"?>\n<!DOCTYPE cross-domain-policy SYSTEM test\"/xml/dtds/cross-domain-policy.dtd\">\n");
            strcat(request,"<cross-domain-policy>\n<site-control permitted-cross-domain-policies=\"master-only\" />\n");
            strcat(request,"<allow-access-from domain=\"*\" to-ports=\"47474\" />\n");
            strcat(request,"<allow-access-from domain=\"*\" to-ports=\"47474\" />\n");
            strcat(request,"</cross-domain-policy>\0A\x00");
            //printf("Try send Policy answer:\n%s\n",request);
            client.send_packet2(&request,strlen(request)+2);

        }




        /** **/


        if ((client.peek_packet() == 3) and (client.type_packet() == -1))
        {
            printf("Request Version\n");
            struct IR_VER pack;
            memset(&pack,0,sizeof(struct IR_VER));
            pack.Size = sizeof(struct IR_VER);
            pack.Type = 4;
            pack.ReqI = client.get_reqi();
            pack.ReqI2 = client.type_packet();
            pack.Ver = 213;
            client.send_packet(&pack);
        }



        switch (client.peek_packet())
        {
        case IRP_CLOSE:
            irp_close(&client, &insim);
            ok = 0;
            break;

        case IRP_HLR:
            irp_hlr(&client, &insim);
            break;

        case IRP_SEL:
            irp_sel(&client, &insim);
            break;

        case IRP_ARQ:
            irp_arq(&client, &insim);
            break;

            /**************/
            default:
            insim.send_packet(client.get_packet());
            break;



        }// switch


    } // for (;;)

    for (int i=0; i<=100; i++)
    {
        if (ginfo.client[i].UCID == (int)th_sock)
        {
            memset(&ginfo.client[i],0,sizeof(ginfo.client[i]));
            break;
        }
    }
    closesocket(th_sock);
    clients--;
    GetLocalTime(&sm); //seconds = time (NULL);
    printf("%d:%d Client disconnected (Id = %d)\n",sm.wHour,sm.wMinute,(int)th_sock);
    return 0;
}


DWORD WINAPI InsimTread(void* param)
{

    struct SERVERS *pack_serv = (struct SERVERS*)param;

    char query[64];



    int id = atoi(pack_serv->id);




    CInsim insim;
    int serv_id;
    int error_ch;
    struct IS_VER pack_ver;

    //cout << "ID = " << pack_serv->id << endl;
    //cout << "IP = " << pack_serv->IP << endl;
    //cout << "Port = " << pack_serv->Port << endl;
    //cout << "Admin = " << pack_serv->AdminPass << endl;

    if (insim.init (pack_serv->IP, pack_serv->Port, IS_PRODUCT_NAME,pack_serv->AdminPass, &pack_ver, '!', ISF_MCI+ISF_NLP,500,0) < 0)
    {
        //cout << ip << ":" << port << " : Error init" << endl;
        sprintf(query,"UPDATE servers SET status='100' WHERE id='%d';",id);
        mysql_query(&db,query);
        Sleep(1000);
        return 0;
    }




    printf("Connected to: %s:%d\n",pack_serv->IP,pack_serv->Port);

    for (serv_id=0; serv_id<100; serv_id++)
    {
        if (ginfo.server[serv_id].ID == 0 )
        {
            break;
        }
    }
    // cout << "Type = " << pack_ver.Product  << endl;
    ginfo.server[serv_id].ID = id;

    sprintf(query,"UPDATE servers SET status='1' WHERE id='%d';",id);
    mysql_query(&db,query);
    //cout << pack_ver.Product << endl;

    ginfo.server[serv_id].Flags = HOS_LICENSED;

    if (strcmp(pack_ver.Product,"S1") == 0 )
        ginfo.server[serv_id].Flags += HOS_S1;

    if (strcmp(pack_ver.Product,"S2") == 0 )
        ginfo.server[serv_id].Flags += HOS_S2;

    ginfo.server[serv_id].sock = insim.sock;

    strcpy(ginfo.server[serv_id].Admin,pack_serv->AdminPass);

    if (strlen(pack_serv->SpecPass) > 0)
    {
        ginfo.server[serv_id].Flags += HOS_SPECPASS;
        strcpy(ginfo.server[serv_id].Spec,pack_serv->SpecPass);
    }

    ginfo.NServ ++;


   // cout << "LFS Version: " << pack_ver.Version << endl;
   // cout << "LFS Type: " << pack_ver.Product << endl;



    IS_TINY pack;
    memset(&pack,0,sizeof(IS_TINY));
    pack.Size = sizeof(IS_TINY);
    pack.ReqI = 1989;
    pack.Type = ISP_TINY;
    pack.SubT = TINY_ISM;
    insim.send_packet(&pack);
    pack.SubT = TINY_RST;
    insim.send_packet(&pack);
    pack.SubT = TINY_SST;
    insim.send_packet(&pack);

    send_mst(insim.sock,"/msg ^2LFSPRelay connected");

    ginfo.server[serv_id].ok = 1;
    while (ginfo.server[serv_id].ok > 0)
    {

        if (error_ch = insim.next_packet() < 0)
        {
            printf("%s:%d - Error get next paket",pack_serv->IP,pack_serv->Port);
            break;
        }

        if (insim.peek_packet() == ISP_ISM)
        {
            struct IS_ISM *pack_ism = (struct IS_ISM*)insim.get_packet();
          //  cout << "Hostname: " << pack_ism->HName << endl;

            char HostName[32];
            strcpy(HostName,pack_ism->HName);
            char * pch;
            while (pch = strstr(HostName,"^"))
            {
                int point = strlen(pch);
                //cout << pch << endl;
                strcpy(HostName+strlen(HostName)-point,pch+2);
            }
          //  cout << HostName << endl;
            strcpy(ginfo.server[serv_id].HName, HostName);
            sprintf(query,"UPDATE servers SET name='%s' WHERE id='%d';",pack_ism->HName,id);
            mysql_query(&db,query);
        }




        if (insim.peek_packet() == ISP_RST)
        {
            struct IS_RST *pack_rst = (struct IS_RST*)insim.get_packet();
            strcpy(ginfo.server[serv_id].Track, pack_rst->Track);
        }


        if (insim.peek_packet() == ISP_STA)
        {
            struct IS_STA *pack_sta = (struct IS_STA*)insim.get_packet();
            ginfo.server[serv_id].NumP = pack_sta->NumP;
            ginfo.server[serv_id].NumConns= pack_sta->NumConns;
        }



        for (int j=0; j<=100; j++)
        {
            if ((ginfo.client[j].UCID != 0) and (ginfo.server[serv_id].ID == ginfo.client[j].ID))
                send(ginfo.client[j].sock, (const char *)insim.get_packet(), *((unsigned char*)insim.get_packet()), 0);
        }





    }
    ginfo.server[serv_id].NumConns = 0;
    ginfo.server[serv_id].ID = 0;
    ginfo.NServ --;


    sprintf(query,"UPDATE servers SET status='100' WHERE id='%d';",id);
    mysql_query(&db,query);

    printf("Close connection with: %s:%d",pack_serv->IP,pack_serv->Port);

    if (insim.isclose() < 0)
    {
        //cerr << "Error closing connection" << endl;
        return 0;
    }
    return 0;
}

DWORD WINAPI InsimGo(void* param)
{

    for (;;)
    {
        mysql_query(&db,"SELECT id,ip,port,admin,spec FROM servers WHERE status = '0' ORDER BY id;");
        res = mysql_store_result(&db);
        if (mysql_num_rows(res) > 0)
        {
            while (row = mysql_fetch_row(res))
            {

                struct SERVERS Server;
                ZeroMemory(&Server,sizeof(SERVERS));

                strncpy(Server.id,row[0],5);

                //memcpy(Server.id,row[0],5);
             //   cout << Server.id << endl;

                strncpy(Server.IP,row[1],16);
             //    cout << Server.IP << endl;
                Server.Port = atoi(row[2]);
                //cout << Server.Port << endl;
                strncpy(Server.AdminPass,row[3],24);
                //cout << Server.AdminPass << endl;
                strncpy(Server.SpecPass,row[4],24);
                //cout << Server.SpecPass << endl;
                CreateThread(NULL,0,InsimTread,&Server,0,NULL);
                //printf( "%s:%s - %s (%s)\n", row[1],row[2],row[3],row[4]);
                Sleep(2000);
            }

            mysql_free_result(res);
        }
        Sleep(60000);
    }

    return 0;
}
