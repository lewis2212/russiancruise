#ifndef RCBASECLASS_H
#define RCBASECLASS_H

#include "CInsim.h"

#include <math.h>
#include <stdio.h>      //
#include <iostream>     //
#include <time.h>       // для работы с временем и рандомом
#include <windows.h>    // не помню для чего но ингда нужно

class RCBaseClass
{
public:
    RCBaseClass();
    ~RCBaseClass();
    // функции-повторители основных фунцкий ядра
    void        next_packet();   // Функция переборки типа пакета
    void        insim_mci();     // Пакет с данными о координатах и т.д.

    virtual void send_bfn(byte UCID, byte ClickID);
    virtual void send_mst (const char* Text);
    virtual void send_mtc (byte UCID,const char* Msg);
    virtual void btn_information(byte UCID, const char* Message);
    virtual void btn_information_clear(byte UCID);

    static bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);
    static int Distance (int X, int Y, int X1, int Y1);

protected:

    CInsim      *insim;         // Переменная-указатель на класс CInsim
    char        RootDir[MAX_PATH]; // Полный путь до папки с программой

    virtual void    insim_ver();
    virtual void    insim_tiny();
    virtual void    insim_small();
    virtual void    insim_sta();
    virtual void    insim_ism();
    virtual void    insim_iii();
    virtual void    insim_vtn();
    virtual void    insim_rst();
    virtual void    insim_lap();
    virtual void    insim_spx();
    virtual void    insim_pit();
    virtual void    insim_psf();
    virtual void    insim_pla();
    virtual void    insim_pen();
    virtual void    insim_toc();
    virtual void    insim_flg();
    virtual void    insim_reo();
    virtual void    insim_crs();
    virtual void    insim_bfn();
    virtual void    insim_btc();
    virtual void    insim_btt();
    virtual void    insim_axm();
    virtual void    insim_acr();
    virtual void    insim_ncn();    // Новый игрок зашел на сервер
    virtual void    insim_npl();    // Игрок вышел из боксов
    virtual void    insim_plp();    // Игрок ушел в боксы
    virtual void    insim_pll();    // Игрок ушел в зрители
    virtual void    insim_cnl();    // Игрок ушел с сервера
    virtual void    insim_crp();    // Игрок переименовался
    virtual void    insim_mso();    // Игрок отправил сообщение
    virtual void    insim_con();    // Игрок отправил сообщение
    virtual void    insim_obh();    // Игрок отправил сообщение



private:
};

#endif // RCBASECLASS_H
