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
#include <vector>
#include <algorithm>

#include "mysql/include/mysql.h"

//#include "tools.h"

class RCBaseClass
{
public:
    RCBaseClass();
    ~RCBaseClass();

    void        next_packet();   // ������� ��������� ���� ������
    void        InsimMCI( struct IS_MCI* packet );     // ����� � ������� � ����������� � �.�.

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

    CInsim      *insim;         // ����������-��������� �� ����� CInsim
    char        RootDir[MAX_PATH]; // ������ ���� �� ����� � ����������
    std::map< byte, byte >PLIDtoUCID;

    virtual void    InsimVER( struct IS_VER* packet );
    virtual void    InsimTINY( struct IS_TINY* packet );
    virtual void    InsimSMALL( struct IS_SMALL* packet );
    virtual void    InsimSTA( struct IS_STA* packet );
    virtual void    InsimISM( struct IS_ISM* packet );
    virtual void    InsimIII( struct IS_III* packet );		// ������� ��������� /i
    virtual void    InsimVTN( struct IS_VTN* packet );		// �����������
    virtual void    InsimRST( struct IS_RST* packet );		// ������ �����
    virtual void    InsimLAP( struct IS_LAP* packet );		// ����� ������ ���� (�����)
    virtual void    InsimSPX( struct IS_SPX* packet );		// ����� ������� ������ (�����)
    virtual void    InsimPIT( struct IS_PIT* packet );		// ����� ��������� �������
    virtual void    InsimPSF( struct IS_PSF* packet );		// ����� �������� �������
    virtual void    InsimPLA( struct IS_PLA* packet );		// ����� ������ ��� ������ � ��������
    virtual void    InsimPEN( struct IS_PEN* packet );		// ����� ������� �������� (����� ������ ����� �����)
    virtual void    InsimTOC( struct IS_TOC* packet );		// ����� ��� � ������ ������� ������
    virtual void    InsimFLG( struct IS_FLG* packet );
    virtual void    InsimREO( struct IS_REO* packet );
    virtual void    InsimCRS( struct IS_CRS* packet );		// ����� ����������� ������
    virtual void    InsimBFN( struct IS_BFN* packet );		// ����� ����� shift + i
    virtual void    InsimBTC( struct IS_BTC* packet );		// ����� ����� �� ������
    virtual void    InsimBTT( struct IS_BTT* packet );		// ����� �������� ����� � ������
    virtual void    InsimAXM( struct IS_AXM* packet );
    virtual void    InsimACR( struct IS_ACR* packet );
    virtual void    InsimNCN( struct IS_NCN* packet );		// ����� ����� ����� �� ������
    virtual void    InsimNPL( struct IS_NPL* packet );		// ����� ����� �� ������
    virtual void    InsimPLP( struct IS_PLP* packet );		// ����� ���� � �����
    virtual void    InsimPLL( struct IS_PLL* packet );		// ����� ���� � �������
    virtual void    InsimCNL( struct IS_CNL* packet );		// ����� ���� � �������
    virtual void    InsimCPR( struct IS_CPR* packet );		// ����� ��������������
    virtual void    InsimMSO( struct IS_MSO* packet );		// ����� �������� ���������
    virtual void    InsimCON( struct IS_CON* packet );		// ����� ����������� �������
    virtual void    InsimOBH( struct IS_OBH* packet );		// ����� ����������� ��������



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
