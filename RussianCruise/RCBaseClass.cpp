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
        insim_ver( (struct IS_VER*)insim->get_packet() );
        break;

    case ISP_TINY:
        insim_tiny( (struct IS_TINY*)insim->get_packet() );
        break;

    case ISP_SMALL:
        insim_small( (struct IS_SMALL*)insim->get_packet() );
        break;

    case ISP_STA:
        insim_sta( (struct IS_STA*)insim->get_packet() );
        break;

    case ISP_ISM:
        insim_ism( (struct IS_ISM*)insim->get_packet() );
        break;

    case ISP_MSO:
        insim_mso( (struct IS_MSO*)insim->get_packet() );
        break;

    case ISP_III:
        insim_iii( (struct IS_III*)insim->get_packet() );
        break;

    case ISP_VTN:
        insim_vtn( (struct IS_VTN*)insim->get_packet() );
        break;

    case ISP_RST:
        insim_rst( (struct IS_RST*)insim->get_packet() );
        break;

    case ISP_NCN:
		insim_ncn(  (struct IS_NCN*)insim->get_packet()  );
        break;

    case ISP_CNL:
        insim_cnl( (struct IS_CNL*)insim->get_packet() );
        break;

    case ISP_CPR:
        insim_cpr( (struct IS_CPR*)insim->get_packet() );
        break;

    case ISP_NPL:
        insim_npl( (struct IS_NPL*)insim->get_packet() );
        break;

    case ISP_PLP:
        insim_plp( (struct IS_PLP*)insim->get_packet() );
        break;

    case ISP_PLL:
        insim_pll( (struct IS_PLL*)insim->get_packet() );
        break;


    case ISP_LAP:
        insim_lap( (struct IS_LAP*)insim->get_packet() );
        break;

    case ISP_SPX:
        insim_spx( (struct IS_SPX*)insim->get_packet() );
        break;

    case ISP_PIT:
        insim_pit( (struct IS_PIT*)insim->get_packet() );
        break;

    case ISP_PSF:
        insim_psf( (struct IS_PSF*)insim->get_packet() );
        break;

    case ISP_PLA:
        insim_pla( (struct IS_PLA*)insim->get_packet() );
        break;

    case ISP_PEN:
        insim_pen( (struct IS_PEN*)insim->get_packet() );
        break;

    case ISP_TOC:
        insim_toc( (struct IS_TOC*)insim->get_packet() );
        break;

    case ISP_FLG:
        insim_flg( (struct IS_FLG*)insim->get_packet() );
        break;

    case ISP_CRS:
        insim_crs( (struct IS_CRS*)insim->get_packet() );
        break;

    case ISP_BFN:
        insim_bfn( (struct IS_BFN*)insim->get_packet() );
        break;

    case ISP_BTC:
        insim_btc( (struct IS_BTC*)insim->get_packet() );
        break;

    case ISP_BTT:
        insim_btt( (struct IS_BTT*)insim->get_packet() );
        break;

    case ISP_CON:
        insim_con( (struct IS_CON*)insim->get_packet() );
        break;

    case ISP_OBH:
        insim_obh( (struct IS_OBH*)insim->get_packet() );
        break;

    case ISP_AXM:
        insim_axm( (struct IS_AXM*)insim->get_packet() );
        break;

    case ISP_ACR:
        insim_acr( (struct IS_ACR*)insim->get_packet() );
        break;



    }
}

void RCBaseClass::insim_ver( struct IS_VER* packet ){}
void RCBaseClass::insim_tiny( struct IS_TINY* packet ){}
void RCBaseClass::insim_small( struct IS_SMALL* packet ){}
void RCBaseClass::insim_sta( struct IS_STA* packet ){}
void RCBaseClass::insim_ism( struct IS_ISM* packet ){}
void RCBaseClass::insim_iii( struct IS_III* packet ){}
void RCBaseClass::insim_vtn( struct IS_VTN* packet ){}
void RCBaseClass::insim_rst( struct IS_RST* packet ){}
void RCBaseClass::insim_lap( struct IS_LAP* packet ){}
void RCBaseClass::insim_spx( struct IS_SPX* packet ){}
void RCBaseClass::insim_pit( struct IS_PIT* packet ){}
void RCBaseClass::insim_psf( struct IS_PSF* packet ){}
void RCBaseClass::insim_pla( struct IS_PLA* packet ){}
void RCBaseClass::insim_pen( struct IS_PEN* packet ){}
void RCBaseClass::insim_toc( struct IS_TOC* packet ){}
void RCBaseClass::insim_flg( struct IS_FLG* packet ){}
void RCBaseClass::insim_reo( struct IS_REO* packet ){}
void RCBaseClass::insim_crs( struct IS_CRS* packet ){}
void RCBaseClass::insim_bfn( struct IS_BFN* packet ){}
void RCBaseClass::insim_btc( struct IS_BTC* packet ){}
void RCBaseClass::insim_btt( struct IS_BTT* packet ){}
void RCBaseClass::insim_axm( struct IS_AXM* packet ){}
void RCBaseClass::insim_acr( struct IS_ACR* packet ){}
void RCBaseClass::insim_ncn( struct IS_NCN* packet ){}
void RCBaseClass::insim_npl( struct IS_NPL* packet ){}
void RCBaseClass::insim_plp( struct IS_PLP* packet ){}
void RCBaseClass::insim_pll( struct IS_PLL* packet ){}
void RCBaseClass::insim_cnl( struct IS_CNL* packet ){}
void RCBaseClass::insim_cpr( struct IS_CPR* packet ){}
void RCBaseClass::insim_mso( struct IS_MSO* packet ){}
void RCBaseClass::insim_con( struct IS_CON* packet ){}
void RCBaseClass::insim_obh( struct IS_OBH* packet ){}

void    RCBaseClass::insim_mci( struct IS_MCI* packet )
{
    if(!insim)return;/**dont work if insim is NULL**/
}

bool RCBaseClass::Check_Pos(int polySides,int polyX[],int polyY[],int x,int y)
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



void RCBaseClass::send_mtc (byte UCID,const char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strncpy(pack_mtc.Text, Msg,strlen(Msg));
    insim->send_packet(&pack_mtc);
}

void RCBaseClass::send_mst (const char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst);
}

void RCBaseClass::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack);
}

void RCBaseClass::send_bfn_all ( byte UCID )
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.SubT = BFN_CLEAR;
    insim->send_packet(&pack);
}

void RCBaseClass::btn_information(byte UCID, const char* Message)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
    pack.ClickID = 211;
    pack.BStyle = 32;
    pack.L = 85;
    pack.T = 15;
    pack.W = 50;
    pack.H = 4;

    strcpy(pack.Text,Message);

    insim->send_packet(&pack);

}

void RCBaseClass::btn_information_clear(byte UCID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = 211;
    insim->send_packet(&pack);
}
