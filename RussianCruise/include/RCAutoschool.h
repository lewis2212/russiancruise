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

		bool InsimNCN( struct IS_NCN* packet );   // ����� ����� ����� �� ������
		bool InsimNPL( struct IS_NPL* packet );   // ����� ����� �� ������
		bool InsimPLP( struct IS_PLP* packet );   // ����� ���� � �����
		bool InsimPLL( struct IS_PLL* packet );   // ����� ���� � �������
		bool InsimCNL( struct IS_CNL* packet );   // ����� ���� � �������
		bool InsimCPR( struct IS_CPR* packet );   // ����� ��������������
		bool InsimMSO( struct IS_MSO* packet );   // ����� �������� ���������
		bool InsimCON( struct IS_CON* packet );
		bool InsimOBH( struct IS_OBH* packet );
		bool InsimHLV( struct IS_HLV* packet );
		bool InsimAXM( struct IS_AXM* packet );

		void ShowPanel(byte UCID);

	public:
		RCAutoschool(const char *dir);
		~RCAutoschool();

		void init(MYSQL *rcMaindbConn, CInsim *insim, RCMessage *msg);
		bool ReadConfig(const char* Track);

		bool InsimMCI( struct IS_MCI* packet );   // ����� � ������� � ����������� � �.�.
};

#endif // RCAUTOSCHOOL_H
