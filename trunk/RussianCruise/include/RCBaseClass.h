using namespace std;
#ifndef RCBASECLASS_H
#define RCBASECLASS_H

#include "CInsim.h"

#include <CarInfo.h>

#include "RCButtonClickID.h"

#define M_PI 3.14159265358979323846
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <time.h>       // ��� ������ � �������� � ��������

// c++
#include <algorithm>
#include <exception>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <vector>

#include <sys/stat.h>

#include <mysql/mysql.h>

#include <json/json.h>

#include "tools.h"
#define MAX_PATH	260

/**< ���������� - ������� ������� �������� */
typedef queue< ObjectInfo > ObjectsInfo;

/**< ���������� - ������������ ������ ���� "�������� ����" -> "�������� ����" */
typedef map< string, string > DB_ROW;

/**< ���������� - ������ ��������� �� ��������� DB_ROW */
typedef vector<DB_ROW> DB_ROWS;

struct GlobalPlayer
{
    CarInfo Info;
    string	UName;              // Username
    string	PName;              // Player name
    string	CName;             // Car Name
    byte    Admin;             // is Admin?

    bool Loaded = false;
};

class RCBaseClass
{
public:
    RCBaseClass();
    ~RCBaseClass();

    string ClassName = "RCBaseClass";

    void		 next_packet();   						// ������� ��������� ���� ������
    void         upd_next_packet();
	virtual void Save(byte UCID){};
    virtual void Event(){};

    void ButtonInfo (byte UCID, const char* Message);
    void ClearButtonInfo (byte UCID);
    void ShowPanel(byte UCID, string Caption, list<string>Text);

    static bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);
    static int Distance (int X, int Y, int X1, int Y1);
    static void CCText(string Text, bool useTime = true);

    static const char* GetReason(byte Reason);
    static string packetType(byte type);


    void AddObjects();
	void DelObjects();
	void AddObject( ObjectInfo *object );
	void DelObject( ObjectInfo *object );

	static string ToString (int i);
	static string ToString (unsigned int i);
	static string ToString (byte b);
	static string ToString (bool b);
	static string ToString (float f);
	static string ToString (double d);
	static string ToString (const char *c);

	inline string Trim(const string &s);

	static string StringFormat(const string fmt_str, ...);

	static inline bool FileExists (const std::string& name)
	{
		struct stat status;
        return ( stat(name.c_str(),&status)==0 );
	}

protected:
    CInsim      *insim;				// ����������-��������� �� ����� CInsim
    char        RootDir[MAX_PATH];	// ������ ���� �� ����� � ����������
    map< byte, byte >PLIDtoUCID;

    ObjectsInfo addObjects;
    /**< ������� �������� ��� �� ���������� �� ����� */

    ObjectsInfo delObjects;
    /**< ������� �������� ��� �� �������� � ����� */

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
    virtual void InsimMCI( struct IS_MCI* packet ){if (!insim) return;}
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
    virtual void InsimRST( struct IS_RST* packet ){ ReadConfig( packet->Track ); }
    virtual void InsimSMALL( struct IS_SMALL* packet ){}
    virtual void InsimSPX( struct IS_SPX* packet ){}
    virtual void InsimSTA( struct IS_STA* packet ){}
    virtual void InsimTINY( struct IS_TINY* packet ){}
    virtual void InsimTOC( struct IS_TOC* packet ){}
    virtual void InsimVER( struct IS_VER* packet ){}
    virtual void InsimVTN( struct IS_VTN* packet ){}

    virtual void ReadConfig(const char *Track){};


    //MYSQL
    MYSQL       *dbconn;
    MYSQL_RES   *dbres;
    MYSQL_ROW   dbrow;

    bool dbPing(string query);
	DB_ROWS dbSelect( string query );
	bool dbExec( string query );
	bool dbExec( const char *query );

	/** @brief ���������� ������ � ����
	 *
	 * @param string query - ����������� SQL �������
	 * @return unsigned int - ID ��������� ���������� ������, ���� ���� PRIMARY KEY (��������� ������� SELECT LAST_INSERT_ID();)
	 *
	 */
	unsigned int dbInsert( string query );
	unsigned int dbInsert( const char *query );
	bool dbUpdate( string table, DB_ROW fields, pair<string, string> where );

	Json::Value 		config;
	Json::Reader 		configReader;
	Json::StyledWriter 	configWriter;

private:

};

class xString : public string
{
    vector<string> flds;
public:
    xString(char *s) : string(s) { };
    xString(const char *s) : string(s) { };
    xString(string s) : string(s) { };
    xString() : string() { };
    vector<string>& split( const char delim, int rep=0);
};

#endif // RCBASECLASS_H
