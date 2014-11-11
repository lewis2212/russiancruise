#ifndef _RC_AUTOSCHOOL_H
#define _RC_AUTOSCHOOL_H

#include "RCBaseClass.h"
#include "RCMessage.h"

struct SchoolPlayer:public GlobalPlayer
{
	struct	CompCar	Info;
	string	UName;
	string	PName;
	string	CName;

	string	Lesson = "shicane";

	bool	OnPlace = false;
	bool 	Started = false;
	bool	Finished = false;
	unsigned int		CheckPoint = 0;


	float			Rate = 0;           // ������� ������
	float			TempRate = 0;       // ��������� ������� �� ����� ���������� ����������
	unsigned int	LastTaskTime = 0;   // ����� ���������� ���������� ������
};

class RCAutoschool : public RCBaseClass
{
	private:
		map <byte, SchoolPlayer>players;          // ��������� �������

		RCMessage * msg;

		void InsimAXM( struct IS_AXM* packet );
		void InsimCNL( struct IS_CNL* packet );   // ����� ���� � �������
		void InsimCON( struct IS_CON* packet );
		void InsimCPR( struct IS_CPR* packet );   // ����� ��������������
		void InsimHLV( struct IS_HLV* packet );
		void InsimMCI( struct IS_MCI* packet );   // ����� � ������� � ����������� � �.�.
		void InsimMSO( struct IS_MSO* packet );   // ����� �������� ���������
		void InsimNCN( struct IS_NCN* packet );   // ����� ����� ����� �� ������
		void InsimNPL( struct IS_NPL* packet );   // ����� ����� �� ������
		void InsimOBH( struct IS_OBH* packet );
		void InsimPLL( struct IS_PLL* packet );   // ����� ���� � �������
		void InsimPLP( struct IS_PLP* packet );   // ����� ���� � �����

		void ShowPanel(byte UCID);

	public:
		RCAutoschool(const char *dir);
		~RCAutoschool();

		void init(DBMySQL *db, CInsim *insim, RCMessage *msg);
		void ReadConfig(const char* Track);

};

#endif // RCAUTOSCHOOL_H
