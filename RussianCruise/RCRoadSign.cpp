#include "RCRoadSign.h"

RCRoadSign::RCRoadSign()
{
	//ctor
}

RCRoadSign::~RCRoadSign()
{
	//dtor
}

int RCRoadSign::Init(const char *dir,void *CInSim, void *Message, void *RCDLic){}
void RCRoadSign::ReadConfig(const char *Track){}

void RCRoadSign::InsimNCN( struct IS_NCN* packet ){}
void RCRoadSign::InsimNPL( struct IS_NPL* packet ){}
void RCRoadSign::InsimPLP( struct IS_PLP* packet ){}
void RCRoadSign::InsimPLL( struct IS_PLL* packet ){}
void RCRoadSign::InsimCNL( struct IS_CNL* packet ){}
void RCRoadSign::InsimCPR( struct IS_CPR* packet ){}
void RCRoadSign::InsimMSO( struct IS_MSO* packet ){}
