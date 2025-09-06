#ifndef __OID_H__
#define __OID_H__

#define MAX_NUMBER_OF_MAC   116 // if MAX_MBSSID_NUM is 8, this value can't be larger than 211

typedef union _MACHTTRANSMIT_SETTING {
	struct {
		unsigned short MCS:6;
		unsigned short ldpc:1;
		unsigned short BW:2;
		unsigned short ShortGI:1;
		unsigned short STBC:1;
		unsigned short eTxBF:1;
		unsigned short iTxBF:1;
		unsigned short MODE:3;
	} field;
	unsigned short word;
} MACHTTRANSMIT_SETTING, *PMACHTTRANSMIT_SETTING;

typedef struct _RT_802_11_MAC_ENTRY {
	unsigned char	ApIdx;
	unsigned char	Addr[ETHER_ADDR_LEN];
	unsigned char	Aid;
	unsigned char	Psm;     // 0:PWR_ACTIVE, 1:PWR_SAVE
	unsigned char	MimoPs;  // 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled
	char		AvgRssi0;
	char		AvgRssi1;
	char		AvgRssi2;
	unsigned int	ConnectedTime;
	MACHTTRANSMIT_SETTING	TxRate;
	unsigned int	LastRxRate;
//#if (defined (USE_MT7615_AP) || defined(USE_MT7613_AP) || defined(USE_MT7915_AP)) && !defined (WITHOUT_KERNEL)
	signed short	StreamSnr[3];
	signed short	SoundingRespSnr[3];
//#endif
} RT_802_11_MAC_ENTRY, *PRT_802_11_MAC_ENTRY;

typedef struct _RT_802_11_MAC_TABLE {
	unsigned long Num;
	RT_802_11_MAC_ENTRY Entry[MAX_NUMBER_OF_MAC];
} RT_802_11_MAC_TABLE, *PRT_802_11_MAC_TABLE;

/* RT3352 iNIC_mii MAC_TABLE */
typedef union _MACHTTRANSMIT_SETTING_INIC {
	struct {
		unsigned short MCS:7;
		unsigned short BW:1;
		unsigned short ShortGI:1;
		unsigned short STBC:2;
		unsigned short eTxBF:1;
		unsigned short rsv:1;
		unsigned short iTxBF:1;
		unsigned short MODE:2;
	} field;
	unsigned short word;
} MACHTTRANSMIT_SETTING_INIC, *PMACHTTRANSMIT_SETTING_INIC;

typedef struct _RT_802_11_MAC_ENTRY_INIC {
	unsigned char	ApIdx;
	unsigned char	Addr[ETHER_ADDR_LEN];
	unsigned char	Aid;
	unsigned char	Psm;     // 0:PWR_ACTIVE, 1:PWR_SAVE
	unsigned char	MimoPs;  // 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled
	char		AvgRssi0;
	char		AvgRssi1;
	char		AvgRssi2;
	unsigned int	ConnectedTime;
	MACHTTRANSMIT_SETTING_INIC	TxRate;
} RT_802_11_MAC_ENTRY_INIC, *PRT_802_11_MAC_ENTRY_INIC;

typedef struct _RT_802_11_MAC_TABLE_INIC {
	unsigned long Num;
	RT_802_11_MAC_ENTRY_INIC Entry[MAX_NUMBER_OF_MAC];
} RT_802_11_MAC_TABLE_INIC, *PRT_802_11_MAC_TABLE_INIC;

typedef struct _SITE_SURVEY 
{
	char channel[4];
	unsigned char ssid[33];
	char bssid[20];
	char security[23];
	char signal[9];
	char wmode[7];
	char extch[7];
	char bsstype[3];
} SITE_SURVEY;

typedef struct _SITE_SURVEY_ARRAY
{
	SITE_SURVEY SiteSurvey[64];
} SSA;

#define SITE_SURVEY_APS_MAX	(16*1024)

typedef struct _PAIR_CHANNEL_FREQ_ENTRY
{
	unsigned long   lChannel;
	unsigned long   lFreq;
} PAIR_CHANNEL_FREQ_ENTRY, *PPAIR_CHANNEL_FREQ_ENTRY;


//#if WIRELESS_EXT <= 11 
//#ifndef SIOCDEVPRIVATE 
#define SIOCDEVPRIVATE 0x8BE0 
//#endif 
#define SIOCIWFIRSTPRIV SIOCDEVPRIVATE 
//#endif 
//
//SET/GET CONVENTION :
// * ------------------
// * Simplistic summary :
// * o even numbered ioctls are SET, restricted to root, and should not
// * return arguments (get_args = 0).
// * o odd numbered ioctls are GET, authorised to anybody, and should
// * not expect any arguments (set_args = 0).
//
#define RT_PRIV_IOCTL			(SIOCIWFIRSTPRIV + 0x01)
#define RTPRIV_IOCTL_SET		(SIOCIWFIRSTPRIV + 0x02)
#define RTPRIV_IOCTL_GSITESURVEY	(SIOCIWFIRSTPRIV + 0x0D)
#define RTPRIV_IOCTL_WSC_PROFILE	(SIOCIWFIRSTPRIV + 0x12)
#define	RTPRIV_IOCTL_GSTAINFO		(SIOCIWFIRSTPRIV + 0x1A)
#define	RTPRIV_IOCTL_GSTAT		(SIOCIWFIRSTPRIV + 0x1B)
#define RTPRIV_IOCTL_GRSSI		(SIOCIWFIRSTPRIV + 0x1C)
#define RTPRIV_IOCTL_GTXBFCALP		(SIOCIWFIRSTPRIV + 0x1D)
#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT	(SIOCIWFIRSTPRIV + 0x1F)
#define OID_802_11_DISASSOCIATE		0x0114
#define OID_802_11_BSSID_LIST_SCAN	0x0508
#define OID_802_11_SSID			0x0509
#define OID_802_11_BSSID		0x050A
#define RT_OID_802_11_RADIO		0x050B
#define RT_OID_802_11_PHY_MODE		0x050C
#define OID_802_11_BSSID_LIST		0x0609
#define OID_802_3_CURRENT_ADDRESS	0x060A
#define OID_GEN_MEDIA_CONNECT_STATUS	0x060B
#define RT_OID_GET_PHY_MODE		0x0761
#define OID_GET_SET_TOGGLE		0x8000
#define RT_OID_SYNC_RT61		0x0D010750
#define RT_OID_WSC_QUERY_STATUS		((RT_OID_SYNC_RT61 + 0x01) & 0xffff)
#define RT_OID_WSC_PIN_CODE		((RT_OID_SYNC_RT61 + 0x02) & 0xffff)

#endif
