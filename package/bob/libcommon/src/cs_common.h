#ifndef __CS_COMMON_HEADER__
#define __CS_COMMON_HEADER__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/ioctl.h> 
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <assert.h>
#include <cJSON.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/sysinfo.h>
#include <syslog.h>
#include <stdarg.h>
#include "cs_uci.h"
#include "cs_uci_fun.h"
#include "wifi.h"
#include <libdatconf.h>
#include <ifaddrs.h>

enum {
	GPIO_OFF,
	GPIO_ON,
};
	
#define CAPTCHA_TRY_LIMIT 3
#define CAPTCHA_LIMIT_TIMEOUT 180
#define GPIO_MAX 1000

#if defined(CONFIG_USER_CSTE_PRINT_CMD)
    #define CSTE_PRINT_CMD       1
#else
    #define CSTE_PRINT_CMD       0	
#endif

/*platform len macro*/
#define SMALL_STR_LEN                                    8
#define SHORT_STR_LEN                                    16
#define RESULT_STR_LEN                                   32
#define OPTION_STR_LEN                                   64
#define TEMP_STR_LEN                                     128
#define CMD_STR_LEN                                      256
#define LIST_STR_LEN                                     512
#define LONG_BUFF_LEN                                    1024
#define LONGLONG_BUFF_LEN                                8192

#define MAX_WAN_NUM							4
#define MAX_ADDR_LEN						6
#define FILTER_RULE_NUM_MAX 				16
#define ETHER_ADDR_LEN						6


#define	T(s) 				                             s 

#define HACSQLPATH                                       "/tmp/hacs/hacs.sqlite3"
#define HACSQLPATH_BAK                                   "/userdata/hacs/hacs.sqlite3"
#define LOCK_CHECKABNORMAL                               "/tmp/lock.cAbnormal"

#define APP_CONFIG_PATH                                  "/userdata/ibms_config"
#define APP_CONFIG_TEMP_PATH                             "/tmp/ibms_config/user"
#define APP_CONFIG_TEMP_LIST_PATH                        "/tmp/ibms_config/user/applist"

#define DL_IMAGE_FILE                                    "/tmp/cloudupdate.web"
#define AUTOUPDATE_SH									 "/tmp/autoupdate.sh"
#define UPDATE_SH										 "/tmp/update.sh"

#define PING_TMPFILE                                     "/tmp/.ping_success"
#define TCP_TMPFILE "/tmp/.tcpcheck.tmp"
#define CHECK_TIMEOUT 1
#define MQTT_SRV_PORT 1883

#define SOCK_CONNECT_TIMEOUT 3
#define SOCK_RW_TIMEOUT 5
#define SAFE_CLOSE(fd)				if(fd > 0)	{ close(fd); fd = -1; }
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


/* file lock */
#define lock_net_handler_f                               "/tmp/nethandler.lock"

#define LAN_DEV_NAME                                     "br-lan"
#define LOCAL_LAN_DEV_NAME                               "br-lan:1"
#define LOCAL_LAN_DEV_IPADDR                             "169.254.0.1"

#define WAN_STATIC_MODE									 "static"
#define WAN_USB_NET_IFNAME	                             "usb0"


#define PAGE_WPS_SIGN 89

#define PRODUCT_FILE       	"/etc/product"
#define CUSTOM_FILE       	"/userdata/product"
#define WEBDIR       		"/userdata/web"
#define CUR_WEB_DIR  		"/web"



#define WAN_MODE_STATIC                                  "0"
#define WAN_MODE_DHCP                                    "1"
#define WAN_MODE_PPPOE                                   "3"
#define WAN_MODE_PPTP                                    "4"
#define WAN_MODE_L2TP                                    "6"
#define WAN_MODE_3G	                                     "3G"
#define WAN_MODE_USB_NET                                 "7"

typedef enum
{
	WIRE_FRIST=0,
	ONLY_4G,
	ONLY_WIRE
}WanStrategy;


#define OPMODE_BRIDGE_STR                                "0"
#define OPMODE_GATEWAY_STR                               "1"
#define OPMODE_REPEATER_STR                              "2"
#define OPMODE_WISP_STR                                  "3"
#define OPMODE_MESH_STR                                  "4"
#define OPMODE_CLIENT_STR                                "5"

#define WIRE_FRIST_STR									 "0"
#define ONLY_4G_STR									 	 "1"
#define ONLY_WIRE_STR									 "2"

#define DISABLE		                                     "0"
#define ENABLE		                                     "1"

#ifdef CONFIG_RALINK_SOC_MT7621
#define ETH_IFNAME	"eth0 eth1"
#define WAN_IFNAME	"eth1"
#define LAN_IFNAME	"eth0"
#define WAN_X_IFNAME "eth1"
#else
#define ETH_IFNAME	"eth0.1 eth0.2"
#define WAN_IFNAME	"eth0.2"
#define WAN2_IFNAME	"eth0.3"
#define WAN3_IFNAME	"eth0.4"
#define WAN4_IFNAME	"eth0.5"
#define LAN_IFNAME	"eth0.1"
#endif
#define WAN_PPPOE_IFNAME								"pppoe-wan"

#define WAN4G_IFNAME									"eth1"


//#ifdef WIFI_SUPPORT

#define W24G_PATH  						"/etc/wireless/mediatek/mt7915.dbdc.b0.dat"
#define W58G_PATH  						"/etc/wireless/mediatek/mt7915.dbdc.b1.dat"
#define ANDLINK_PATH  					"/etc/mtk_andlink/mtk_andlink_store.cfg"

#define WIFI_2G_CONF_FILE 				W24G_PATH
#define WIFI_5G_CONF_FILE 				W58G_PATH

#define MESH_INFO_FILE					"/tmp/dump.txt"
#define MAPD_CONF_FILE					"/etc/map/mapd_cfg"
#define MAPD_DEF_CONF_FILE				"/etc/map/mapd_default.cfg"
//#define MAPD_CS_DEF_CONF_FILE			"/etc/map/mapd_def"
#define MAPD_CONTROLLER_DEF_CONF_FILE 	"/etc/map/mapd_controller_default.cfg"
#define MAPD_AGENT_DEF_CONF_FILE 		"/etc/map/mapd_agent_default.cfg"
#define MAPD_USER_CONF_FILE 			"/etc/map/mapd_user.cfg"
#define MAPD_1905D_CONF_FILE 			"/etc/map/1905d.cfg"

#define MAPD_AGENT_USER_CONF_FILE 		"/etc/map/agent_user.cfg"

#define MAPD_CTRL_FILE					"/tmp/mapd_ctrl"
#define BSS_CONF_FILE					"/etc/map/wts_bss_info_config"

#define TEMP_STATUS_FILE				"/var/cste/temp_status"

typedef enum role{
	DEV_AUTO = 0,
	DEV_CONTROLLER,
	DEV_AGENT,
}DEV_ROLE;

#define LMOVE(m,n) ((m)<<(n))
#define RMOVE(m,n) ((m)>>(n))

#ifdef WIFI_MT7628_SUPPORT
#define W24G_DEV "mt7628"
#elif defined(WIFI_MT7620_SUPPORT)
#define W24G_DEV "mt7620"
#elif defined(WIFI_MT7602E_SUPPORT)
#define W24G_DEV "mt7602e"
#elif defined(WIFI_MT7603E_SUPPORT)
#define W24G_DEV "mt7603e"
#elif defined(WIFI_MT7615E_SUPPORT)
#define W24G_DEV "MT7615_1"
#else
#define W24G_DEV ""
//#error "undef WiFi 2G"
#endif

#ifdef WIFI_MT7612E_SUPPORT
#define W58G_DEV "mt7612e"
#elif defined(WIFI_MT7610E_SUPPORT)
#define W58G_DEV "mt7610e"
#elif defined(WIFI_MT7615E_SUPPORT)
#define W58G_DEV "MT7615_2"
#else
#define W58G_DEV ""
//#error "undef WiFi 5G"
#endif

#define W24G_IF "ra0"
#define W24G_IF_VA1 "ra1"
#define W24G_IF_VA2 "ra2"
#define W24G_IF_VA3 "ra3"
#define W24G_IF_VA4 "ra4"
#define W24G_IF_VA5 "ra5"

#define W58G_IF "rax0"
#define W58G_IF_VA1 "rax1"
#define W58G_IF_VA2 "rax2"
#define W58G_IF_VA3 "rax3"
#define W58G_IF_VA4 "rax4"
#define W58G_IF_VA5 "rax5"

#define W24G_APCLI_IF "apcli0"
#define W58G_APCLI_IF "apclix0"

#define W24G_WDS_IF "wds0"
#define W58G_WDS_IF "wds1"

#if defined(CONFIG_ANDLINK_SUPPORT) || defined(CONFIG_USER_NETCWMP)
#define WLAN_IF_NUM 4
#else
#define WLAN_IF_NUM 5
#endif


//#endif

/* Print directly to the console */
#define dbg(fmt, args...) do { FILE *fp = fopen("/dev/console", "w"); if (fp) { fprintf(fp, fmt, ## args); fclose(fp); } else fprintf(stderr, fmt, ## args); } while (0)
#define dbG(fmt, args...) dbg("[%s:%d] " fmt , __FUNCTION__ , __LINE__, ## args)

#define CSTE_DBG(format, args...) do{\
	if (CSTE_PRINT_CMD==1){\
		FILE *fp=fopen("/var/log/cste_debug.log", "a");\
		if(fp){fprintf(fp, "[%s:%d]=> " format,__FUNCTION__, __LINE__, ##args); fclose(fp);}\
	}}while(0)

#define XCMD( x, fmt, args... ) do {\
	sprintf( x, fmt, ##args ); \
	CsteSystem( x, CSTE_PRINT_CMD ); \
} while( 0 )


#define CSTE_LOG(format, args...) do{\
	FILE *fp=fopen("/var/log/cs_info.log", "a");\
	if(fp){fprintf(fp, format, ##args); fclose(fp);}\
	}while(0)

typedef unsigned short 		 char_t;

typedef enum
{
	OPMODE_BRIDGE_INT=0,
	OPMODE_GATEWAY_INT,
	OPMODE_REPEATER_INT,
	OPMODE_WISP_INT,
	OPMODE_MESH_INT,
	OPMODE_CLIENT_INT
}OPMODE;

typedef enum
{
	PLATFORM_UNDEFINED=0,
	RALINK3052_STD,
	RALINK3050_STD,
	RALINK5350_STD,
	MTK7620_STD,
	MTK7628_STD,
	MTK7621_STD,
	MTK305x_STD,
	PLATFORM_UNKNOW
}PlatformType;

enum{
	UPG_FORCE_CHECK=0,
	UPG_UNNET,
	UPG_LATEST,
	UPG_CHECKING,
	UPG_NEW,
	UPG_FORCE_UPGRADEING
}UPG_STATUS;

typedef enum {
	DHCP_DISABLED = 0,
	DHCP_CLIENT = 1,
	DHCP_SERVER = 2,
	PPPOE = 3,
	PPTP = 4,
	DHCP_RELAY = 5,
	L2TP = 6
} DHCP_T;


typedef enum
{
	WIFI_EASY=1,
	WIFI_GUEST_2G,
	WIFI_GUEST_5G,
	WIFI_UNKNOW
}WLAN_EFFECT_ID;

struct interface_status 
{
	int up;
	int uptime;
	char proto[8];
	char device[8];
	char ipaddr_v4[16];
	char mask_v4[16];
	char gateway_v4[16];
	char pri_dns_v4[16];
	char sec_dns_v4[16];
};


#define USERDATA_PATH				"/userdata"

#define IH_MAGIC    0x27051956
#define IH_NMLEN    32

typedef struct image_header {
    uint32_t    ih_magic;   /* Image Header Magic Number    */
    uint32_t    ih_hcrc;    /* Image Header CRC Checksum    */
    uint32_t    ih_time;    /* Image Creation Timestamp */
    uint32_t    ih_size;    /* Image Data Size      */
    uint32_t    ih_load;    /* Data  Load  Address      */
    uint32_t    ih_ep;      /* Entry Point Address      */
    uint32_t    ih_dcrc;    /* Image Data CRC Checksum  */
    uint8_t     ih_os;      /* Operating System     */
    uint8_t     ih_arch;    /* CPU architecture     */
    uint8_t     ih_type;    /* Image Type           */
    uint8_t     ih_comp;    /* Compression Type     */
    uint8_t     ih_name[IH_NMLEN];  /* Image Name       */
} image_header_t;
typedef struct custom_header {
	uint32_t    ih_size;    /* Image Data Size      */
	uint8_t     ih_md5[40];  /* md5       */
    uint8_t     ih_name[32];  /* Image Name       */
} custom_header_t;

#define IH3_MAGIC	0x34D58CA2
typedef struct image3_header {
	unsigned int magic;
	unsigned char name[16];
	int version;
	unsigned int flashStart;
	unsigned int flashEnd;
	unsigned int dataSize;
	unsigned int hcrc;
	unsigned int dcrc;
} image3_header_t;

/* 
 * Concatenates NULL-terminated list of arguments into a single
 * commmand and executes it
 * @param	argv	argument list
 * @param	path	NULL, ">output", or ">>output"
 * @param	timeout	seconds to wait before timing out or 0 for no timeout
 * @param	ppid	NULL to wait for child termination or pointer to pid
 * @return	return value of executed command or errno
 */
extern void hook_list_init(void);
//extern int cs_cmd(char *cmd, char *action, int sleep_sec);
extern int _eval(char *const argv[], char *path, int timeout, int *ppid);
extern int CsteSystem(char *command, int printFlag);
extern pid_t get_pid_by_name(char *name);
extern char *websGetVar(cJSON *object, char *var, char *defaultGetValue);
extern int isMacValid(char *str);
extern int isIpValid(char *str);
extern int isIpNetmaskValid(char *s);
extern int getRuleNums(char *rules);
extern int getNthValueSafe(int index, char *value, char delimit, char *result, int len);
extern int lock_cs_mtd(void);
extern int unlock_cs_mtd(void);
extern void setLktosEffect(char *action);
extern int is_phyconnected(int portNum);
extern int tcpcheck_net(const char *host, int port, int timeout);
extern int do_ping_detect(const char *host, char *ifname);
extern int getCmdResult(char *cmd, char *resultbuf, size_t buf_size);
extern int getCmdVal(const char *cmd);
extern char* getLanIfName(void);
extern int getVpnServerIp(char *ifname, char *if_addr);
extern int getIfBytes(const char *ifname, unsigned long long *rxb, unsigned long long *txb);
extern int getIfIp(char *ifname, char *if_addr);
extern int getIfMask(char *ifname, char *if_addr);
extern int getIfIsUp(char *ifname);
extern int getIfMac(char *ifname, char *if_hw);
extern unsigned int getFlashSizeCs(void);
extern int domain_to_ip(unsigned char *domain, unsigned char *ip);
extern int check_img_valid(char *imagefile, int offset, int len, char *err_msg);


// files.c
#define FW_CREATE	0
#define FW_APPEND	1
#define FW_NEWLINE	2
#define FW_SILENT	4	/* Don't print error message even write file fail. */

#define ACTION_LOCK_FILE "/var/lock/a_w_l" // action write lock

extern int check_if_dir_empty(const char *dirpath);
extern int file_lock(const char *tag);
extern void file_unlock(int lockfd);
extern unsigned long f_size(const char *path);
extern int f_exists(const char *file);
extern int d_exists(const char *path);
extern int f_read_excl(const char *path, void *buffer, int max);
extern int f_read(const char *file, void *buffer, int max);												// returns bytes read
extern int f_write_excl(const char *path, const void *buffer, int len, unsigned flags, unsigned cmode);
extern int f_write(const char *file, const void *buffer, int len, unsigned flags, unsigned cmode);		//
extern int f_read_string(const char *file, char *buffer, int max);										// returns bytes read, not including term; max includes term
extern int f_write_string(const char *file, const char *buffer, unsigned flags, unsigned cmode);		//
extern int f_read_alloc(const char *path, char **buffer, int max);
extern int f_read_alloc_string(const char *path, char **buffer, int max);
extern int f_read_offset(const char *path, void *buffer, int offset, int max);
extern int f_wait_exists(const char *name, int max);
extern int f_wait_notexists(const char *name, int max);
extern int lock_file(const char * lock_file);
extern int unlock_file(const char * lock_file);
extern unsigned long long getLongValFromFile(char* name);
extern void getStrFromFile(char* path, char* tmpbuf);
extern int getFileVal(const char *path);
extern long get_current_uptime_sec(void);
extern void str_del_char_bak(char *a,char c);
extern void add_mac_split(const char *mac_org, char *mac_new);
extern int get_sta_ipaddr_bymac(char *pmac, char *ipv4_addr, char *ipv6_addr);
extern int get_sta_hostname_bymac(char *pmac, char *hostname);

extern void STATUS_WAN_LINK_GET(int idx, int *value);

extern void STATUS_WAN_IP_ADDRESS_GET(int idx, char *value);

extern void STATUS_SUBNET_MASK_GET(int idx, char *value);

extern void STATUS_WAN_DEFAULT_GW_GET(int idx, char *value);

extern void STATUS_WAN_DNS_SERVER_GET(int idx, char *value);

extern void STATUS_WAN_IF_NAME_GET(int idx, char *value);

extern int cs_ubus_cli_call(char *path, char *method, char *ptr);
extern void logmessage(char *logheader, char *fmt, ...);
extern int wificonf_set_by_key(char *path,char *key,char *value);
extern int wificonf_get_by_key(char *path,char *key,char *value,int len);
extern int wificonf_set_by_ifname_key(char *ifname,char *key,char *value);
extern int wificonf_get_by_ifname_key(char *ifname,char *key,char *value,int len);
extern char *get_ifaddr6(const char *ifname, int linklocal, char *p_addr6s);
extern int get_interface_status(struct interface_status *status_paremeter,char *interface);
extern int is_interface_exist(const char *ifname);
extern int is_interface_up(const char *ifname);
extern int is_wan_up(void);
extern int get_wanmode_int(char *proto);
extern int get_wan_ifname(char *ifanem);
extern int wl_ioctl(const char *ifname, int cmd, struct iwreq *pwrq);
extern int mask_num2string(int num, char *mask_buf, int buf_len);
extern int mask_string2num(char *mask);
extern int ether_atoe(const char *a, unsigned char *e);
extern char *ether_etoa(const unsigned char *e, char *a);
extern char *ether_etoa2(const unsigned char *e, char *a);
extern char *ether_etoa3(const unsigned char *e, char *a);
extern void inc_mac(unsigned char *mac, int offset);
extern void getRealGateway(char  *sgw);
extern int getDns(int dnsIdx, char *dns, int is_ipv6);

extern void set_timezone_to_kernel(void);

extern void urldecode(char url[], char *result);
extern void urlencode(char url[], char *result);

extern int mtd_dev_idx(const char *mtd_part);
extern int get_apcli_connected(const char *ifname);
extern int get_apcli_connect_ssid(const char *ifname,char *ssid,int len);
extern int get_apcli_connect_bssid(const char *ifname,char *bssid,int len);
extern int get_apcli_connect_noise(const char *ifname,char *noise,int len);
extern int get_apcli_connect_rssi(const char *ifname,char *rssi,int len);
extern int get_apcli_ioctl_connected(char *apcli_x);
extern int str_toupper(char *src);

int setBssConfig(int idx,char *band,char *ssid,char *auth,char *encryption,char *passwd,int hidden);
int getChannel(char *ifname, char *channel);
void getVersion(char *version);
int Validity_check(char *tmpbuf);

extern void led_system_init(void);
extern void led_reset_blink(void);
extern void led_start_mesh_wps(void);
extern void led_schedule_control();

extern int get_cjson_string(cJSON *object, char *key,  char *val, int len);



#endif /* __CS_COMMON_HEADER__ */

