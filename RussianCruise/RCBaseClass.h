using namespace std;
#ifndef RCBASECLASS_H
#define RCBASECLASS_H

#include "CInsim.h"

#include <math.h>
#include <stdio.h>      //
#include <iostream>
#include <fstream>
#include <ctime>
#include <time.h>       // для работы с временем и рандомом
#include <windows.h>    // не помню для чего но ингда нужно
#include <exception>
#include <map>

#include "mysql/include/mysql.h"

//#include "tools.h"

class RCBaseClass
{
public:
    RCBaseClass();
    ~RCBaseClass();

    void        next_packet();   // Функция переборки типа пакета
    void        insim_mci( struct IS_MCI* packet );     // Пакет с данными о координатах и т.д.

    virtual void send_bfn(byte UCID, byte ClickID);
    virtual void send_bfn_all ( byte UCID );
    virtual void send_mst (const char* Text);
    virtual void send_mtc (byte UCID,const char* Msg);
    virtual void btn_information(byte UCID, const char* Message);
    virtual void btn_information_clear(byte UCID);

    bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);
    int Distance (int X, int Y, int X1, int Y1);

protected:

    CInsim      *insim;         // Переменная-указатель на класс CInsim
    char        RootDir[MAX_PATH]; // Полный путь до папки с программой
    std::map< byte, byte >PLIDtoUCID;

    virtual void    insim_ver( struct IS_VER* packet );
    virtual void    insim_tiny( struct IS_TINY* packet );
    virtual void    insim_small( struct IS_SMALL* packet );
    virtual void    insim_sta( struct IS_STA* packet );
    virtual void    insim_ism( struct IS_ISM* packet );
    virtual void    insim_iii( struct IS_III* packet );		// скрытое сообщение /i
    virtual void    insim_vtn( struct IS_VTN* packet );		// Голосование
    virtual void    insim_rst( struct IS_RST* packet );		// Начало гонки
    virtual void    insim_lap( struct IS_LAP* packet );		// Игрок сделал круг (гонка)
    virtual void    insim_spx( struct IS_SPX* packet );		// Игрок пересек сектор (гонка)
    virtual void    insim_pit( struct IS_PIT* packet );		// Игрок совершает питстоп
    virtual void    insim_psf( struct IS_PSF* packet );		// Игрок завершил питстоп
    virtual void    insim_pla( struct IS_PLA* packet );		// Игрок въехал или выехал с питлейна
    virtual void    insim_pen( struct IS_PEN* packet );		// Игрок получил пенальти (штраф проезд через боксы)
    virtual void    insim_toc( struct IS_TOC* packet );		// Игрок сел в машину другого игрока
    virtual void    insim_flg( struct IS_FLG* packet );
    virtual void    insim_reo( struct IS_REO* packet );
    virtual void    insim_crs( struct IS_CRS* packet );		// Игрок восстановил машину
    virtual void    insim_bfn( struct IS_BFN* packet );		// Игрок нажал shift + i
    virtual void    insim_btc( struct IS_BTC* packet );		// Игрок нажал на кнопку
    virtual void    insim_btt( struct IS_BTT* packet );		// Игрок отправил текст в кнопке
    virtual void    insim_axm( struct IS_AXM* packet );
    virtual void    insim_acr( struct IS_ACR* packet );
    virtual void    insim_ncn( struct IS_NCN* packet );		// Новый игрок зашел на сервер
    virtual void    insim_npl( struct IS_NPL* packet );		// Игрок вышел из боксов
    virtual void    insim_plp( struct IS_PLP* packet );		// Игрок ушел в боксы
    virtual void    insim_pll( struct IS_PLL* packet );		// Игрок ушел в зрители
    virtual void    insim_cnl( struct IS_CNL* packet );		// Игрок ушел с сервера
    virtual void    insim_cpr( struct IS_CPR* packet );		// Игрок переименовался
    virtual void    insim_mso( struct IS_MSO* packet );		// Игрок отправил сообщение
    virtual void    insim_con( struct IS_CON* packet );		// Игрок столкнулсяс машиной
    virtual void    insim_obh( struct IS_OBH* packet );		// Игрок столкнулсяс объектом



private:
};

#endif // RCBASECLASS_H
