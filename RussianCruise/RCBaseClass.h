using namespace std;
#ifndef RCBASECLASS_H
#define RCBASECLASS_H

#include "CInsim.h"

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <time.h>       // для работы с временем и рандомом
#include <exception>
#include <map>
#include <vector>
#include <algorithm>

#include "mysql/include/mysql.h"

//#include "tools.h"

class RCBaseClass
{
public:
    RCBaseClass();
    ~RCBaseClass();

    void        next_packet();   // Функция переборки типа пакета
    void        InsimMCI( struct IS_MCI* packet );     // Пакет с данными о координатах и т.д.

    virtual void SendBFN(byte UCID, byte ClickID);
    virtual void SendBFNAll ( byte UCID );
    virtual void SendMST (const char* Text);
    virtual void SendMTC (byte UCID,const char* Msg);
    virtual void SendButton (byte ReqI, byte UCID, byte ClickID, byte L, byte T, byte W, byte H, byte BStyle, const char * Text );
    virtual void btn_information(byte UCID, const char* Message);
    virtual void btn_information_clear(byte UCID);

    bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);
    int Distance (int X, int Y, int X1, int Y1);

protected:

    CInsim      *insim;         // Переменная-указатель на класс CInsim
    char        RootDir[MAX_PATH]; // Полный путь до папки с программой
    std::map< byte, byte >PLIDtoUCID;

    virtual void    InsimVER( struct IS_VER* packet );
    virtual void    InsimTINY( struct IS_TINY* packet );
    virtual void    InsimSMALL( struct IS_SMALL* packet );
    virtual void    InsimSTA( struct IS_STA* packet );
    virtual void    InsimISM( struct IS_ISM* packet );
    virtual void    InsimIII( struct IS_III* packet );		// скрытое сообщение /i
    virtual void    InsimVTN( struct IS_VTN* packet );		// Голосование
    virtual void    InsimRST( struct IS_RST* packet );		// Начало гонки
    virtual void    InsimLAP( struct IS_LAP* packet );		// Игрок сделал круг (гонка)
    virtual void    InsimSPX( struct IS_SPX* packet );		// Игрок пересек сектор (гонка)
    virtual void    InsimPIT( struct IS_PIT* packet );		// Игрок совершает питстоп
    virtual void    InsimPSF( struct IS_PSF* packet );		// Игрок завершил питстоп
    virtual void    InsimPLA( struct IS_PLA* packet );		// Игрок въехал или выехал с питлейна
    virtual void    InsimPEN( struct IS_PEN* packet );		// Игрок получил пенальти (штраф проезд через боксы)
    virtual void    InsimTOC( struct IS_TOC* packet );		// Игрок сел в машину другого игрока
    virtual void    InsimFLG( struct IS_FLG* packet );
    virtual void    InsimREO( struct IS_REO* packet );
    virtual void    InsimCRS( struct IS_CRS* packet );		// Игрок восстановил машину
    virtual void    InsimBFN( struct IS_BFN* packet );		// Игрок нажал shift + i
    virtual void    InsimBTC( struct IS_BTC* packet );		// Игрок нажал на кнопку
    virtual void    InsimBTT( struct IS_BTT* packet );		// Игрок отправил текст в кнопке
    virtual void    InsimAXM( struct IS_AXM* packet );
    virtual void    InsimACR( struct IS_ACR* packet );
    virtual void    InsimNCN( struct IS_NCN* packet );		// Новый игрок зашел на сервер
    virtual void    InsimNPL( struct IS_NPL* packet );		// Игрок вышел из боксов
    virtual void    InsimPLP( struct IS_PLP* packet );		// Игрок ушел в боксы
    virtual void    InsimPLL( struct IS_PLL* packet );		// Игрок ушел в зрители
    virtual void    InsimCNL( struct IS_CNL* packet );		// Игрок ушел с сервера
    virtual void    InsimCPR( struct IS_CPR* packet );		// Игрок переименовался
    virtual void    InsimMSO( struct IS_MSO* packet );		// Игрок отправил сообщение
    virtual void    InsimCON( struct IS_CON* packet );		// Игрок столкнулсяс машиной
    virtual void    InsimOBH( struct IS_OBH* packet );		// Игрок столкнулсяс объектом



private:
};

class SplitString : public string {
    std::vector<string> flds;
public:
    SplitString(char *s) : string(s) { };
    SplitString(string s) : string(s) { };
    SplitString() : string() { };
    std::vector<string>& split( const char delim, int rep=0);
};

#endif // RCBASECLASS_H
