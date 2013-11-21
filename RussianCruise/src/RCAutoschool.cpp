#include "RCAutoschool.h"

RCAutoschool::RCAutoschool(const char *dir)
{
	strcpy(RootDir, dir);
	players.clear();
}

RCAutoschool::~RCAutoschool()
{
	players.clear();
}

void
RCAutoschool::init(MYSQL *rcMaindbConn, CInsim *insim, RCMessage *msg)
{
	dbconn = rcMaindbConn;

	this->insim = insim;
	this->msg = msg;
}

void
RCAutoschool::readconfig(const char* Track)
{

}

void
RCAutoschool::InsimNCN( struct IS_NCN* packet )
{
	 if (packet->UCID == 0)
    {
        return;
    }

    players[packet->UCID].UName = packet->UName;
    players[packet->UCID].PName = packet->PName;
}

void
RCAutoschool::InsimNPL( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID;
    players[packet->UCID].CName = packet->CName;
}

void
RCAutoschool::InsimPLP( struct IS_PLP* packet )
{
}

void
RCAutoschool::InsimPLL( struct IS_PLL* packet )
{
	PLIDtoUCID.erase( packet->PLID );
}

void
RCAutoschool::InsimCNL( struct IS_CNL* packet )
{
	players.erase( packet->UCID );
}

void
RCAutoschool::InsimCPR( struct IS_CPR* packet )
{
	players[packet->UCID].PName = packet->PName;
}

void
RCAutoschool::InsimMSO( struct IS_MSO* packet )
{
	if (packet->UCID == 0)
    {
        return;
    }

    byte UCID = packet->UCID;

    string msg = packet->Msg + packet->TextStart;

    if( msg.find( "!erase" ) == 0 )
	{
		SendMST(msg.c_str());
	}
}

void
RCAutoschool::InsimCON( struct IS_CON* packet )
{
	byte UCIDA = PLIDtoUCID[ packet->A.PLID ];
    byte UCIDB = PLIDtoUCID[ packet->B.PLID ];
}

void
RCAutoschool::InsimOBH( struct IS_OBH* packet )
{
	 byte UCID = PLIDtoUCID[ packet->PLID ];
}

void
RCAutoschool::InsimHLV( struct IS_HLV* packet )
{
	 byte UCID = PLIDtoUCID[ packet->PLID ];

    /** столкновение со стеной **/
    if (packet->HLVC==1)
    {

    }
}

void
RCAutoschool::InsimAXM( struct IS_AXM* packet )
{
	if( packet->UCID == 0 )
		return;

	CCText("^3RCAutoschool^1::^2InsimAXM");

	cout << NumToString( packet->NumO ) << endl;
	cout << NumToString( packet->UCID ) << endl;
	cout << NumToString( packet->PMOAction ) << endl;
	cout << NumToString( packet->PMOFlags ) << endl;

	for( int i = 0; i < packet->NumO; ++i)
	{
		cout << "X: " << packet->Info[i].X << "\t" << "Y: " << packet->Info[i].Y << endl;
	}
}

void
RCAutoschool::InsimMCI ( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]
    {
        byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];
    }
}
