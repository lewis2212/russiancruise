#include "RCAutoschool.h"

RCAutoschool::RCAutoschool()
{
	players.clear();
}

RCAutoschool::~RCAutoschool()
{
	players.clear();
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
}

void
RCAutoschool::InsimMCI ( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]
    {
        byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];
    }
}
