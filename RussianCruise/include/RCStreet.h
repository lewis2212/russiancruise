#ifndef _RC_STREET_H // ��������, ����� ���� ����������� ���� ���
#define _RC_STREET_H

#include "RCBaseClass.h"
#include "RCMessage.h"  // Messages
#include "RCBank.h"     // Bank

#include "tools.h"      // Check_Pos  etc.

#define MAX_STREETS 32
#define DEFAULT_STREET_NUM 250
// ������ ��������� ������

struct StrPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    int     StreetNum;
};

struct streets
{
    int 	StreetID;
    char    StreetRu[50];
    char    StreetEn[50];
    word    PointCount;
    int     *StreetX = NULL;
    int     *StreetY = NULL;
    int     SpeedLimit;
};


// �������� ������ �����
class RCStreet:public RCBaseClass
{
private:
    char RootDir[MAX_PATH]; // ������ ���� �� ����� � ����������

    char errmsg[64];

    RCMessage   *msg;   // ����������-��������� �� ����� RCMessage
    RCBank      *bank;  // ����������-��������� �� ����� RCBank

    //struct  StrPlayer *players;     // ��������� �������
    map< byte, StrPlayer >players;

    // struct  streets2 Street2[30];             // Array of streets

    // �������-����������� �������� ������� ����
    bool InsimNCN( struct IS_NCN* packet );   // ����� ����� ����� �� ������
    bool InsimNPL( struct IS_NPL* packet );   // ����� ����� �� ������
    bool InsimPLP( struct IS_PLP* packet );   // ����� ���� � �����
    bool InsimPLL( struct IS_PLL* packet );   // ����� ���� � �������
    bool InsimCNL( struct IS_CNL* packet );   // ����� ���� � �������
    bool InsimCPR( struct IS_CPR* packet );   // ����� ��������������
    bool InsimMSO( struct IS_MSO* packet );   // ����� �������� ���������

    void BtnStreet( byte UCID );

public:
    // ���������� � �������, ��������� ��� ����

    RCStreet(const char* Dir);   // ����������� ������ (�����������)
    ~RCStreet();  // ���������� ������ (�����������)

    byte    StreetNums;                          // Count of streets
    struct  streets Street[ MAX_STREETS ];       // Array of streets
    Json::Value lang;

    bool IfInited;
    int CurentStreetNum(byte UCID);
    int CurentStreetInfo(void *StreetInfo, byte UCID);
    int CurentStreetInfoByNum(void *StreetInfo, int StrNum);
    int StreetCount();

    const char* GetStreetName(byte UCID, int StreetID);

    int init(MYSQL *conn,CInsim *InSim, void *Message);
    bool ReadConfig(const char *Track); 	// ������ ������ � ������ "����� ����������"

    bool InsimMCI( struct IS_MCI* packet );   // ����� � ������� � ����������� � �.�.
};
#endif // #define _RC_STREET_H
