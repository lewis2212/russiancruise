#ifndef _RC_PIZZA_H
#define _RC_PIZZA_H

#include "RCBaseClass.h"
#include "RCMessage.h"
#include "RCBank.h"
#include "RCEnergy.h"
#include "RCDrivingLicense.h"
#include "RCTaxi.h"
#include "RCStreet.h"

#include "tools.h"

#define PIZZA_WORK_TIME 360

struct pizza_info
{
    int     ShopCount;
    int     XShop[10];
    int     YShop[10];
};

struct PizzaPlayer: GlobalPlayer
{
    byte    Zone;
    byte    Pizza;                  // ���� ����� ������� �����, �� ��� ������ � �������
    /** Work **/
    char    WorkDest[96];       // destination text
    byte    WorkType;
    byte    WorkZone;
    byte    WorkAccept;			// 0 = �� ����� ������� , 1 = ����� �������
    byte    WorkPlayerAccept;   // ���� ������� ����� ������� ����� (100 + ������� � �������)
    byte    WorkDestinaion;		// ����� ����� ��������
    int     WorkTime;			// ����� �� ������� �� ������ ��������� �����
    int     WorkCountDone;
    bool    FreeEat;
};

enum
{
    WK_NULL = 0,
    WK_PIZZA = 1,
};

struct  Store
{
    /**
    *   ���� ������ ����� - 1 �� ���� 0.6 �� ����, 0.9 �������, 0.4 ����
    *   ������������� �� ������ = 12+ 9 +72 +224 = 317 ���
    *   ������������� ���� 552 ���
    *   ������ �� ������������ 235 ���
    *   ������ �� �������� 248 ���.
    **/
    // all max = 1000 (1000kg)
    float    Muka; // 12000 rur/t.
    float    Voda; // 10000 rur/t
    float    Ovoshi; // 80000 rur/t
    float    Cheese; // 560000 rur/t
};

class RCPizza:public RCBaseClass
{
private:

    char RootDir[MAX_PATH];

    RCMessage   *msg;
    RCBank      *bank;
    RCEnergy    *nrg;
    RCStreet    *street;
#ifdef _RC_LEVEL_H
    RCDL        *dl;
#endif

    u_int   Capital = 0;
    int     NumCars = 0;
    bool    ShopAccepted = false;
    int     CarsInWork = 0;
    //int     Next;

    int NumP = 0;
    int ginfo_time;
    struct Store PStore;
    struct pizza_info TrackInf;
    struct place zone;

    map<byte,PizzaPlayer>players;

    void Deal( byte UCID );
    void Undeal( byte UCID ,const char *Reason);
    void Take ( byte UCID );
    void Done ( byte UCID );
    void btn_work ( byte UCID );

    void InsimCNL( struct IS_CNL* packet );
    void InsimCPR( struct IS_CPR* packet );
    void InsimMCI( struct IS_MCI* packet );
    void InsimMSO( struct IS_MSO* packet );
    void InsimNCN( struct IS_NCN* packet );
    void InsimNPL( struct IS_NPL* packet );
    void InsimPLL( struct IS_PLL* packet );
    void InsimPLP( struct IS_PLP* packet );

    // �������-�������
    int check_pos ( byte UCID );

public:
    RCPizza(const char* Dir);
    ~RCPizza();

    // �������� ������� ������
    int init(DBMySQL *db, CInsim *InSim, void *Message,void *Bank,void *Energy,void *DrLic, void * STreet);
    void ReadConfig(const char *Track);



    bool IfWork(byte UCID);
    void Event();
};

#endif
