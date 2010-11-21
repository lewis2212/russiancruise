#ifndef _RC_PIZZA_H
#define _RC_PIZZA_H

//typedef unsigned char byte;
//typedef unsigned short word;

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <windows.h>
#include "CInsim.h"
#include "message.h"

void *pizzathread(void *arg);// Поток предназначен для того чтобы работали часики, которые отсчитывают обратный счет


struct PizzaPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    char    CName[4];              // Car Name
    byte    Zone;
    /** Work **/
    byte    WorkType;
    byte    WorkZone;
    byte    WorkAccept;			// 0 = не занят работой , 1 = занят работой
    byte    WorkPlayerAccept;   // если какойто плеер заказал пиццу (100 + позиция в массиве)
    byte    WorkDestinaion;		// номер точки доставки
    int     WorkTime;			// время за которое он должен доставить товар
    int     WorkCountDone;
};

struct square           // square of destination point
{
    int     X[5];       // 4 X points
    int     Y[5];       // 4 Y points
    char    Place[64];  // The name of Destination Point
};

struct place
{
    int     dealX[4];       // 4 X points of Dealer Place
    int     dealY[4];       // 4 Y points of Dealer Place
    int     NumPoints;      // Count of Destination points/ Need for random
    struct  square point[40]; // Destination points. See Bellow
};

enum
{
    WK_NULL = 0,
    WK_PIZZA = 1,
};

class RCPizza
{
private:


    pthread_t tid; // Thread ID
    pthread_attr_t attr;


public:
    RCPizza::RCPizza();
    RCPizza::~RCPizza();

    CInsim  *insim;
    RCMessage *msg;

    struct  place zone;
    struct  PizzaPlayer players[32];     // Array of players
    // Основные функции класса
    int init(void *classname, void *CInSim, void *Message);    // classname - указатель на класс RCPizza. Нужно для доступа к классу внутри потока
    // Эта штука нужна для того чтобы отдельно запущенный поток имел доступ к классу RCPizza
    void readconfig(char *Track);
    void deal(struct PizzaPlayer *splayer);
    void undeal(struct PizzaPlayer *splayer, char *Reason);
    void take (struct PizzaPlayer *splayer);
    void done (struct PizzaPlayer *splayer);

    // функции-повторители основных фунцкий ядра
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

    // Функции-утилиты
    int check_pos (struct PizzaPlayer *splayer); //+

};

#endif
