#ifndef _RC_ROADSIGN
#define _RC_ROADSIGN

#include "RCBaseClass.h"
#include "RCLight.h"

struct RoadPlayer: GlobalPlayer
{
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


    void InsimNCN(struct IS_NCN* packet);   // ����� ����� ����� �� ������
    void InsimNPL(struct IS_NPL* packet);   // ����� ����� �� ������
    void InsimPLP(struct IS_PLP* packet);   // ����� ���� � �����
    void InsimPLL(struct IS_PLL* packet);   // ����� ���� � �������
    void InsimCNL(struct IS_CNL* packet);   // ����� ���� � �������
    void InsimCPR(struct IS_CPR* packet);   // ����� ��������������
    void InsimMSO(struct IS_MSO* packet);   // ����� �������� ���������

    void ShowSign(byte UCID, byte ID, byte Count);

public:
    RCRoadSign(const char* Dir);
    ~RCRoadSign();
    int init(DBMySQL *db,CInsim *InSim, RCLight * Light);
    void ReadConfig(const char *Track);
    void InsimMCI(struct IS_MCI* packet);
};

#endif // _RC_ROADSIGN
