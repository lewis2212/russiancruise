#include "main.h"

CInsim *insim;
GlobalInfo *ginfo;

int ok = 1;


struct IS_VER pack_ver;
word	isf_flag;

#ifdef _RC_QUEST_H
RCQuest quest;
#endif



void init_classes()
{

#ifdef _RC_QUEST_H
	quest.init(RootDir, insim);
#endif

}

void send_mst (const char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    sprintf( pack_mst.Msg, "%.63s\0", Text );
    insim->send_packet(&pack_mst);
};




int core_connect(void *pack_ver)
{
	memset(pack_ver,0,sizeof(struct IS_VER));
	struct IS_VER *pack_v = (IS_VER*)pack_ver;

	if (insim->init (ginfo->IP, ginfo->TCPPORT, IS_PRODUCT_NAME,ginfo->ADMIN,pack_v, '!', isf_flag,500, ginfo->UDPPORT) < 0)
	{
		return -1;
	}

	strcpy(ginfo->Product,pack_v->Product);

	struct IS_TINY pack_requests;
	memset(&pack_requests, 0, sizeof(struct IS_TINY));
	pack_requests.Size = sizeof(struct IS_TINY);
	pack_requests.Type = ISP_TINY;
	pack_requests.ReqI = 1;

	pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
	insim->send_packet(&pack_requests);

	pack_requests.SubT = TINY_NCN;      // Request all connections to store their user data
	insim->send_packet(&pack_requests);

	pack_requests.SubT = TINY_NPL;      // Request all players in-grid to know their PLID
	insim->send_packet(&pack_requests);


	pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
	insim->send_packet(&pack_requests);

	return 1;
}

int core_reconnect(void *pack_ver)
{
	insim->isclose();

	Sleep (60000);


	memset(pack_ver,0,sizeof(struct IS_VER));
	struct IS_VER *pack_v = (IS_VER*)pack_ver;

	if (insim->init (ginfo->IP, ginfo->TCPPORT, IS_PRODUCT_NAME,ginfo->ADMIN, pack_v, '!', isf_flag,500, ginfo->UDPPORT) < 0)
	{
		return -1;
	}

	strcpy(ginfo->Product,pack_v->Product);

	struct IS_TINY pack_requests;
	memset(&pack_requests, 0, sizeof(struct IS_TINY));
	pack_requests.Size = sizeof(struct IS_TINY);
	pack_requests.Type = ISP_TINY;
	pack_requests.ReqI = 1;

	pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
	insim->send_packet(&pack_requests);

	pack_requests.SubT = TINY_NCN;      // Request all connections to store their user data
	insim->send_packet(&pack_requests);

	pack_requests.SubT = TINY_NPL;      // Request all players in-grid to know their PLID
	insim->send_packet(&pack_requests);

	pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
	insim->send_packet(&pack_requests);

	return 1;
}


void read_cfg()
{

	char file[255];
	sprintf(file,"%smisc\\%s.cfg",RootDir,ServiceName);

	HANDLE fff;
	WIN32_FIND_DATA fd;
	fff = FindFirstFile(file,&fd);
	if (fff == INVALID_HANDLE_VALUE)
	{
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
				strncpy(ginfo->IP,str+3,strlen(str+3));

			// GET TCPPORT
			if (strncmp("TcpPort=",str,8)==0)
				ginfo->TCPPORT = atoi(str+8);

			// GET UDPPORT
			if (strncmp("UdpPort=",str,8)==0)
				ginfo->UDPPORT = atoi(str+8);

			//GET ADMIN PASSWORD
			if (strncmp("Admin=",str,6)==0)
				strncpy(ginfo->ADMIN,str+6,strlen(str+6));
		}
	}
}

/*********************************************/

void *thread_mci (void *params)
{
	while (true)
	{
		if (insim->udp_next_packet() < 0)
			continue;

		#ifdef _RC_QUEST_H
		quest.insim_mci();
		#endif

	}
	return 0;
};

DWORD WINAPI ThreadMain(void *CmdLine)
{


	sprintf(IS_PRODUCT_NAME,"RC-%d.%d.%d:%s",1,1,1,"100");

	pthread_mutex_init(&RCmutex, NULL);

	if (strlen(ServiceName) == 0)
	{
		return 0;
	}

	int error_ch;

	ginfo = new GlobalInfo();
	insim = new CInsim();

	read_cfg();

	if ( core_connect(&pack_ver) < 0 )
	{
		for (;;)
		{
			if ( core_reconnect(&pack_ver) > 0 )
				break;
		}
	}

	if (pack_ver.InSimVer != 5)
	{
		cout << "INSIM VER != 5" << endl;
		return 0;
	}

	send_mst("/msg ^C^3Russian Cruise квест");

	init_classes();
	Sleep(1000);

	while (ok > 0)
	{
		if (error_ch = insim->next_packet() < 0)
		{
			for (;;)
			{
				if ( core_reconnect(&pack_ver) > 0 )
					break;
			}

		}


	#ifdef _RC_QUEST_H
		quest.next_packet();
	#endif


	}

	if (insim->isclose() < 0)
		return 0;

	delete insim;
	delete ginfo;

	pthread_mutex_destroy(&RCmutex);

	return 0;
}


int  nCount;     // счетчик

// главная функция приложения
int main(int argc, char* argv[])
{

	isf_flag = ISF_MCI + ISF_MSO_COLS + ISF_CON + ISF_OBH + ISF_HLV + ISF_AXM_EDIT + ISF_AXM_LOAD;


	int need = 92;
	int d;

	for (d = strlen(argv[0]); d > 0; d--)
	{
		if ( int(argv[0][d]) == need )
			break;
	}

	strncpy(RootDir,argv[0],d+1);
	strcpy(ServiceName,argv[1]);

	SYSTEMTIME sm;
	GetLocalTime(&sm);

	CreateThread(NULL,0,ThreadMain,0,0,NULL);

	// рабочий цикл сервиса
	while (ok)
	{
		Sleep(1000);
	}


	Sleep(10000);
	return 0;
}



