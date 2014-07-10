#ifndef _RC_ROADSIGN
#define _RC_ROADSIGN

#include "RCBaseClass.h"
#include "RCLight.h"

struct RoadPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    int 	OnSign;
    byte 	SignCount;
};

struct Signs
{
    byte    ID;
    int     Heading;
    int     X;
    int     Y;
};

struct SignsN
{
    byte    ID;
    char  Name[128];
};

class RCRoadSign : public RCBaseClass
{
private:
    RCLight    	*lgh;

    char TrackName[5];
    vector  <Signs> Sign;
    struct  SignsN SignName[50];

    map<byte, RoadPlayer> players;


    bool InsimNCN(struct IS_NCN* packet);   // ����� ����� ����� �� ������
    bool InsimNPL(struct IS_NPL* packet);   // ����� ����� �� ������
    bool InsimPLP(struct IS_PLP* packet);   // ����� ���� � �����
    bool InsimPLL(struct IS_PLL* packet);   // ����� ���� � �������
    bool InsimCNL(struct IS_CNL* packet);   // ����� ���� � �������
    bool InsimCPR(struct IS_CPR* packet);   // ����� ��������������
    bool InsimMSO(struct IS_MSO* packet);   // ����� �������� ���������

    void ShowSign(byte UCID, byte ID, byte Count);

public:
    RCRoadSign(const char* Dir);
    ~RCRoadSign();
    int Init(MYSQL *conn,CInsim *InSim, void *Message, void * Light);
    bool ReadConfig(const char *Track);
    bool InsimMCI(struct IS_MCI* packet);
};

#endif // _RC_ROADSIGN
