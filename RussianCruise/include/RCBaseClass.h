using namespace std;
#ifndef RCBASECLASS_H
#define RCBASECLASS_H

#include "CInsim.h"

#include <CarInfo.h>

#include "RCButtonClickID.h"

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <time.h>       // для работы с временем и рандомом

#include <list>
#include <exception>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>

#include <direct.h>

#include <mysql/mysql.h>

#include <json/json.h>

//#include "tools.h"
#define MAX_PATH	260

/**< переменная - очередь игровых объектов */
typedef queue< ObjectInfo > ObjectsInfo;

/**< переменная - асоциативный массив вида "название поля" -> "значение поля" */
typedef map< string, string > DB_ROW;

/**< переменная - массив состоящий из элементов DB_ROW */
typedef list<DB_ROW> DB_ROWS;

struct GlobalPlayer
{
    CarInfo Info;
    string	UName;              // Username
    string	PName;              // Player name
    string	CName;             // Car Name

    bool DontSave;
};

class RCBaseClass
{
public:
    RCBaseClass();
    ~RCBaseClass();

    void         next_packet();   						// Функция переборки типа пакета
    virtual void InsimMCI( struct IS_MCI* packet );     	// Пакет с данными о координатах и т.д.

    void SendBFN(byte UCID, byte ClickID);
    void SendBFNAll ( byte UCID );

    void SendMST (const char* Text);
    void SendMST (string Text){ SendMST( Text.c_str() ); };

    void SendMTC (byte UCID, string Msg);

    void SendButton (byte ReqI, byte UCID, byte ClickID, byte Left, byte Top, byte Width, byte Height, byte BStyle, const char * Text, byte TypeIn = 0 );
    void SendButton (byte ReqI, byte UCID, byte ClickID, byte Left, byte Top, byte Width, byte Height, byte BStyle, string Text, byte TypeIn = 0 );

    void ButtonInfo (byte UCID, const char* Message);
    void ClearButtonInfo (byte UCID);

    static bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);
    static int Distance (int X, int Y, int X1, int Y1);

    static void CCText(string Text);

    /** @brief Получение причины отключения пользователя с сервера
     *
     * @param  byte Reason - Числовой индетификатор кода
     * @return static const char* - Буквенное значение кода
     *
     */
    static const char* GetReason(byte Reason);

    /** @brief Добавление объектов на карту.
     *			Перед вызовом метода, добавляемые объекты необходимо добавить в очередь addObjects,
     *			используя метод addObjects.push( ObjectInfo object )
     *
     * @return void - ничего
     *
     */
    void AddObjects();

    /** @brief Удаление объектов на карте.
     *			Перед вызовом метода, удаляемые объекты необходимо добавить в очередь delObjects,
     *			используя метод delObjects.push( ObjectInfo object )
     *
     * @return void - ничего
     *
     */
	void DelObjects();

	/** @brief Добавление объекта на карту.
	 *
     * @param  ObjectInfo *object - указатель на структуру объекта
     *
     * @return void - ничего
     *
     */
	void AddObject( ObjectInfo *object );

	/** @brief Удаление объекта с карты
	 *
     * @param  ObjectInfo *object - указатель на структуру объекта
     *
     * @return void - ничего
     *
     */
	void DelObject( ObjectInfo *object );

	static string ToString (int i);
	static string ToString (byte b);
	static string ToString (bool b);
	static string ToString (float f);
	static string ToString (double d);
	static string ToString (const char *c);

	inline string trim(const std::string &s);

protected:
    CInsim      *insim;         	// Переменная-указатель на класс CInsim
    char        RootDir[MAX_PATH]; 	// Полный путь до папки с программой
    map< byte, byte >PLIDtoUCID;


    ObjectsInfo addObjects;
    /**< очередь объектов для их добавления на карту */

    ObjectsInfo delObjects;
    /**< очередь объектов для их удаления с карты */

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
    virtual void InsimRST( struct IS_RST* packet ){ readconfig( packet->Track ); }
    virtual void InsimSMALL( struct IS_SMALL* packet ){}
    virtual void InsimSPX( struct IS_SPX* packet ){}
    virtual void InsimSTA( struct IS_STA* packet ){}
    virtual void InsimTINY( struct IS_TINY* packet ){}
    virtual void InsimTOC( struct IS_TOC* packet ){}
    virtual void InsimVER( struct IS_VER* packet ){}
    virtual void InsimVTN( struct IS_VTN* packet ){}

    virtual void readconfig(const char *Track){};
    virtual void Save(byte UCID){};


    //MYSQL
    MYSQL       *dbconn;
    MYSQL_RES   *dbres;
    MYSQL_ROW   dbrow;

    /** @brief Выборка данных из базы данных
	 *
	 * @param string query - стандартная SQL команда
	 * @return DB_ROWS - массив элементов DB_ROW
	 *
	 */
	DB_ROWS dbSelect( string query );

	/** @brief Выполнение SQL запроса, который не возвращает данные (обновление строк, служебные запросы)
	 *
	 * @param string query - стандартная SQL команда
	 * @return bool - результат функции mysql_query()
	 *
	 */
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

	Json::Value 		config;
	Json::Reader 		configReader;
	Json::StyledWriter 	configWriter;

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
