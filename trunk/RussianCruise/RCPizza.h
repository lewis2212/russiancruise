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

struct PizzaPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    char    CName[4];              // Car Name
    byte    Zone;
    byte    Pizza;                  // Если игрок заказал пиццу, то его ставят в очередь
    /** Work **/
    char    WorkDest[96];       // destination text
    byte    WorkType;
    byte    WorkZone;
    byte    WorkAccept;			// 0 = не занят работой , 1 = занят работой
    byte    WorkPlayerAccept;   // если какойто плеер заказал пиццу (100 + позиция в массиве)
    byte    WorkDestinaion;		// номер точки доставки
    int     WorkTime;			// время за которое он должен доставить товар
    int     WorkCountDone;
};

enum
{
    WK_NULL = 0,
    WK_PIZZA = 1,
};

struct  Store
{
    /**
    *   Одна порция пиццы - 1 кг муки 0.6 кг вода, 0.9 томатов, 0.4 сыра
    *   Себестоимость за порцию = 12+ 9 +72 +224 = 317 руб
    *   Окончательная цена 552 руб
    *   оплата за изготовление 235 руб
    *   оплата за доставку 248 руб.
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

    void InsimNCN( struct IS_NCN* packet );
    void InsimNPL( struct IS_NPL* packet );
    void InsimPLP( struct IS_PLP* packet );
    void InsimPLL( struct IS_PLL* packet );
    void InsimCNL( struct IS_CNL* packet );
    void InsimCPR( struct IS_CPR* packet );
    void InsimMSO( struct IS_MSO* packet );

    // Функции-утилиты
    int check_pos ( byte UCID );

public:
    RCPizza(const char* Dir);
    ~RCPizza();

    // Основные функции класса
    int init(MYSQL *conn, CInsim *InSim, void *Message,void *Bank,void *Energy,void *DrLic, void * STreet);
    void readconfig(const char *Track);


    void InsimMCI( struct IS_MCI* packet );

    bool IfWork(byte UCID);
    void Event();
};

#endif
