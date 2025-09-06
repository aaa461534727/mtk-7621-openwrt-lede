#ifndef __CS_MODEM_HEADER__
#define __CS_MODEM_HEADER__

//#include <telephony/ril.h>

#define MODEM_IF_X           "ccinet"
#define MODEM_PROTO          "rildata"

typedef enum
{
	AUTH_NONE,
	AUTH_PAP,
	AUTH_CHAP,
	AUTH_PAPCHAP
}AUTH_T;

enum {
	MODEM_PRIO_ONLY_WIRE = 0,
	MODEM_PRIO_ONLY_3G,
	MODEM_PRIO_ONLY_4G,
	MODEM_PRIO_ONLY_5G,
	MODEM_PRIO_WIRE_FRIST,
	MODEM_PRIO_LTE_FRIST
};


enum {
	MODEM_STATUS_NO = 0,
	MODEM_STATUS_3G,
	MODEM_STATUS_4G,
	MODEM_STATUS_5G 
};

enum {
	MODEM_REG_STATUS_REG_HOME   = 1,
	MODEM_REG_STATUS_NO_SIM     = 10,
	MODEM_REG_STATUS_PIN_LOCK   =20,
	MODEM_REG_STATUS_UN_REG,
};

enum {
	CS_SIM_UNKOWN,
	CS_SIM_DETECTED,
	CS_SIM_PIN,
	CS_SIM_PUK,
	CS_UNKOWN, // 临时填充，暂时不清楚这个值为什么状态
	CS_SIM_READY
};

typedef struct _modem_info {

	int reg_status;
	int sim_status;
	char net_type[16];

	char imei[32];
	char imsi[32];

	char arfcn[16];
	char pci[16];
	char cellid[16];
	char rsrp[16];
	char rsrq[16];
	char sinr[16];
	char band[16];
	char signal[16];
	char isp[16];
	char ip[32];
	char version[32];

	//only support lte
	char channel4g[16];
	char rssi4g[16];
} modem_info_t;

#define AT_TIMEOUT              3
#define SERIAL_PORT_NAME        "/tmp/atcmdni1"

struct atcmd_call_st {
	int serial_fd;
	int rsp_len;
	char *rsp_data;
};

extern int send_atcmd(char *at_cmd,char *rsp_data,int rsp_len);
extern void set_modem_flight(void);

#endif /* __CS_MODEM_HEADER__ */
