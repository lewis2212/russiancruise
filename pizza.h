#ifndef _RC_PIZZA_H
#define _RC_PIZZA_H

//typedef unsigned char byte;
//typedef unsigned short word;

#include <stdio.h>
#include "CInsim.h"

void *pizzathread(void *arg);// Поток предназначен для того чтобы работали часики, которые отсчитывают обратный счет


struct PizzaPlayer
{
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    char    CName[4];              // Car Name
    /** Work **/
    byte    WorkZone;
    byte    WorkAccept;			// 0 = не занят работой , 1 = занят работой
    byte    WorkPlayerAccept;   // если какойто плеер заказал пиццу (100 + позиция в массиве)
    byte    WorkDestinaion;		// номер точки доставки
    int     WorkTime;			// время за которое он должен доставить товар
    int     WorkCountDone;
};


class Pizza{
    private:
    CInsim  *insim;
    struct  PizzaPlayer players[32];     // Array of players
    pthread_t tid; // Thread ID
    pthread_attr_t attr;
    int init(void *classname, void *CInSim);  // classname - указатель на класс Pizza. Нужно для доступа к классу внутри потока
                                // Эта штука нужна для того чтобы отдельно запущенный поток имел доступ к классу Pizza
    public:
    Pizza::Pizza();
    Pizza::~Pizza();
    void Pizza::pizza_ncn();
    void Pizza::pizza_npl();
    void Pizza::pizza_plp();
    void Pizza::pizza_pll();
    void Pizza::pizza_cnl();
    void Pizza::pizza_crp();
    void Pizza::pizza_mci();
    void Pizza::pizza_mso();

};

#endif
