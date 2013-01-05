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
        insim_ver();
        break;

    case ISP_TINY:
        insim_tiny();
        break;

    case ISP_SMALL:
        insim_small();
        break;

    case ISP_STA:
        insim_sta();
        break;

    case ISP_ISM:
        insim_ism();
        break;

    case ISP_MSO:
        insim_mso();
        break;

    case ISP_III:
        insim_iii();
        break;

    case ISP_VTN:
        insim_vtn();
        break;

    case ISP_RST:
        insim_rst();
        break;

    case ISP_NCN:
        insim_ncn();
        break;

    case ISP_CNL:
        insim_cnl();
        break;

    case ISP_CPR:
        insim_crp();
        break;

    case ISP_NPL:
        insim_npl();
        break;

    case ISP_PLP:
        insim_plp();
        break;

    case ISP_PLL:
        insim_pll();
        break;


    case ISP_LAP:
        insim_lap();
        break;

    case ISP_SPX:
        insim_spx();
        break;

    case ISP_PIT:
        insim_pit();
        break;

    case ISP_PSF:
        insim_psf();
        break;

    case ISP_PLA:
        insim_pla();
        break;

    case ISP_PEN:
        insim_pen();
        break;

    case ISP_TOC:
        insim_toc();
        break;

    case ISP_FLG:
        insim_flg();
        break;

    case ISP_CRS:
        insim_crs();
        break;

    case ISP_BFN:
        insim_bfn();
        break;

    case ISP_BTC:
        insim_btc();
        break;

    case ISP_BTT:
        insim_btt();
        break;

    case ISP_CON:
        insim_con();
        break;

    case ISP_OBH:
        insim_obh();
        break;

    case ISP_AXM:
        insim_axm();
        break;

    case ISP_ACR:
        insim_acr();
        break;



    }
}

void    RCBaseClass::insim_ver() {}
void    RCBaseClass::insim_tiny() {}
void    RCBaseClass::insim_small() {}
void    RCBaseClass::insim_sta() {}
void    RCBaseClass::insim_ism() {}
void    RCBaseClass::insim_iii() {}
void    RCBaseClass::insim_vtn() {}
void    RCBaseClass::insim_rst() {}
void    RCBaseClass::insim_lap() {}
void    RCBaseClass::insim_spx() {}
void    RCBaseClass::insim_pit() {}
void    RCBaseClass::insim_psf() {}
void    RCBaseClass::insim_pla() {}
void    RCBaseClass::insim_pen() {}
void    RCBaseClass::insim_toc() {}
void    RCBaseClass::insim_flg() {}
void    RCBaseClass::insim_reo() {}
void    RCBaseClass::insim_crs() {}
void    RCBaseClass::insim_bfn() {}
void    RCBaseClass::insim_btc() {}
void    RCBaseClass::insim_btt() {}
void    RCBaseClass::insim_axm() {}
void    RCBaseClass::insim_acr() {}
void    RCBaseClass::insim_ncn() {}
void    RCBaseClass::insim_npl() {}
void    RCBaseClass::insim_plp() {}
void    RCBaseClass::insim_pll() {}
void    RCBaseClass::insim_cnl() {}
void    RCBaseClass::insim_crp() {}
void    RCBaseClass::insim_mso() {}
void    RCBaseClass::insim_con() {}
void    RCBaseClass::insim_obh() {}
void    RCBaseClass::insim_mci()
{
    if(!insim)return;/**dont work if insim is NULL**/
}

bool RCBaseClass::Check_Pos(int polySides,int polyX[],int polyY[],float x,float y)
{

    int      i, j=polySides-1 ;
    bool  oddNodes=false     ;
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
