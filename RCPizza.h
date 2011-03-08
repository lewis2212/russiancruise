#ifndef _RC_PIZZA_H
#define _RC_PIZZA_H

//typedef unsigned char byte;
//typedef unsigned short word;

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <windows.h>

#include "CInsim.h"
#include "RCMessage.h"
#include "RCBank.h"
#include "RCEnergy.h"

#include "tools.h"

void *pizzathread(void *arg);// Поток предназначен для того чтобы работали часики, которые отсчитывают обратный счет

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
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
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

class RCPizza
{
private:

    u_int   Capital;
    int     NumCars;




    pthread_t tid; // Thread ID
    pthread_attr_t attr;

    char RootDir[MAX_PATH];


public:
    RCPizza::RCPizza();
    RCPizza::~RCPizza();

    bool    ShopAccepted;
    int     CarsInWork;

    int NumP;
    int ginfo_time;
    struct Store PStore;
    struct  pizza_info TrackInf;

    CInsim      *insim;
    RCMessage   *msg;
    RCBank      *bank;
    RCEnergy    *nrg;

    struct  place zone;
    struct  PizzaPlayer players[32];     // Array of players
    // Основные функции класса
    int init(char *dir,void *classname,void *CInSim, void *Message,void *Bank,void *Energy);    // classname - указатель на класс RCPizza. Нужно для доступа к классу внутри потока
    // Эта штука нужна для того чтобы отдельно запущенный поток имел доступ к классу RCPizza
    void readconfig(char *Track);
    void deal(struct PizzaPlayer *splayer);
    void undeal(struct PizzaPlayer *splayer, char *Reason);
    void take (struct PizzaPlayer *splayer);
    void done (struct PizzaPlayer *splayer);

    // функции-повторители основных фунцкий ядра
    void next_packet();
    void pizza_ncn(); //+
    void pizza_npl(); //+
    void pizza_plp(); //+
    void pizza_pll(); //+
    void pizza_cnl(); //+
    void pizza_crp(); //+
    void pizza_mci();
    void pizza_mso();

    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);
    void btn_work (struct PizzaPlayer *splayer);
    void btn_destination(struct PizzaPlayer *splayer);

    // Функции-утилиты
    int check_pos (struct PizzaPlayer *splayer); //+

};

#endif
