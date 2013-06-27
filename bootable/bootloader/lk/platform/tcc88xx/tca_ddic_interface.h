#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************************************
*   Internal Function interface
***************************************************************************************************/
enum{
    NTSCPALSELECT,
    LVDSSELECT,
    LVDSRESET,
    LVDSENABLE,
    LVDSCLOCKCONTROL,
    LASTFUNCTION
};

/*
enum{
    DDICFG_NTSCPALSELECT,
    DDICFG_LVDSSELECT,
    DDICFG_LVDSRESET,
    DDICFG_LVDSENABLE,
    DDICFG_CLOCKCONTROL,
    DDICFG_INTERFACE,
    DDICFG_SETDDICACHECONTROL,
    DDICFG_SETDDICACHECONFIG,
    DDICFG_SETDEFAULT
};
*/



/**************************************************************************************************
*   CACHE Selection for Cache Control
***************************************************************************************************/
#define DDICACHE_DDIC0_ENABLE_CH0    0x00000001
#define DDICACHE_DDIC0_ENABLE_CH1    0x00000002
#define DDICACHE_DDIC0_ENABLE_CH2    0x00000004
#define DDICACHE_DDIC0_ENABLE_CH3    0x00000008
#define DDICACHE_DDIC0_ENABLE_CH4    0x00000010
#define DDICACHE_DDIC0_ENABLE_CH5    0x00000020

#define DDICACHE_DDIC1_ENABLE_CH0    0x00010000
#define DDICACHE_DDIC1_ENABLE_CH1    0x00020000
#define DDICACHE_DDIC1_ENABLE_CH2    0x00040000
#define DDICACHE_DDIC1_ENABLE_CH3    0x00080000
#define DDICACHE_DDIC1_ENABLE_CH4    0x00100000
#define DDICACHE_DDIC1_ENABLE_CH5    0x00200000



/**************************************************************************************************
*   CACHE Selection for Cache Configuration
***************************************************************************************************/
// FOR LCDC0 Cache
enum{
    DDICACHE_LCD0_DMA_0_0,
    DDICACHE_LCD0_DMA_0_1, 
    DDICACHE_LCD0_DMA_0_2, 
    DDICACHE_LCD0_DMA_1, 
    DDICACHE_LCD0_DMA_2, 
    DDICACHE_MSCL0_DMA_0, 
    DDICACHE_MSCL0_DMA_1, 
    DDICACHE_MSCL0_DMA_2, 
    DDICACHE_VIQE_DMA_0_0, 
    DDICACHE_VIQE_DMA_0_1, 
    DDICACHE_VIQE_DMA_0_2, 
    DDICACHE_LCD0_DMA_3,
    DDOCACHE_LCD1_DMA_3,
};
// FOR LCDC1 Cache
enum{
    DDICACHE_LCD1_DMA_0_0,
    DDICACHE_LCD1_DMA_0_1,
    DDICACHE_LCD1_DMA_0_2,
    DDICACHE_LCD1_DMA_1,
    DDICACHE_LCD1_DMA_2,
    DDICACHE_MSCL1_DMA_0,
    DDICACHE_MSCL1_DMA_1,
    DDICACHE_MSCL1_DMA_2,
    DDICACHE_VIQE_DMA_1_0,
    DDICACHE_VIQE_DMA_1_1,
    DDICACHE_VIQE_DMA_1_2,
    DDICACHE_VIQE_DMA_2_0,
    DDICACHE_VIQE_DMA_2_1,
    DDICACHE_VIQE_DMA_2_2,
};

enum{
    DDICACHE_SEL26,
    DDICACHE_SEL27,
    DDICACHE_SEL28,
    DDICACHE_SEL29,
    DDICACHE_SEL30,
    DDICACHE_SEL31
};

enum{
    DDICACHE_SELCH0,
    DDICACHE_SELCH1,
    DDICACHE_SELCH2,
    DDICACHE_SELCH3,
    DDICACHE_SELCH4,
    DDICACHE_SELCH5,
    DDICACHE_SELCH6,
    DDICACHE_SELCH7,
    DDICACHE_SELCH8,
    DDICACHE_SELCH9,
    DDICACHE_SELCH10,
    DDICACHE_SELCH11,
    DDICACHE_SELCH12,
    DDICACHE_SELCH13,       
};
/**************************************************************************************************
*   HDMI_CTRL
***************************************************************************************************/
enum{
    HDMICTRL_HDMIRESET,
    HDMICTRL_SPDIFRESET,
    HDMICTRL_TMDSRESET,
    HDMICTRL_NOTUSED
};

/**************************************************************************************************
    DDI_BUS PWDN
***************************************************************************************************/
enum{
    DDIBUS_PWDN,
    DDIBUS_SWRESET
};
enum{
    DDICFG_PWDNSWRESET_CIF, // RESERVED for TCC9300
    DDICFG_PWDNSWRESET_VIQE,
    DDICFG_PWDNSWRESET_LCDC0,
    DDICFG_PWDNSWRESET_LCDC1,
    DDICFG_PWDNSWRESET_LCDSI0,
    DDICFG_PWDNSWRESET_MSCL0,
    DDICFG_PWDNSWRESET_MSCL1,
    DDICFG_PWDNSWRESET_DDIC,
    DDICFG_PWDNSWRESET_HDMI,
    DDICFG_PWDNSWRESET_LCDSI1,
};

/**************************************************************************************************
***************************************************************************************************/
typedef union _ntscpalsel{
    unsigned int ntscpal_sel;
    struct{
        unsigned int sel:1;
        unsigned int reserved:31;
    }B;
}UNTSCPALSEL;

typedef union _lvdsctrl{
    unsigned int lvds_ctrl;
    struct{
        unsigned int sel:1;
        unsigned int rst:1;
        unsigned int en:1;
        unsigned int reserved3:1;
        unsigned int clkcon:3;
        unsigned int reserved7:1;
        unsigned int acon:3;
        unsigned int reserved11:1;
        unsigned int bcon:3;
        unsigned int reserved15:1;
        unsigned int ccon:3;
        unsigned int reserved19:1;
        unsigned int dcon:3;
        unsigned int reserved23:1;
        unsigned int econ:3;
        unsigned int reserved:5;
    }B;
}ULVDSCTRL;

typedef union _lvdstxosel{
    unsigned int lvds_txo_sel;
    struct{
        unsigned int sel0:8;
        unsigned int sel1:8;
        unsigned int sel2:8;
        unsigned int sel3:8;
    }B;
}ULVDSTXOSEL;


typedef union _hdmictrl{
    unsigned int hdmictrl;
    struct{
        unsigned int hdmireset:1;
        unsigned int spdifreset:1;
        unsigned int tmdsreset:1;
        unsigned int notused:1;
        unsigned int reserved1:10;
        unsigned int en:1;
        unsigned int sel:1;
        unsigned int reserved2:16;
    }B;
}UHDMICTRL;

typedef union _pwdn{
    unsigned int pwdn;
    struct{
        unsigned int cif:1; // Reserved for TCC8800
        unsigned int viqe:1;
        unsigned int lcdc0:1;
        unsigned int lcdc1:1;
        unsigned int lcdsi0:1;
        unsigned int mscl0:1;
        unsigned int mscl1:1;
        unsigned int ddic:1;
        unsigned int hdmi:1;
        unsigned int lcdsi1:1;
        unsigned int reserved:22;
    }B;
}UPWDN;

typedef union _swreset{
    unsigned int swreset;
    struct{
        unsigned int cif:1; // Reserved for TCC8800
        unsigned int viqe:1;
        unsigned int lcdc0:1;
        unsigned int lcdc1:1;
        unsigned int lcdsi0:1;
        unsigned int mscl0:1;
        unsigned int mscl1:1;
        unsigned int ddic:1;
        unsigned int hdmi:1;
        unsigned int lcdsi1:1;
        unsigned int reserved:22;
    }B;
}USWRESET;

typedef union _onthefly{
    unsigned int onthefly;
    struct{
        unsigned int viqe:2;
        unsigned int sc0:2;
        unsigned int sc1:2;
        unsigned int reserved:26;
    }B;
}UONTHEFLY;

typedef union _hdmiaes{
    unsigned int hdmiaes;
    struct{
        unsigned int vld:1;
        unsigned int reserved:31;
    }B;
}UHDMIAES;

typedef struct{
    unsigned int aeskey0;
    unsigned int aeskey1;
    unsigned int aeskeyhw0;
    unsigned int aeskeyhw1;
    unsigned int aeskeyhw2;    
}SHDMIAESDATA;

typedef struct _ddiconfig{
    
    UNTSCPALSEL     uNTSCPALSEL;
    ULVDSCTRL       uLVDSCTRL;      // NOT used on TCC9300
    ULVDSTXOSEL     uLVDSTXOSEL[9]; // NOT used on TCC9300
    UHDMICTRL       uHDMICTRL;
    UPWDN           uPWDN;
    USWRESET        uSWRESET;
    UONTHEFLY       uONTHEFLY;
    UHDMIAES        uHDMIAES;
    SHDMIAESDATA    uHDMIAESDATA;
}SDDICONFIG, *PSDDICONFIG;






/**************************************************************************************************
*       DDI_Cache
***************************************************************************************************/
typedef union _ddicctrl{
    unsigned int ddicctrl;
    struct{
        unsigned int ddic0_en:6;
        unsigned int reserved0:9;
        unsigned int ddic1_en:6;
        unsigned int reserved1:9;
    }B;
}UDDICCTRL, *PUDDICCTRL;


typedef union _ddiccfg0{
        unsigned int ddiccfg0;
        struct{
            unsigned int sel00:4;
            unsigned int sel01:4;
            unsigned int sel02:4;
            unsigned int sel03:4;
            unsigned int sel04:4;
            unsigned int sel05:4;
            unsigned int reserved:8;
        }B;
}UDDICCFG0,  *PUDDICCFG0;

typedef union _ddiccfg1{
        unsigned int ddiccfg1;
        struct{
            unsigned int sel06:4;
            unsigned int sel07:4;
            unsigned int sel08:4;
            unsigned int sel09:4;
            unsigned int sel10:4;
            unsigned int sel11:4;
            unsigned int sel12:4;
            unsigned int sel13:4;
        }B;
}UDDICCFG1,  *PUDDICCFG1;

typedef struct _ddicache{
    UDDICCTRL   uDDICCTRL;
    UDDICCFG0   uDDICCFG00;
    UDDICCFG1   uDDICCFG01;
    UDDICCFG0   uDDICCFG10;
    UDDICCFG1   uDDICCFG11;
}SDDICACHE, *PSDDICACHE;

/**************************************************************************************************
***************************************************************************************************/
/**************************************************************************************************
***************************************************************************************************/
/**************************************************************************************************
***************************************************************************************************/
/**************************************************************************************************
***************************************************************************************************/
/**************************************************************************************************
***************************************************************************************************/
/**************************************************************************************************
***************************************************************************************************/
/**************************************************************************************************
***************************************************************************************************/


/**************************************************************************************************
*   int tca_ddicfg_ntscpalselect(int port)
***************************************************************************************************/
extern int tca_ddicfg_ntscpalselect(int port);
extern int tca_ddicfg_getntscpalselect(int* psel);
extern int tca_ddicfg_lvdsselect(int port);
extern int tca_ddicfg_getlvdsselect(int* psel);
extern int tca_ddicfg_lvdsreset(int flag);
extern int tca_ddicfg_lvdsenable(int flag);
extern int tca_ddicfg_getlvdsenable(int* pflag);
extern int tca_ddicfg_lvdstxoselection(void);
extern int tca_ddicfg_hdmireset(int flag);
extern int tca_ddicfg_swresetddibus(int flag);
extern int tca_ddicfg_ddibuspwrdnstate(unsigned int* pstate);
extern int tca_ddicfg_pwdnddibus(int flag, int onoff);
extern int tca_ddicfg_sethdmiselect(int flag);
extern int tca_ddicfg_gethdmiselect(int* psel);
extern int tca_ddicfg_hdmienable(int flag);
extern int tca_ddicfg_gethdmienable(int* pflag);
extern int tca_ddicfg_clockcontrol(int param);
extern int tca_ddicfg_interface(int function, int param);
extern int tca_ddicfg_setddicachecontrol(int id, int sel, int onoff);
extern int tca_ddicfg_getddicachecontrol(int id, int sel, int* ponoff);
extern int tca_ddicfg_setddicacheconfig(int id, int sel, int value);
extern int tca_ddicfg_getddicacheconfig(int id, int sel, int* pvalue);
extern int tca_ddicfg_setdefault(void);




#ifdef __cplusplus
}
#endif

