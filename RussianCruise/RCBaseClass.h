using namespace std;
#ifndef RCBASECLASS_H
#define RCBASECLASS_H

#include "CInsim.h"

#include <math.h>
#include <stdio.h>      //
#include <iostream>
#include <fstream>
#include <ctime>
#include <time.h>       // ��� ������ � �������� � ��������
#include <windows.h>    // �� ����� ��� ���� �� ����� �����
#include <exception>
#include <map>

#include "mysql/include/mysql.h"

//#include "tools.h"

class RCBaseClass
{
public:
    RCBaseClass();
    ~RCBaseClass();

    void        next_packet();   // ������� ��������� ���� ������
    void        insim_mci( struct IS_MCI* packet );     // ����� � ������� � ����������� � �.�.

    virtual void send_bfn(byte UCID, byte ClickID);
    virtual void send_bfn_all ( byte UCID );
    virtual void send_mst (const char* Text);
    virtual void send_mtc (byte UCID,const char* Msg);
    virtual void btn_information(byte UCID, const char* Message);
    virtual void btn_information_clear(byte UCID);

    bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);
    int Distance (int X, int Y, int X1, int Y1);

protected:

    CInsim      *insim;         // ����������-��������� �� ����� CInsim
    char        RootDir[MAX_PATH]; // ������ ���� �� ����� � ����������
    std::map< byte, byte >PLIDtoUCID;

    virtual void    insim_ver( struct IS_VER* packet );
    virtual void    insim_tiny( struct IS_TINY* packet );
    virtual void    insim_small( struct IS_SMALL* packet );
    virtual void    insim_sta( struct IS_STA* packet );
    virtual void    insim_ism( struct IS_ISM* packet );
    virtual void    insim_iii( struct IS_III* packet );		// ������� ��������� /i
    virtual void    insim_vtn( struct IS_VTN* packet );		// �����������
    virtual void    insim_rst( struct IS_RST* packet );		// ������ �����
    virtual void    insim_lap( struct IS_LAP* packet );		// ����� ������ ���� (�����)
    virtual void    insim_spx( struct IS_SPX* packet );		// ����� ������� ������ (�����)
    virtual void    insim_pit( struct IS_PIT* packet );		// ����� ��������� �������
    virtual void    insim_psf( struct IS_PSF* packet );		// ����� �������� �������
    virtual void    insim_pla( struct IS_PLA* packet );		// ����� ������ ��� ������ � ��������
    virtual void    insim_pen( struct IS_PEN* packet );		// ����� ������� �������� (����� ������ ����� �����)
    virtual void    insim_toc( struct IS_TOC* packet );		// ����� ��� � ������ ������� ������
    virtual void    insim_flg( struct IS_FLG* packet );
    virtual void    insim_reo( struct IS_REO* packet );
    virtual void    insim_crs( struct IS_CRS* packet );		// ����� ����������� ������
    virtual void    insim_bfn( struct IS_BFN* packet );		// ����� ����� shift + i
    virtual void    insim_btc( struct IS_BTC* packet );		// ����� ����� �� ������
    virtual void    insim_btt( struct IS_BTT* packet );		// ����� �������� ����� � ������
    virtual void    insim_axm( struct IS_AXM* packet );
    virtual void    insim_acr( struct IS_ACR* packet );
    virtual void    insim_ncn( struct IS_NCN* packet );		// ����� ����� ����� �� ������
    virtual void    insim_npl( struct IS_NPL* packet );		// ����� ����� �� ������
    virtual void    insim_plp( struct IS_PLP* packet );		// ����� ���� � �����
    virtual void    insim_pll( struct IS_PLL* packet );		// ����� ���� � �������
    virtual void    insim_cnl( struct IS_CNL* packet );		// ����� ���� � �������
    virtual void    insim_cpr( struct IS_CPR* packet );		// ����� ��������������
    virtual void    insim_mso( struct IS_MSO* packet );		// ����� �������� ���������
    virtual void    insim_con( struct IS_CON* packet );		// ����� ����������� �������
    virtual void    insim_obh( struct IS_OBH* packet );		// ����� ����������� ��������



private:
};

#endif // RCBASECLASS_H
