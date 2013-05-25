#include "RCBaseClass.h"

RCBaseClass::RCBaseClass()
{
    //ctor
}

RCBaseClass::~RCBaseClass()
{
    //dtor
}

void RCBaseClass::next_packet()
{
    if(!insim)
        return;     //dont work if insim is NULL

    switch (insim->peek_packet())
    {
    case ISP_VER:
        InsimVER( (struct IS_VER*)insim->get_packet() );
        break;

    case ISP_TINY:
        InsimTINY( (struct IS_TINY*)insim->get_packet() );
        break;

    case ISP_SMALL:
        InsimSMALL( (struct IS_SMALL*)insim->get_packet() );
        break;

    case ISP_STA:
        InsimSTA( (struct IS_STA*)insim->get_packet() );
        break;

    case ISP_ISM:
        InsimISM( (struct IS_ISM*)insim->get_packet() );
        break;

    case ISP_MSO:
        InsimMSO( (struct IS_MSO*)insim->get_packet() );
        break;

    case ISP_III:
        InsimIII( (struct IS_III*)insim->get_packet() );
        break;

    case ISP_VTN:
        InsimVTN( (struct IS_VTN*)insim->get_packet() );
        break;

    case ISP_RST:
        InsimRST( (struct IS_RST*)insim->get_packet() );
        break;

    case ISP_NCN:
		InsimNCN(  (struct IS_NCN*)insim->get_packet()  );
        break;

    case ISP_CNL:
        InsimCNL( (struct IS_CNL*)insim->get_packet() );
        break;

    case ISP_CPR:
        InsimCPR( (struct IS_CPR*)insim->get_packet() );
        break;

    case ISP_NPL:
        InsimNPL( (struct IS_NPL*)insim->get_packet() );
        break;

    case ISP_PLP:
        InsimPLP( (struct IS_PLP*)insim->get_packet() );
        break;

    case ISP_PLL:
        InsimPLL( (struct IS_PLL*)insim->get_packet() );
        break;

    case ISP_LAP:
        InsimLAP( (struct IS_LAP*)insim->get_packet() );
        break;

    case ISP_SPX:
        InsimSPX( (struct IS_SPX*)insim->get_packet() );
        break;

    case ISP_PIT:
        InsimPIT( (struct IS_PIT*)insim->get_packet() );
        break;

    case ISP_PSF:
        InsimPSF( (struct IS_PSF*)insim->get_packet() );
        break;

    case ISP_PLA:
        InsimPLA( (struct IS_PLA*)insim->get_packet() );
        break;

    case ISP_PEN:
        InsimPEN( (struct IS_PEN*)insim->get_packet() );
        break;

    case ISP_TOC:
        InsimTOC( (struct IS_TOC*)insim->get_packet() );
        break;

    case ISP_FLG:
        InsimFLG( (struct IS_FLG*)insim->get_packet() );
        break;

    case ISP_CRS:
        InsimCRS( (struct IS_CRS*)insim->get_packet() );
        break;

    case ISP_BFN:
        InsimBFN( (struct IS_BFN*)insim->get_packet() );
        break;

    case ISP_BTC:
        InsimBTC( (struct IS_BTC*)insim->get_packet() );
        break;

    case ISP_BTT:
        InsimBTT( (struct IS_BTT*)insim->get_packet() );
        break;

    case ISP_CON:
        InsimCON( (struct IS_CON*)insim->get_packet() );
        break;

    case ISP_OBH:
        InsimOBH( (struct IS_OBH*)insim->get_packet() );
        break;

    case ISP_AXM:
        InsimAXM( (struct IS_AXM*)insim->get_packet() );
        break;

    case ISP_ACR:
        InsimACR( (struct IS_ACR*)insim->get_packet() );
        break;

	case ISP_HLV:
        InsimHLV( (struct IS_HLV*)insim->get_packet() );
        break;
    }
}

void RCBaseClass::InsimACR( struct IS_ACR* packet ){}
void RCBaseClass::InsimAXM( struct IS_AXM* packet ){}
void RCBaseClass::InsimBFN( struct IS_BFN* packet ){}
void RCBaseClass::InsimBTC( struct IS_BTC* packet ){}
void RCBaseClass::InsimBTT( struct IS_BTT* packet ){}
void RCBaseClass::InsimCNL( struct IS_CNL* packet ){}
void RCBaseClass::InsimCON( struct IS_CON* packet ){}
void RCBaseClass::InsimCPR( struct IS_CPR* packet ){}
void RCBaseClass::InsimCRS( struct IS_CRS* packet ){}
void RCBaseClass::InsimFLG( struct IS_FLG* packet ){}
void RCBaseClass::InsimHLV( struct IS_HLV* packet ){}
void RCBaseClass::InsimIII( struct IS_III* packet ){}
void RCBaseClass::InsimISM( struct IS_ISM* packet ){}
void RCBaseClass::InsimLAP( struct IS_LAP* packet ){}
void RCBaseClass::InsimMSO( struct IS_MSO* packet ){}
void RCBaseClass::InsimNCN( struct IS_NCN* packet ){}
void RCBaseClass::InsimNPL( struct IS_NPL* packet ){}
void RCBaseClass::InsimOBH( struct IS_OBH* packet ){}
void RCBaseClass::InsimPEN( struct IS_PEN* packet ){}
void RCBaseClass::InsimPIT( struct IS_PIT* packet ){}
void RCBaseClass::InsimPLA( struct IS_PLA* packet ){}
void RCBaseClass::InsimPLL( struct IS_PLL* packet ){}
void RCBaseClass::InsimPLP( struct IS_PLP* packet ){}
void RCBaseClass::InsimPSF( struct IS_PSF* packet ){}
void RCBaseClass::InsimREO( struct IS_REO* packet ){}
void RCBaseClass::InsimRST( struct IS_RST* packet ){}
void RCBaseClass::InsimSMALL( struct IS_SMALL* packet ){}
void RCBaseClass::InsimSPX( struct IS_SPX* packet ){}
void RCBaseClass::InsimSTA( struct IS_STA* packet ){}
void RCBaseClass::InsimTINY( struct IS_TINY* packet ){}
void RCBaseClass::InsimTOC( struct IS_TOC* packet ){}
void RCBaseClass::InsimVER( struct IS_VER* packet ){}
void RCBaseClass::InsimVTN( struct IS_VTN* packet ){}

void    RCBaseClass::InsimMCI( struct IS_MCI* packet )
{
    if(!insim)return;/**dont work if insim is NULL**/
}

bool RCBaseClass::Check_Pos(int polySides,int polyX[],int polyY[],float x,float y)
{
    int		i, j=polySides-1 ;
    bool	oddNodes=false;
	try{
		for (i=0; i<polySides; i++)
		{
			if (polyY[i]<y && polyY[j]>=y ||  polyY[j]<y && polyY[i]>=y)
			{
				if (polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i])<x)
				{
					oddNodes=!oddNodes;
				}
			}
			j=i;
		}
	}
	catch(...){
	//tools::log("Check_Pos Exeption\n");
	}
    return oddNodes;
}

int RCBaseClass::Distance (int X, int Y, int X1, int Y1)
{
    return (int)sqrt((pow(X-X1,2))+(pow(Y-Y1,2)));
}

void RCBaseClass::SendMTC (byte UCID,const char* Msg)
{
	IS_MTC *pack = new IS_MTC;
	memset( pack, 0, sizeof( IS_MTC ) );
	pack->Size = sizeof( IS_MTC );
	pack->Type = ISP_MTC;
	pack->UCID = UCID;
	sprintf( pack->Text, "%.127s\0", Msg );
	insim->send_packet( pack );
	delete pack;
}

void RCBaseClass::SendMST (const char* Text)
{
	IS_MST *pack = new IS_MST;
	memset( pack, 0, sizeof( IS_MST));
	pack->Size = sizeof( IS_MST);
	pack->Type = ISP_MST;
	sprintf( pack->Msg, "%.63s\0", Text );
	insim->send_packet( pack );
	delete pack;
}

void RCBaseClass::SendBFN (byte UCID, byte ClickID)
{
    IS_BFN *pack = new IS_BFN;
    memset( pack, 0, sizeof( IS_BFN ) );
    pack->Size = sizeof( IS_BFN );
    pack->Type = ISP_BFN;
    pack->UCID = UCID;
    pack->ClickID = ClickID;
    insim->send_packet( pack );
    delete pack;
}

void RCBaseClass::SendBFNAll ( byte UCID )
{
    IS_BFN *pack = new IS_BFN;
    memset( pack, 0, sizeof( IS_BFN ) );
    pack->Size = sizeof( IS_BFN );
    pack->Type = ISP_BFN;
    pack->UCID = UCID;
    pack->SubT = BFN_CLEAR;
    insim->send_packet( pack );
    delete pack;
}

void RCBaseClass::ButtonInfo(byte UCID, const char* Message)
{
    IS_BTN *pack = new IS_BTN;
    memset( pack, 0, sizeof( IS_BTN ) );
    pack->Size = sizeof( IS_BTN );
    pack->Type = ISP_BTN;
    pack->ReqI = 1;
    pack->UCID = UCID;
    pack->ClickID = 211;
    pack->BStyle = 16;
    pack->L = 70;
    pack->T = 9;
    pack->W = 60;
    pack->H = 4;
    strcpy( pack->Text, Message );
    insim->send_packet( pack );
    delete pack;
}

void RCBaseClass::ClearButtonInfo(byte UCID)
{
	SendBFN( UCID, 211);
}

void RCBaseClass::SendButton(byte ReqI, byte UCID, byte ClickID, byte L, byte T, byte W, byte H, byte BStyle, const char * Text )
{
	IS_BTN *pack = new IS_BTN;
    memset( pack, 0, sizeof( IS_BTN ) );
    pack->Size = sizeof( IS_BTN );
    pack->Type = ISP_BTN;
    pack->ReqI = ReqI;
    pack->UCID = UCID;
    pack->Inst = 0;
    pack->BStyle = BStyle;
    pack->TypeIn = 0;
    pack->ClickID = ClickID;
    pack->L = L;
    pack->T = T;
    pack->W = W;
    pack->H = H;
    strcpy( pack->Text ,Text );
    insim->send_packet( pack );
    delete pack;
}


// split: receives a char delimiter; returns a vector of strings
// By default ignores repeated delimiters, unless argument rep == 1.
vector<string>& SplitString::split( const char delim, int rep) {
    if (!flds.empty()) flds.clear();  // empty vector if necessary
    string work = data();
    string buf = "";
    int i = 0;
    while (i < work.length()) {
        if (work[i] != delim)
            buf += work[i];
        else if (rep == 1) {
            flds.push_back(buf);
            buf = "";
        } else if (buf.length() > 0) {
            flds.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        flds.push_back(buf);
    return flds;
}
