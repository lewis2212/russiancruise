#ifndef _RC_LIGHT_H // ��������, ����� ���� ����������� ���� ���
#define _RC_LIGHT_H

#include "RCBaseClass.h"
#include "RCMessage.h"  // Messages
#include "RCDrivingLicense.h"

#include "tools.h"      // Check_Pos  etc.

struct LghPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    Light;
    bool    Light3;
    /** misc **/
    byte	WrongWay;
    bool	RedLight;
    bool	GreenLight;
    byte 	LightNum;
    byte 	DiffDir;
    bool 	OnRed = false; //������� �� �� �������
};

struct Lights
{
    byte    ID;
    int     Heading;
    int     PointCount;
    int     *X;
    int     *Y;
};

class RCLight:public RCBaseClass
{
private:
    char RootDir[MAX_PATH]; // ������ ���� �� ����� � ����������
    char TrackName[5];
    RCMessage   *msg;   // ����������-��������� �� ����� RCMessage
    RCDL        *dl;

    map< byte, LghPlayer > players;

    int     LightsCount;
    struct  Lights Light[30];

    bool	LightWorks=true;

    int red1, red2, yell1, yell2, green1, green2;
    bool gff=false;

    time_t sstime;

    void InsimNCN( struct IS_NCN* packet );   // ����� ����� ����� �� ������
    void InsimNPL( struct IS_NPL* packet );   // ����� ����� �� ������
    void InsimPLP( struct IS_PLP* packet );   // ����� ���� � �����
    void InsimPLL( struct IS_PLL* packet );   // ����� ���� � �������
    void InsimCNL( struct IS_CNL* packet );   // ����� ���� � �������
    void InsimCPR( struct IS_CPR* packet );   // ����� ��������������
    void InsimMSO( struct IS_MSO* packet );   // ����� �������� ���������


    void Svetofor1 ( byte UCID );
    void Svetofor2 ( byte UCID );
    void Svetofor3 ( byte UCID );
    void WrongWay ( byte UCID );


public:
    RCLight(const char* Dir);   // ����������� ������ (�����������)
    ~RCLight();  // ���������� ������ (�����������)

    bool SetLight3(byte UCID,bool Key);
    void Event();

    bool GetOnLight(byte UCID);
    void OnRedFalse(byte UCID);
    bool CheckOnRed(byte UCID);
    int CheckLight(byte UCID);

    int init(MYSQL *conn,CInsim *InSim, void *Message, void *RCDLic);    // classname - ��������� �� ����� RCStreet.
    void ReadConfig(const char *Track); // ������ ������ � ������ "����� ����������"
    void InsimMCI( struct IS_MCI* packet );   // ����� � ������� � ����������� � �.�.
};
#endif // #define _RC_STREET_H
