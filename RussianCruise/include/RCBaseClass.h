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
#include <time.h>       // для работы с временем и рандомом

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

/**< переменная - очередь игровых объектов */
typedef queue< ObjectInfo > ObjectsInfo;

/**< переменная - асоциативный массив вида "название поля" -> "значение поля" */
typedef map< string, string > DB_ROW;

/**< переменная - массив состоящий из элементов DB_ROW */
typedef vector<DB_ROW> DB_ROWS;

struct GlobalPlayer
{
    CarInfo Info;
    string	UName;              // Username
    string	PName;              // Player name
    string	CName;             // Car Name

    bool Loaded = false;
};

class RCBaseClass
{
public:
    RCBaseClass();
    ~RCBaseClass();

    string ClassName = "RCBaseClass";

    bool		 next_packet();   						// Функция переборки типа пакета
    virtual bool InsimMCI( struct IS_MCI* packet );     	// Пакет с данными о координатах и т.д.
	virtual bool Save(byte UCID){return true;};
    virtual bool Event(){return true;};

    void ButtonInfo (byte UCID, const char* Message);
    void ClearButtonInfo (byte UCID);
    void ShowPanel(byte UCID, string Caption, list<string>Text);

    static bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);
    static int Distance (int X, int Y, int X1, int Y1);
    static void CCText(string Text, bool useTime = true);

    static const char* GetReason(byte Reason);


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
    CInsim      *insim;				// Переменная-указатель на класс CInsim
    char        RootDir[MAX_PATH];	// Полный путь до папки с программой
    map< byte, byte >PLIDtoUCID;

    ObjectsInfo addObjects;
    /**< очередь объектов для их добавления на карту */

    ObjectsInfo delObjects;
    /**< очередь объектов для их удаления с карты */

    virtual bool InsimACR( struct IS_ACR* packet ){return true;}
    virtual bool InsimAXM( struct IS_AXM* packet ){return true;}
    virtual bool InsimBFN( struct IS_BFN* packet ){return true;}
    virtual bool InsimBTC( struct IS_BTC* packet ){return true;}
    virtual bool InsimBTT( struct IS_BTT* packet ){return true;}
    virtual bool InsimCNL( struct IS_CNL* packet ){return true;}
    virtual bool InsimCON( struct IS_CON* packet ){return true;}
    virtual bool InsimCPR( struct IS_CPR* packet ){return true;}
    virtual bool InsimCRS( struct IS_CRS* packet ){return true;}
    virtual bool InsimFLG( struct IS_FLG* packet ){return true;}
    virtual bool InsimHLV( struct IS_HLV* packet ){return true;}
    virtual bool InsimIII( struct IS_III* packet ){return true;}
    virtual bool InsimISM( struct IS_ISM* packet ){return true;}
    virtual bool InsimLAP( struct IS_LAP* packet ){return true;}
    virtual bool InsimMSO( struct IS_MSO* packet ){return true;}
    virtual bool InsimNCN( struct IS_NCN* packet ){return true;}
    virtual bool InsimNPL( struct IS_NPL* packet ){return true;}
    virtual bool InsimOBH( struct IS_OBH* packet ){return true;}
    virtual bool InsimPEN( struct IS_PEN* packet ){return true;}
    virtual bool InsimPIT( struct IS_PIT* packet ){return true;}
    virtual bool InsimPLA( struct IS_PLA* packet ){return true;}
    virtual bool InsimPLL( struct IS_PLL* packet ){return true;}
    virtual bool InsimPLP( struct IS_PLP* packet ){return true;}
    virtual bool InsimPSF( struct IS_PSF* packet ){return true;}
    virtual bool InsimREO( struct IS_REO* packet ){return true;}
    virtual bool InsimRST( struct IS_RST* packet ){ return ReadConfig( packet->Track ); }
    virtual bool InsimSMALL( struct IS_SMALL* packet ){return true;}
    virtual bool InsimSPX( struct IS_SPX* packet ){return true;}
    virtual bool InsimSTA( struct IS_STA* packet ){return true;}
    virtual bool InsimTINY( struct IS_TINY* packet ){return true;}
    virtual bool InsimTOC( struct IS_TOC* packet ){return true;}
    virtual bool InsimVER( struct IS_VER* packet ){return true;}
    virtual bool InsimVTN( struct IS_VTN* packet ){return true;}

    virtual bool ReadConfig(const char *Track){return true;};


    //MYSQL
    MYSQL       *dbconn;
    MYSQL_RES   *dbres;
    MYSQL_ROW   dbrow;

    bool dbPing(string query);
	DB_ROWS dbSelect( string query );
	bool dbExec( string query );
	bool dbExec( const char *query );

	/** @brief Добавление данных в базу
	 *
	 * @param string query - стандартная SQL команда
	 * @return unsigned int - ID последней добавленой строки, если есть PRIMARY KEY (результат запроса SELECT LAST_INSERT_ID();)
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
