using namespace std;
#ifndef RCBASECLASS_H
#define RCBASECLASS_H

#include "CInsim.h"

#include "RCButtonClickID.h"

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <time.h>       // ��� ������ � �������� � ��������

#include <list>
#include <exception>
#include <map>
#include <vector>
#include <algorithm>

#include <direct.h>

#include "mysql/include/mysql.h"

//#include "tools.h"
#define MAX_PATH	260

/**< ���������� - ������������ ������ ���� "�������� ����" -> "�������� ����" */
typedef std::map< string, string > DB_ROW;

/**< ���������� - ������ ��������� �� ��������� DB_ROW */
typedef list<DB_ROW> DB_ROWS;

struct GlobalPlayer
{
    struct  	CompCar Info;
    char    	UName[24];              // Username
    char    	PName[24];              // Player name
    char    	CName[4];             // Car Name

    bool DontSave;
};

class RCBaseClass
{
public:
    RCBaseClass();
    ~RCBaseClass();

    void        next_packet();   						// ������� ��������� ���� ������
    void        InsimMCI( struct IS_MCI* packet );     	// ����� � ������� � ����������� � �.�.

    virtual void SendBFN(byte UCID, byte ClickID);
    virtual void SendBFNAll ( byte UCID );
    virtual void SendMST (const char* Text);

    virtual void SendMTC (byte UCID, string Msg);

    virtual void SendButton (byte ReqI, byte UCID, byte ClickID, byte L, byte T, byte W, byte H, byte BStyle, const char * Text, byte TypeIn = 0 );
    virtual void SendStringButton (byte ReqI, byte UCID, byte ClickID, byte L, byte T, byte W, byte H, byte BStyle, string Text, byte TypeIn = 0 );

    virtual void ButtonInfo (byte UCID, const char* Message);
    virtual void ClearButtonInfo (byte UCID);

    static bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);
    static int Distance (int X, int Y, int X1, int Y1);

    /** ��������� ������� ���������� ������������ � �������
     *
     * @param  byte Reason - �������� ������������� ����
     * @return static const char* - ��������� �������� ����
     *
     */
    static const char* GetReason(byte Reason);

    virtual void AddMarshal();
    virtual void DeleteMarshal();



	string NumToString (int n);

	inline std::string trim(const std::string &s);

protected:
    CInsim      *insim;         	// ����������-��������� �� ����� CInsim
    char        RootDir[MAX_PATH]; 	// ������ ���� �� ����� � ����������
    std::map< byte, byte >PLIDtoUCID;

    virtual void InsimACR( struct IS_ACR* packet ){}
    virtual void InsimAXM( struct IS_AXM* packet ){}
    virtual void InsimBFN( struct IS_BFN* packet ){}
    virtual void InsimBTC( struct IS_BTC* packet ){}
    virtual void InsimBTT( struct IS_BTT* packet ){}
    virtual void InsimCNL( struct IS_CNL* packet ){}
    virtual void InsimCON( struct IS_CON* packet ){}
    virtual void InsimCPR( struct IS_CPR* packet ){}
    virtual void InsimCRS( struct IS_CRS* packet ){}
    virtual void InsimFLG( struct IS_FLG* packet ){}
    virtual void InsimHLV( struct IS_HLV* packet ){}
    virtual void InsimIII( struct IS_III* packet ){}
    virtual void InsimISM( struct IS_ISM* packet ){}
    virtual void InsimLAP( struct IS_LAP* packet ){}
    virtual void InsimMSO( struct IS_MSO* packet ){}
    virtual void InsimNCN( struct IS_NCN* packet ){}
    virtual void InsimNPL( struct IS_NPL* packet ){}
    virtual void InsimOBH( struct IS_OBH* packet ){}
    virtual void InsimPEN( struct IS_PEN* packet ){}
    virtual void InsimPIT( struct IS_PIT* packet ){}
    virtual void InsimPLA( struct IS_PLA* packet ){}
    virtual void InsimPLL( struct IS_PLL* packet ){}
    virtual void InsimPLP( struct IS_PLP* packet ){}
    virtual void InsimPSF( struct IS_PSF* packet ){}
    virtual void InsimREO( struct IS_REO* packet ){}
    virtual void InsimRST( struct IS_RST* packet ){}
    virtual void InsimSMALL( struct IS_SMALL* packet ){}
    virtual void InsimSPX( struct IS_SPX* packet ){}
    virtual void InsimSTA( struct IS_STA* packet ){}
    virtual void InsimTINY( struct IS_TINY* packet ){}
    virtual void InsimTOC( struct IS_TOC* packet ){}
    virtual void InsimVER( struct IS_VER* packet ){}
    virtual void InsimVTN( struct IS_VTN* packet ){}

    //MYSQL
    MYSQL       *dbconn;
    MYSQL_RES   *dbres;
    MYSQL_ROW   dbrow;

    /** ������� ������ �� ���� ������
	 *
	 * @param string query - ����������� SQL �������
	 * @return DB_ROWS - ������ ��������� DB_ROW
	 *
	 */
	DB_ROWS dbSelect( string query );

	/** ���������� SQL �������, ������� �� ���������� ������ (���������� �����, ��������� �������)
	 *
	 * @param string query - ����������� SQL �������
	 * @return bool - ��������� ������� mysql_query()
	 *
	 */
	bool dbExec( string query );
	bool dbExec( const char *query );

	/** ���������� ������ � ����
	 *
	 * @param string query - ����������� SQL �������
	 * @return unsigned int - ID ��������� ���������� ������, ���� ���� PRIMARY KEY (��������� ������� SELECT LAST_INSERT_ID();)
	 *
	 */
	unsigned int dbInsert( string query );
	unsigned int dbInsert( const char *query );


private:

};

class SplitString : public string
{
    std::vector<string> flds;
public:
    SplitString(char *s) : string(s) { };
    SplitString(const char *s) : string(s) { };
    SplitString(string s) : string(s) { };
    SplitString() : string() { };
    std::vector<string>& split( const char delim, int rep=0);
};

#endif // RCBASECLASS_H
